/*
Created:        --
Last Updated:   29 August    2017

Dan Marley
daniel.edison.marley@cernSPAMNOT.ch
Texas A&M University

-----

Make histograms for systematic uncertainties (& nominal) 
to go into plots || TRexFitter

*/
#include "Analysis/CyMiniAna/interface/histogrammerFlatNtuple.h"


histogrammerFlatNtuple::histogrammerFlatNtuple( configuration& cmaConfig, std::string name ) :
  m_config(&cmaConfig),
  m_name(name),
  m_putOverflowInLastBin(true),
  m_putUnderflowInFirstBin(true){
    m_map_histograms1D.clear();
    m_map_histograms2D.clear();
    m_map_histograms3D.clear();

    m_isMC  = m_config->isMC();

    if (m_name.length()>0  && m_name.substr(m_name.length()-1,1).compare("_")!=0)
        m_name = m_name+"_"; // add '_' to end of string, if needed
  }

histogrammerFlatNtuple::~histogrammerFlatNtuple() {}


/**** INITIALIZE HISTOGRAMS ****/

// -- 1D Histograms
void histogrammerFlatNtuple::init_hist( const std::string& name, const unsigned int nBins, const double x_min, const double x_max ){
    /* Initialize histogram -- equal bins */
    m_map_histograms1D["h_"+name] = new TH1D(("h_"+name).c_str(), ("h_"+name).c_str(),nBins,x_min,x_max);
    m_map_histograms1D["h_"+name]->Sumw2();

    return;
}
void histogrammerFlatNtuple::init_hist( const std::string& name, const unsigned int nBins, const double *xbins ){
    /* Initialize histogram -- variable bins */
    m_map_histograms1D["h_"+name] = new TH1D(("h_"+name).c_str(), ("h_"+name).c_str(),nBins,xbins);
    m_map_histograms1D["h_"+name]->Sumw2();

    return;
}
// -- 2D Histograms
void histogrammerFlatNtuple::init_hist( const std::string& name, const unsigned int nBinsX, const double x_min, const double x_max,
                              const unsigned int nBinsY, const double y_min, const double y_max ){
    /* Initialize histogram -- equal bins */
    m_map_histograms2D["h_"+name] = new TH2D(("h_"+name).c_str(), ("h_"+name).c_str(),
                                            nBinsX,x_min,x_max,nBinsY,y_min,y_max);
    m_map_histograms2D["h_"+name]->Sumw2();

    return;
}
void histogrammerFlatNtuple::init_hist( const std::string& name, const unsigned int nBinsX, const double *xbins,
                              const unsigned int nBinsY, const double *ybins ){
    /* Initialize histogram -- variable bins */
    m_map_histograms2D["h_"+name] = new TH2D(("h_"+name).c_str(), ("h_"+name).c_str(),
                                           nBinsX,xbins,nBinsY,ybins);
    m_map_histograms2D["h_"+name]->Sumw2();

    return;
}
// -- 3D Histograms
void histogrammerFlatNtuple::init_hist( const std::string& name, const unsigned int nBinsX, const double x_min, const double x_max,
                              const unsigned int nBinsY, const double y_min, const double y_max,
                              const unsigned int nBinsZ, const double z_min, const double z_max ){
    /* Initialize histogram -- equal bins */
    m_map_histograms3D["h_"+name] = new TH3D(("h_"+name).c_str(), ("h_"+name).c_str(),
                                            nBinsX,x_min,x_max,nBinsY,y_min,y_max,nBinsZ,z_min,z_max);
    m_map_histograms3D["h_"+name]->Sumw2();

    return;
}
void histogrammerFlatNtuple::init_hist( const std::string& name, const unsigned int nBinsX, const double *xbins,
                              const unsigned int nBinsY, const double *ybins,
                              const unsigned int nBinsZ, const double *zbins ){
    /* Initialize histogram -- variable bins */
    m_map_histograms3D["h_"+name] = new TH3D(("h_"+name).c_str(), ("h_"+name).c_str(),
                                           nBinsX,xbins,nBinsY,ybins,nBinsZ,zbins);
    m_map_histograms3D["h_"+name]->Sumw2();

    return;
}


void histogrammerFlatNtuple::initialize( TFile& outputFile, bool doSystWeights ){
    /* Setup some values and book histograms */
    m_doSystWeights = doSystWeights;
    outputFile.cd();


    // loop over treenames (typically systematic uncertainties)
    for (const auto& treename : m_config->treeNames() )
        bookHists( m_name+treename );

    // weight systematics
    if (m_isMC && m_doSystWeights){
        // In ATLAS these were only necessary for the nominal tree.
        // Do not need to make them for every systematic variation!
        for (const auto& syst : m_config->listOfWeightSystematics()){
            bookHists( m_name+syst );
        } // end weight systematics

        // vector weight systematics
        for (const auto& syst : m_config->mapOfWeightVectorSystematics()){
            for (unsigned int el=0;el<syst.second;++el){
                std::string weightIndex = std::to_string(el);
                bookHists( m_name+weightIndex+"_"+syst.first );
            } // end components of vector
        } // end vector weight systematics
    } // end if MC and save weight systematics
}

void histogrammerFlatNtuple::bookHists( std::string name ){
    /* 
      Book histograms -- modify/inherit this function for analysis-specific hists 

      @param name   This is the string used to identify histograms for different systematics/event weights
    */
    m_names.resize(0); // append names to this to keep track of later

    if (m_config->useJets()){
        init_hist("n_jets_"+name,   31, -0.5,  30.5);
        init_hist("n_btags_"+name,  11, -0.5,  10.5);

        init_hist("jet1_pt_"+name,  500, 0.0, 2000);
        init_hist("jet1_eta_"+name,  50, -2.5, 2.5);
        init_hist("jet1_phi_"+name,  64, -3.2, 3.2);
        init_hist("jet1_cMVAv2_"+name, 200, -1,1);
        init_hist("jet2_pt_"+name,  500, 0.0, 2000);
        init_hist("jet2_eta_"+name,  50, -2.5, 2.5);
        init_hist("jet2_phi_"+name,  64, -3.2, 3.2);
        init_hist("jet2_cMVAv2_"+name, 200, -1,1);
    }

    if (m_config->useLeptons()){
        init_hist("lep_pt_"+name,  500, 0.0, 2000);
        init_hist("lep_eta_"+name,  50, -2.5, 2.5);
        init_hist("lep_phi_"+name,  64, -3.2, 3.2);
    }

    if (m_config->useNeutrino()){
        init_hist("nu_pt_"+name,  500, 0.0, 2000);
        init_hist("nu_eta_"+name,  50, -2.5, 2.5);
        init_hist("nu_phi_"+name,  64, -3.2, 3.2);
    }

    // kinematics
    init_hist("met_met_"+name, 500,  0.0,  500);
    init_hist("met_phi_"+name, 6.4, -3.2,  3.2);
    init_hist("ht_"+name,     5000,  0.0, 5000);

    // HME && DNN && AMWT
    //init_hist("hme_"+name, 2000, 0.0, 2000);
    //init_hist("dnn_"+name,  100, 0.0,   1.);

/*  VLQ/Wprime
    init_hist("top_pt_"+name,  2000, 0.0, 2000);
    init_hist("top_eta_"+name,   50,-2.5,  2.5);
    init_hist("top_phi_"+name,   64,-3.2,  3.2);
    init_hist("top_m_"+name,    200, 100,  300);
    init_hist("antitop_pt_"+name,  2000, 0.0, 2000);
    init_hist("antitop_eta_"+name,   50,-2.5,  2.5);
    init_hist("antitop_phi_"+name,   64,-3.2,  3.2);
    init_hist("antitop_m_"+name,    200, 100,  300);

    init_hist("mttbar_"+name,  5000, 0.0, 5000);
    init_hist("pTttbar_"+name,  300, 0.0,  300);
*/
    return;
}




/**** FILL HISTOGRAMS ****/

void histogrammerFlatNtuple::fill( const std::string& name, const double& value, const double& weight ){
    /* TH1D */
    TH1D* this_hist = m_map_histograms1D.at("h_"+name);

    this_hist->Fill(value,weight);

    return;
}

void histogrammerFlatNtuple::fill( const std::string& name, 
                         const double& xvalue, const double& yvalue, const double& weight ){
    /* TH2D */
    TH2D* this_hist = m_map_histograms2D.at("h_"+name);

    this_hist->Fill(xvalue,yvalue,weight);

    return;
}

void histogrammerFlatNtuple::fill( const std::string& name, 
                         const double& xvalue, const double& yvalue, const double& zvalue, const double& weight ){
    /* TH3D */
    TH3D* this_hist = m_map_histograms3D.at("h_"+name);

    this_hist->Fill(xvalue,yvalue,zvalue,weight);

    return;
}


void histogrammerFlatNtuple::fill( Event& event ){
    /* Fill histograms -- fill histograms based on treename or systematic weights ("nominal" but different weight)
       This is the function to modify / inherit for analysis-specific purposes
    */
    std::string treeName = event.treeName();
    double event_weight  = event.nominal_weight();

    fill( m_name+treeName, event, event_weight );


    // if there are systematics stored as weights (e.g., b-tagging, pileup, etc.)
    // the following calls the fill() function with different event weights
    // to make histograms
    // In ATLAS, these weights only existed in the 'nominal' tree
    bool isNominal = m_config->isNominalTree( treeName );
    if (m_isMC && isNominal && m_doSystWeights){
        // weight systematics
        event_weight = 1.0;
        for (const auto& syst : m_config->listOfWeightSystematics()){
            event_weight = event.getSystEventWeight( syst );
            fill( m_name+syst, event, event_weight );
        } // end weight systematics

        // vector weight systematics
        event_weight = 1.0;
        for (const auto& syst : m_config->mapOfWeightVectorSystematics()){
            for (unsigned int el=0;el<syst.second;++el){
                event_weight = event.getSystEventWeight( syst.first, el );
                std::string weightIndex = std::to_string(el);

                fill( m_name+weightIndex+"_"+syst.first, event, event_weight );
            } // end components of vector
        } // end vector weight systematics
    } // end if nominal and doSystWeights

    return;
}


void histogrammerFlatNtuple::fill( const std::string& name, Event& event, double event_weight){
    /* Fill histograms -- just use information from the event and fill histogram
       This is the function to modify / inherit for analysis-specific purposes
    */
    cma::DEBUG("HISTOGRAMMERFLATNTUPLE : Fill histograms.");
    cma::DEBUG("HISTOGRAMMERFLATNTUPLE : event weight = "+std::to_string(event_weight) );

    if (m_config->useJets()){
        cma::DEBUG("HISTOGRAMMERFLATNTUPLE : Fill small-R jets");
        fill("n_btags_"+name, event.btag_jets().size(), event_weight );

        std::vector<Jet> jets = event.jets();
        fill("n_jets_"+name, jets.size(), event_weight );  // reflects the "nJetsL" variables, not 2

        fill("jet1_pt_"+name,  jets.at(0).p4.Pt(),   event_weight);
        fill("jet1_eta_"+name, jets.at(0).p4.Eta(),  event_weight);
        fill("jet1_phi_"+name, jets.at(0).p4.Phi(),  event_weight);
        fill("jet1_cMVAv2_"+name, jets.at(0).cMVAv2, event_weight);
        fill("jet2_pt_"+name,  jets.at(1).p4.Pt(),   event_weight);
        fill("jet2_eta_"+name, jets.at(1).p4.Eta(),  event_weight);
        fill("jet2_phi_"+name, jets.at(1).p4.Phi(),  event_weight);
        fill("jet2_cMVAv2_"+name, jets.at(1).cMVAv2, event_weight);
    }


    if (m_config->useLeptons()){
        cma::DEBUG("HISTOGRAMMERFLATNTUPLE : Fill leptons");
        std::vector<Lepton> leptons = event.leptons();
        fill("lep_pt_"+name,  leptons.at(0).p4.Pt(), event_weight);
        fill("lep_eta_"+name, leptons.at(0).p4.Eta(), event_weight);
        fill("lep_phi_"+name, leptons.at(0).p4.Phi(), event_weight);
    }


    if (m_config->useNeutrino()){
        cma::DEBUG("HISTOGRAMMERFLATNTUPLE : Fill neutrinos");
        std::vector<Neutrino> nus = event.neutrinos();

        fill("nu_pt_"+name,  nus.at(0).p4.Pt(),  event_weight);
        fill("nu_eta_"+name, nus.at(0).p4.Eta(), event_weight);
        fill("nu_phi_"+name, nus.at(0).p4.Phi(), event_weight);
    }


    // kinematics
    cma::DEBUG("HISTOGRAMMERFLATNTUPLE : Fill kinematics");
    fill("met_met_"+name, event.met("met"), event_weight);
    fill("met_phi_"+name, event.met("phi"), event_weight);
    fill("ht_"+name,      event.HT(),       event_weight);

    // HME && DNN && AMWT
    cma::DEBUG("HISTOGRAMMERFLATNTUPLE : Fill AMWT");
    //fill("dnn_"+name, event.dnn(), event_weight); // N/A

    cma::DEBUG("HISTOGRAMMERFLATNTUPLE : Fill VLQ/Wprime");
/*    fill("top_pt_"+name,  top.p4.Pt(),  event_weight);
    fill("top_eta_"+name, top.p4.Eta(), event_weight);
    fill("top_phi_"+name, top.p4.Phi(), event_weight);
    fill("top_m_"+name,   top.p4.M(),   event_weight);
    fill("antitop_pt_"+name,  antitop.p4.Pt(),  event_weight); 
    fill("antitop_eta_"+name, antitop.p4.Eta(), event_weight);
    fill("antitop_phi_"+name, antitop.p4.Phi(), event_weight);
    fill("antitop_m_"+name,   antitop.p4.M(),   event_weight);

    fill("mttbar_"+name,  (top.p4+antitop.p4).M(), event_weight);
    fill("pTttbar_"+name, (top.p4+antitop.p4).Pt(), event_weight);
*/
    cma::DEBUG("HISTOGRAMMERFLATNTUPLE : End histograms");

    return;
}





/**** OVER/UNDERFLOW ****/

void histogrammerFlatNtuple::overUnderFlow(){
    /* Call overflow and underflow functions at once */
    overFlow();
    underFlow();
    return;
}


void histogrammerFlatNtuple::overFlow() {
    /* Add overflow to last bin */
    if (!m_putOverflowInLastBin){
        cma::INFO("HISTOGRAMMERFLATNTUPLE : Not putting overflow in last bin(s)");
        return;
    }
    else{
        // loop over 1D histograms
        for (const auto& hist : m_map_histograms1D){
            unsigned int numBins = hist.second->GetNbinsX();
            double overflowContent = hist.second->GetBinContent(numBins + 1);

            hist.second->AddBinContent(numBins,overflowContent); // add overflow to last bin
            hist.second->SetBinContent(numBins+1, 0);            // set overflow to 0
        }
        // loop over 2D histograms
        for (const auto& hist : m_map_histograms2D){
            unsigned int numBinsX = hist.second->GetXaxis()->GetNbins();
            unsigned int numBinsY = hist.second->GetYaxis()->GetNbins();

            // x-axis :: overflow in y
            for (unsigned int xx=1;xx<numBinsX+1;++xx){
                double overflowContent = hist.second->GetBinContent(xx,numBinsY+1);

                int lastBin     = hist.second->GetBin(xx,numBinsY);
                int overflowBin = hist.second->GetBin(xx,numBinsY+1);
                hist.second->AddBinContent(lastBin,overflowContent); // add overflow to last bin
                hist.second->SetBinContent(overflowBin,0);           // set overflow to 0
            }
            // y-axis :: overflow in x
            for (unsigned int yy=1;yy<numBinsY;++yy){
                double overflowContent = hist.second->GetBinContent(numBinsX+1,yy);

                int lastBin     = hist.second->GetBin(numBinsX,yy);
                int overflowBin = hist.second->GetBin(numBinsX+1,yy);
                hist.second->AddBinContent(lastBin,overflowContent); // add overflow to last bin
                hist.second->SetBinContent(overflowBin,0);           // set overflow to 0
            }
        } // end 2D histogram overflow
    } // end else put overflow in first bin

    return;
}

void histogrammerFlatNtuple::underFlow() {
    /* Add underflow to first bin */
    if (!m_putUnderflowInFirstBin){
        cma::INFO("HISTOGRAMMERFLATNTUPLE : Not putting underflow in first bin(s)");
        return;
    }
    else{
        // loop over 1D histograms
        for (const auto& hist : m_map_histograms1D){
            double underflowContent = hist.second->GetBinContent(0);

            hist.second->AddBinContent(1, underflowContent);  // add underflow to first bin
            hist.second->SetBinContent(0, 0);                 // set underflow to 0
        }
        // loop over 2D histograms
        for (const auto& hist : m_map_histograms2D){
            unsigned int numBinsX = hist.second->GetXaxis()->GetNbins();
            unsigned int numBinsY = hist.second->GetYaxis()->GetNbins();

            // x-axis :: underflow in y
            for (unsigned int xx=1;xx<numBinsX+1;++xx){
                double underflowContent = hist.second->GetBinContent(xx,numBinsY+1);

                int firstBin     = hist.second->GetBin(xx,1);
                int underflowBin = hist.second->GetBin(xx,0);
                hist.second->AddBinContent(firstBin,underflowContent); // add overflow to last bin
                hist.second->SetBinContent(underflowBin,0);            // set overflow to 0
            }
            // y-axis :: underflow in x
            for (unsigned int yy=1;yy<numBinsY;++yy){
                double underflowContent = hist.second->GetBinContent(0,yy);

                int firstBin     = hist.second->GetBin(1,yy);
                int underflowBin = hist.second->GetBin(0,yy);
                hist.second->AddBinContent(firstBin,underflowContent); // add overflow to last bin
                hist.second->SetBinContent(underflowBin,0);           // set overflow to 0
            }
        } // end 2D histogram underflow
    } // end else put underflow in first bin
}

// THE END
