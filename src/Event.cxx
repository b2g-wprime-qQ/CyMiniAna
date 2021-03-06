/*
Created:        --
Last Updated:   13 December 2017

Dan Marley
daniel.edison.marley@cernSPAMNOT.ch
University of Michigan, Ann Arbor, MI 48109

-----

Event class
Contains all the objects (& structs) with event information
Assumes a flat ntuple data structure

*/
#include "Analysis/CyMiniAna/interface/Event.h"

// constructor
Event::Event( TTreeReader &myReader, configuration &cmaConfig ) :
  m_config(&cmaConfig),
  m_ttree(myReader),
  m_treeName("SetMe"),
  m_fileName("SetMe"),
  m_DNN(0.0){
    m_treeName = m_ttree.GetTree()->GetName();      // for systematics
    m_fileName = m_config->filename();              // for accessing file metadata

    // Set options for the class
    m_isMC         = m_config->isMC();              // simulated or real sample
    m_useTruth     = m_config->useTruth();          // access and use truth information
    m_grid         = m_config->isGridFile();        // file directly from original analysis team
    m_useJets      = m_config->useJets();           // use AK4 jets in analysis
    m_useLargeRJets= m_config->useLargeRJets();     // use AK8 jets in analysis
    m_useLeptons   = m_config->useLeptons();        // use leptons in analysis
    m_useNeutrinos = m_config->useNeutrinos();      // use neutrinos in analysis
    m_useWprime    = m_config->useWprime();         // use reconstructed Wprime in analysis

    m_neutrinoReco  = m_config->neutrinoReco();      // reconstruct neutrino
    m_wprimeReco    = m_config->wprimeReco();        // reconstruct Wprime
    m_DNNinference  = m_config->DNNinference();      // use DNN to predict values
    m_DNNtraining   = m_config->DNNtraining();       // load DNN features (save/use later)
    m_getDNN = (m_DNNinference || m_DNNtraining);
    m_useDNN = m_config->useDNN();                   // use DNN in analysis

    // b-tagging working points
    m_CSVv2L  = m_config->CSVv2L();
    m_CSVv2M  = m_config->CSVv2M();
    m_CSVv2T  = m_config->CSVv2T();


    //** Access branches from Tree **//
    m_eventNumber  = new TTreeReaderValue<unsigned long long>(m_ttree,"eventNumber");
    m_runNumber    = new TTreeReaderValue<unsigned int>(m_ttree,"runNumber");
    m_lumiblock    = new TTreeReaderValue<unsigned int>(m_ttree,"lumiblock");

    m_npv = new TTreeReaderValue<unsigned int>(m_ttree,"npv");
    m_rho = new TTreeReaderValue<float>(m_ttree,"rho");
    m_true_pileup = new TTreeReaderValue<unsigned int>(m_ttree,"true_pileup");

    /** Triggers **/
    m_HLT_Ele45_CaloIdVT_GsfTrkIdT_PFJet200_PFJet50 = new TTreeReaderValue<unsigned int>(m_ttree,"HLT_Ele45_CaloIdVT_GsfTrkIdT_PFJet200_PFJet50");
    m_HLT_Ele50_CaloIdVT_GsfTrkIdT_PFJet165 = new TTreeReaderValue<unsigned int>(m_ttree,"HLT_Ele50_CaloIdVT_GsfTrkIdT_PFJet165");
    m_HLT_Ele115_CaloIdVT_GsfTrkIdT    = new TTreeReaderValue<unsigned int>(m_ttree,"HLT_Ele115_CaloIdVT_GsfTrkIdT");
    m_HLT_Mu40_Eta2P1_PFJet200_PFJet50 = new TTreeReaderValue<unsigned int>(m_ttree,"HLT_Mu40_Eta2P1_PFJet200_PFJet50");
    m_HLT_Mu50    = new TTreeReaderValue<unsigned int>(m_ttree,"HLT_Mu50");
    m_HLT_TkMu50  = new TTreeReaderValue<unsigned int>(m_ttree,"HLT_TkMu50");
    m_HLT_PFHT800 = new TTreeReaderValue<unsigned int>(m_ttree,"HLT_PFHT800");
    m_HLT_PFHT900 = new TTreeReaderValue<unsigned int>(m_ttree,"HLT_PFHT900");
    m_HLT_AK8PFJet450 = new TTreeReaderValue<unsigned int>(m_ttree,"HLT_AK8PFJet450");
    m_HLT_PFHT700TrimMass50  = new TTreeReaderValue<unsigned int>(m_ttree,"HLT_PFHT700TrimMass50");
    m_HLT_PFJet360TrimMass30 = new TTreeReaderValue<unsigned int>(m_ttree,"HLT_PFJet360TrimMass30");
    //m_HLT_Ele45_WPLoose_Gsf = new TTreeReaderValue<int>(m_ttree,"HLT_Ele45_WPLoose_Gsf");
    //m_HLT_TkMu50 = new TTreeReaderValue<int>(m_ttree,"HLT_TkMu50");

    /** Filters **/
    m_Flag_goodVertices  = new TTreeReaderValue<unsigned int>(m_ttree,"Flag_goodVertices");
    m_Flag_eeBadScFilter = new TTreeReaderValue<unsigned int>(m_ttree,"Flag_eeBadScFilter");
    m_Flag_HBHENoiseFilter    = new TTreeReaderValue<unsigned int>(m_ttree,"Flag_HBHENoiseFilter");
    m_Flag_HBHENoiseIsoFilter = new TTreeReaderValue<unsigned int>(m_ttree,"Flag_HBHENoiseIsoFilter");
    m_Flag_globalTightHalo2016Filter = new TTreeReaderValue<unsigned int>(m_ttree,"Flag_globalTightHalo2016Filter");
    m_Flag_EcalDeadCellTriggerPrimitiveFilter = new TTreeReaderValue<unsigned int>(m_ttree,"Flag_EcalDeadCellTriggerPrimitiveFilter");

    /** JETS **/
    if (m_useJets){
      // small-R jet information
      m_jet_pt  = new TTreeReaderValue<std::vector<float>>(m_ttree,"AK4pt");
      m_jet_eta = new TTreeReaderValue<std::vector<float>>(m_ttree,"AK4eta");
      m_jet_phi = new TTreeReaderValue<std::vector<float>>(m_ttree,"AK4phi");
      m_jet_m   = new TTreeReaderValue<std::vector<float>>(m_ttree,"AK4mass");
      m_jet_bdisc    = new TTreeReaderValue<std::vector<float>>(m_ttree,"AK4bDisc");
      m_jet_deepCSV  = new TTreeReaderValue<std::vector<float>>(m_ttree,"AK4deepCSV");
      m_jet_area     = new TTreeReaderValue<std::vector<float>>(m_ttree,"AK4area");
      m_jet_uncorrPt = new TTreeReaderValue<std::vector<float>>(m_ttree,"AK4uncorrPt");
      m_jet_uncorrE  = new TTreeReaderValue<std::vector<float>>(m_ttree,"AK4uncorrE");
    }

    if (m_useLargeRJets){
      // large-R Jet information
      m_ljet_pt     = new TTreeReaderValue<std::vector<float>>(m_ttree,"AK8pt");
      m_ljet_eta    = new TTreeReaderValue<std::vector<float>>(m_ttree,"AK8eta");
      m_ljet_phi    = new TTreeReaderValue<std::vector<float>>(m_ttree,"AK8phi");
      m_ljet_m      = new TTreeReaderValue<std::vector<float>>(m_ttree,"AK8mass");
      m_ljet_SDmass = new TTreeReaderValue<std::vector<float>>(m_ttree,"AK8SDmass");
      m_ljet_tau1   = new TTreeReaderValue<std::vector<float>>(m_ttree,"AK8tau1");
      m_ljet_tau2   = new TTreeReaderValue<std::vector<float>>(m_ttree,"AK8tau2");
      m_ljet_tau3   = new TTreeReaderValue<std::vector<float>>(m_ttree,"AK8tau3");
      m_ljet_area   = new TTreeReaderValue<std::vector<float>>(m_ttree,"AK8area");
      m_ljet_charge = new TTreeReaderValue<std::vector<float>>(m_ttree,"AK8charge");
      m_ljet_subjet0_charge = new TTreeReaderValue<std::vector<float>>(m_ttree,"AK8subjet0charge");
      m_ljet_subjet0_bdisc  = new TTreeReaderValue<std::vector<float>>(m_ttree,"AK8subjet0bDisc");
      m_ljet_subjet0_deepCSV= new TTreeReaderValue<std::vector<float>>(m_ttree,"AK8subjet0deepCSV");
      m_ljet_subjet0_pt     = new TTreeReaderValue<std::vector<float>>(m_ttree,"AK8subjet0pt");
      m_ljet_subjet0_mass   = new TTreeReaderValue<std::vector<float>>(m_ttree,"AK8subjet0mass");

      if (m_config->isGridFile()){
          m_ljet_subjet0_tau1   = new TTreeReaderValue<std::vector<float>>(m_ttree,"AK8subjet0tau1");
          m_ljet_subjet0_tau2   = new TTreeReaderValue<std::vector<float>>(m_ttree,"AK8subjet0tau2");
          m_ljet_subjet0_tau3   = new TTreeReaderValue<std::vector<float>>(m_ttree,"AK8subjet0tau3");
          m_ljet_subjet1_tau1   = new TTreeReaderValue<std::vector<float>>(m_ttree,"AK8subjet1tau1");
          m_ljet_subjet1_tau2   = new TTreeReaderValue<std::vector<float>>(m_ttree,"AK8subjet1tau2");
          m_ljet_subjet1_tau3   = new TTreeReaderValue<std::vector<float>>(m_ttree,"AK8subjet1tau3");
      }

      m_ljet_subjet1_charge = new TTreeReaderValue<std::vector<float>>(m_ttree,"AK8subjet1charge");
      m_ljet_subjet1_bdisc  = new TTreeReaderValue<std::vector<float>>(m_ttree,"AK8subjet1bDisc");
      m_ljet_subjet1_deepCSV= new TTreeReaderValue<std::vector<float>>(m_ttree,"AK8subjet1deepCSV");
      m_ljet_subjet1_pt     = new TTreeReaderValue<std::vector<float>>(m_ttree,"AK8subjet1pt");
      m_ljet_subjet1_mass   = new TTreeReaderValue<std::vector<float>>(m_ttree,"AK8subjet1mass");

      m_ljet_BEST_class = new TTreeReaderValue<std::vector<int>>(m_ttree,"AK8BEST_class");
      m_ljet_BEST_t = new TTreeReaderValue<std::vector<float>>(m_ttree,"AK8BEST_t");
      m_ljet_BEST_w = new TTreeReaderValue<std::vector<float>>(m_ttree,"AK8BEST_w");
      m_ljet_BEST_z = new TTreeReaderValue<std::vector<float>>(m_ttree,"AK8BEST_z");
      m_ljet_BEST_h = new TTreeReaderValue<std::vector<float>>(m_ttree,"AK8BEST_h");
      m_ljet_BEST_j = new TTreeReaderValue<std::vector<float>>(m_ttree,"AK8BEST_j");

      m_ljet_uncorrPt = new TTreeReaderValue<std::vector<float>>(m_ttree,"AK8uncorrPt");
      m_ljet_uncorrE  = new TTreeReaderValue<std::vector<float>>(m_ttree,"AK8uncorrE");
    }


    /** LEPTONS **/
    if (m_useLeptons){
      m_el_pt  = new TTreeReaderValue<std::vector<float>>(m_ttree,"ELpt");
      m_el_eta = new TTreeReaderValue<std::vector<float>>(m_ttree,"ELeta");
      m_el_phi = new TTreeReaderValue<std::vector<float>>(m_ttree,"ELphi");
      m_el_e   = new TTreeReaderValue<std::vector<float>>(m_ttree,"ELenergy");
      m_el_charge = new TTreeReaderValue<std::vector<float>>(m_ttree,"ELcharge");
      //m_el_iso = new TTreeReaderValue<std::vector<float>>(m_ttree,"ELiso");
      m_el_id_loose  = new TTreeReaderValue<std::vector<unsigned int>>(m_ttree,"ELlooseID");
      m_el_id_medium = new TTreeReaderValue<std::vector<unsigned int>>(m_ttree,"ELmediumID");
      m_el_id_tight  = new TTreeReaderValue<std::vector<unsigned int>>(m_ttree,"ELtightID");
      m_el_id_loose_noIso  = new TTreeReaderValue<std::vector<unsigned int>>(m_ttree,"ELlooseIDnoIso");
      m_el_id_medium_noIso = new TTreeReaderValue<std::vector<unsigned int>>(m_ttree,"ELmediumIDnoIso");
      m_el_id_tight_noIso  = new TTreeReaderValue<std::vector<unsigned int>>(m_ttree,"ELtightIDnoIso");

      m_mu_pt  = new TTreeReaderValue<std::vector<float>>(m_ttree,"MUpt");
      m_mu_eta = new TTreeReaderValue<std::vector<float>>(m_ttree,"MUeta");
      m_mu_phi = new TTreeReaderValue<std::vector<float>>(m_ttree,"MUphi");
      m_mu_e   = new TTreeReaderValue<std::vector<float>>(m_ttree,"MUenergy");
      m_mu_charge = new TTreeReaderValue<std::vector<float>>(m_ttree,"MUcharge");
      m_mu_iso = new TTreeReaderValue<std::vector<float>>(m_ttree,"MUcorrIso");
      m_mu_id_loose  = new TTreeReaderValue<std::vector<unsigned int>>(m_ttree,"MUlooseID");
      m_mu_id_medium = new TTreeReaderValue<std::vector<unsigned int>>(m_ttree,"MUmediumID");
      m_mu_id_tight  = new TTreeReaderValue<std::vector<unsigned int>>(m_ttree,"MUtightID");
    }

    if (!m_neutrinoReco && m_useNeutrinos){
        // Neutrinos aren't stored in the baseline ntuples, requires 'kinematicReco' to create
        m_nu_pt  = new TTreeReaderValue<std::vector<float>>(m_ttree, "nu_pt");
        m_nu_eta = new TTreeReaderValue<std::vector<float>>(m_ttree, "nu_eta");
        m_nu_phi = new TTreeReaderValue<std::vector<float>>(m_ttree, "nu_phi");
    }

    m_met_met  = new TTreeReaderValue<float>(m_ttree,"METpt");
    m_met_phi  = new TTreeReaderValue<float>(m_ttree,"METphi");

    m_HTAK8    = new TTreeReaderValue<float>(m_ttree,"HTak8");
    m_HTAK4    = new TTreeReaderValue<float>(m_ttree,"HTak4");

    // set some event weights and access necessary branches
    m_xsection       = 1.0;
    m_kfactor        = 1.0;
    m_sumOfWeights   = 1.0;
    m_LUMI           = m_config->LUMI();

    Sample ss = m_config->sample();

    // MC information
    if (m_isMC){
      //m_weight_mc    = 1;//new TTreeReaderValue<float>(m_ttree,"evt_Gen_Weight");
      m_xsection     = ss.XSection;
      m_kfactor      = ss.KFactor;        // most likely =1
      m_sumOfWeights = ss.sumOfWeights;

      m_mc_pt  = new TTreeReaderValue<std::vector<float>>(m_ttree,"GENpt");
      m_mc_eta = new TTreeReaderValue<std::vector<float>>(m_ttree,"GENeta");
      m_mc_phi = new TTreeReaderValue<std::vector<float>>(m_ttree,"GENphi");
      m_mc_e   = new TTreeReaderValue<std::vector<float>>(m_ttree,"GENenergy");
      m_mc_pdgId  = new TTreeReaderValue<std::vector<int>>(m_ttree,"GENid");
      m_mc_status = new TTreeReaderValue<std::vector<int>>(m_ttree,"GENstatus");
      m_mc_parent_idx = new TTreeReaderValue<std::vector<int>>(m_ttree,"GENparent_idx");
      m_mc_child0_idx = new TTreeReaderValue<std::vector<int>>(m_ttree,"GENchild0_idx");
      m_mc_child1_idx = new TTreeReaderValue<std::vector<int>>(m_ttree,"GENchild1_idx");
      m_mc_isHadTop = new TTreeReaderValue<std::vector<int>>(m_ttree,"GENisHadTop");
/*
      m_mc_ht = new TTreeReaderValue<float>(m_ttree,"evt_Gen_Ht");
      m_truth_jet_pt  = new TTreeReaderValue<float>(m_ttree,"jetAK4CHS_GenJetPt");
      m_truth_jet_eta = new TTreeReaderValue<std::vector<float>>(m_ttree,"jetAK4CHS_GenJetEta");
      m_truth_jet_phi = new TTreeReaderValue<std::vector<float>>(m_ttree,"jetAK4CHS_GenJetPhi");
      m_truth_jet_e   = new TTreeReaderValue<std::vector<float>>(m_ttree,"jetAK4CHS_GenJetCharge");
      m_truth_ljet_pt  = new TTreeReaderValue<std::vector<float>>(m_ttree,"jetAK8CHS_GenJetPt");
      m_truth_ljet_eta = new TTreeReaderValue<std::vector<float>>(m_ttree,"jetAK8CHS_GenJetEta");
      m_truth_ljet_phi = new TTreeReaderValue<std::vector<float>>(m_ttree,"jetAK8CHS_GenJetPhi");
      m_truth_ljet_e   = new TTreeReaderValue<std::vector<float>>(m_ttree,"jetAK8CHS_GenJetE");
      m_truth_ljet_charge     = new TTreeReaderValue<std::vector<float>>(m_ttree,"jetAK8CHS_GenJetCharge");
      m_truth_ljet_subjet_charge = new TTreeReaderValue<std::vector<float>>(m_ttree,"subjetAK8CHS_GenJetCharge");
*/
    } // end isMC


    // Truth matching tool
    m_truthMatchingTool = new truthMatching(cmaConfig);
    m_truthMatchingTool->initialize();

    // DNN material
    m_deepLearningTool = new DeepLearning(cmaConfig);
    if (!m_getDNN && m_useDNN)
        m_dnn_score = new TTreeReaderValue<float>(m_ttree,"ljet_CWoLa");

    // Kinematic reconstruction algorithms
    m_neutrinoRecoTool = new NeutrinoReco(cmaConfig);
    m_wprimeTool = new WprimeReco(cmaConfig);
} // end constructor

Event::~Event() {}



void Event::initialize_eventWeights(){
    /* Create vectors of the systematics that are weights for the nominal events
       Must be called from the constructor for the access to TTreeReaderValues to work!
    */
    std::map<std::string,unsigned int> mapWeightSystematics = m_config->mapOfWeightVectorSystematics();

    m_listOfWeightSystematics = m_config->listOfWeightSystematics();

    m_weightSystematicsFloats.clear();
    m_weightSystematicsVectorFloats.clear();

    // systematics from the nominal tree that are floats
    for (const auto& nom_syst : m_listOfWeightSystematics){
        if (!m_config->useLeptons() && nom_syst.find("leptonSF")!=std::string::npos)
            continue;
        m_weightSystematicsFloats[nom_syst] = new TTreeReaderValue<float>(m_ttree,nom_syst.c_str());
    }

    // systematics from the nominal tree that are vectors
    for (const auto& syst : mapWeightSystematics)
        m_weightSystematicsVectorFloats[syst.first] = new TTreeReaderValue<std::vector<float>>(m_ttree,syst.first.c_str());

    return;
}



void Event::updateEntry(Long64_t entry){
    /* Update the entry -> update all TTree variables */
    cma::DEBUG("EVENT : Update Entry "+std::to_string(entry) );
    m_entry = entry;

    // make sure the entry exists
    if(isValidRecoEntry())
        m_ttree.SetEntry(m_entry);
    else
        cma::ERROR("EVENT : Invalid Reco entry "+std::to_string(m_entry)+"!");

    return;
}


void Event::clear(){
    /* Clear many of the vectors/maps for each event -- SAFETY PRECAUTION */
    m_truth_ljets.clear();
    m_truth_jets.clear();
    m_truth_leptons.clear();
    m_truth_neutrinos.clear();

    m_jets.clear();
    m_ljets.clear();
    m_leptons.clear();
    m_neutrinos.clear();

    m_btag_jets.clear();
    m_btag_jets_default.clear();
    m_weight_btag_default = 1.0;
    m_nominal_weight = 1.0;

    m_HT = 0;
    m_ST = 0;

    return;
}


void Event::execute(Long64_t entry){
    /* Get the values from the event */
    cma::DEBUG("EVENT : Execute event " );

    // Load data from root tree for this event
    updateEntry(entry);

    // Reset many event-level values
    clear();

    // Get the event weights (for cutflow & histograms)
    initialize_weights();
    cma::DEBUG("EVENT : Setup weights ");

    // Filters
    initialize_filters();

    // Triggers
    initialize_triggers();

    // Truth Information
    if (m_useTruth){
        initialize_truth();
        cma::DEBUG("EVENT : Setup truth information ");
    }

    // Jets
    if (m_useJets){
        initialize_jets();
        cma::DEBUG("EVENT : Setup small-R jets ");
    }

    // Large-R Jets
    if (m_useLargeRJets){
        initialize_ljets();
        cma::DEBUG("EVENT : Setup large-R jets ");
    }

    // Leptons
    if (m_useLeptons){
        initialize_leptons();
        cma::DEBUG("EVENT : Setup leptons ");
    }

    // Get some kinematic variables (MET, HT, ST)
    initialize_kinematics();
    cma::DEBUG("EVENT : Setup kinematic variables ");

    // Neutrinos
    if (m_useNeutrinos){
        // relies on kinematic reconstruction, unless the information is saved in root file
        initialize_neutrinos();
        cma::DEBUG("EVENT : Setup neutrinos ");
    }

    // Kinematic reconstruction (if they values aren't in the root file)
    if (m_useWprime){
        wprimeReconstruction();
    }

   if (m_useNeutrinos){
       deepLearningPrediction();   // store features in map (easily access later)
       cma::DEBUG("EVENT : Deep learning ");
    }

    cma::DEBUG("EVENT : Setup Event ");

    return;
}


void Event::initialize_filters(){
    /* Setup the filters */
    m_filters.clear();

    m_filters["goodVertices"] = **m_Flag_goodVertices;
    m_filters["eeBadScFilter"] = **m_Flag_eeBadScFilter;
    m_filters["HBHENoiseFilter"] = **m_Flag_HBHENoiseFilter;
    m_filters["HBHENoiseIsoFilter"] = **m_Flag_HBHENoiseIsoFilter;
    m_filters["globalTightHalo2016Filter"] = **m_Flag_globalTightHalo2016Filter;
    m_filters["EcalDeadCellTriggerPrimitiveFilter"] = **m_Flag_EcalDeadCellTriggerPrimitiveFilter;

    return;
}


void Event::initialize_triggers(){
    /* Setup triggers */
    m_triggers.clear();

    m_triggers["HLT_Ele45_CaloIdVT_GsfTrkIdT_PFJet200_PFJet50"] = **m_HLT_Ele45_CaloIdVT_GsfTrkIdT_PFJet200_PFJet50;
    m_triggers["HLT_Ele50_CaloIdVT_GsfTrkIdT_PFJet165"] = **m_HLT_Ele50_CaloIdVT_GsfTrkIdT_PFJet165;
    m_triggers["HLT_Ele115_CaloIdVT_GsfTrkIdT"]    = **m_HLT_Ele115_CaloIdVT_GsfTrkIdT;
    m_triggers["HLT_Mu40_Eta2P1_PFJet200_PFJet50"] = **m_HLT_Mu40_Eta2P1_PFJet200_PFJet50;
    m_triggers["HLT_Mu50"]    = **m_HLT_Mu50;
    m_triggers["HLT_TkMu50"]  = **m_HLT_TkMu50;
    m_triggers["HLT_PFHT800"] = **m_HLT_PFHT800;
    m_triggers["HLT_PFHT900"] = **m_HLT_PFHT900;
    m_triggers["HLT_AK8PFJet450"] = **m_HLT_AK8PFJet450;
    m_triggers["HLT_PFHT700TrimMass50"]  = **m_HLT_PFHT700TrimMass50;
    m_triggers["HLT_PFJet360TrimMass30"] = **m_HLT_PFJet360TrimMass30;

    return;
}


void Event::initialize_truth(){
    /* Setup truth information (MC and physics objects) */
    m_truth_partons.clear();

    unsigned int nPartons( (*m_mc_pt)->size() );
    cma::DEBUG("EVENT : N Partons = "+std::to_string(nPartons));

    // loop over truth partons
    unsigned int p_idx(0);
    for (unsigned int i=0; i<nPartons; i++){

        Parton parton;
        parton.p4.SetPtEtaPhiE((*m_mc_pt)->at(i),(*m_mc_eta)->at(i),(*m_mc_phi)->at(i),(*m_mc_e)->at(i));

        int status = (*m_mc_status)->at(i);
        int pdgId  = (*m_mc_pdgId)->at(i);
        unsigned int abs_pdgId = std::abs(pdgId);

        parton.pdgId  = pdgId;
        parton.status = status;

        // simple booleans for type
        parton.isWprime = ( abs_pdgId==9900213 );
        parton.isVLQ    = ( abs_pdgId==8000001 || abs_pdgId==7000001 );
        parton.isTop = ( abs_pdgId==6 );
        parton.isW   = ( abs_pdgId==24 );
        parton.isZ   = ( abs_pdgId==23 );
        parton.isHiggs  = ( abs_pdgId==25 );

        parton.isLepton = ( abs_pdgId>=11 && abs_pdgId<=16 );
        parton.isQuark  = ( abs_pdgId<7 );

        if (parton.isLepton){
            parton.isTau  = ( abs_pdgId==15 );
            parton.isMuon = ( abs_pdgId==13 );
            parton.isElectron = ( abs_pdgId==11 );
            parton.isNeutrino = ( abs_pdgId==12 || abs_pdgId==14 || abs_pdgId==16 );
        }
        else if (parton.isQuark){
            parton.isLight  = ( abs_pdgId<5 );
            parton.isBottom = ( abs_pdgId==5 );
        }

        parton.index      = p_idx;                    // index in vector of truth_partons
        parton.parent_idx = (*m_mc_parent_idx)->at(i);
        parton.child0_idx = (*m_mc_child0_idx)->at(i);
        parton.child1_idx = (*m_mc_child1_idx)->at(i);

        m_truth_partons.push_back( parton );
        p_idx++;
    }

    m_truthMatchingTool->setTruthPartons(m_truth_partons);
    m_truthMatchingTool->buildWprimeSystem();
    m_truth_wprime = m_truthMatchingTool->wprime();         // build the truth wprime decay chain

    return;
}


void Event::initialize_jets(){
    /* Setup struct of jets (small-r) and relevant information 
     * b-tagging: https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation80XReReco
        CSVv2L -0.5884
        CSVv2M 0.4432
        CSVv2T 0.9432
     */
    unsigned int nJets = (*m_jet_pt)->size();
    m_jets.clear();
    m_jets_iso.clear();    // jet collection for lepton 2D isolation

    for (const auto& btagWP : m_config->btagWkpts() ){
        m_btag_jets[btagWP].clear();
    }

    unsigned int idx(0);
    unsigned int idx_iso(0);
    for (unsigned int i=0; i<nJets; i++){
        Jet jet;
        jet.p4.SetPtEtaPhiM( (*m_jet_pt)->at(i),(*m_jet_eta)->at(i),(*m_jet_phi)->at(i),(*m_jet_m)->at(i));

        bool isGood(jet.p4.Pt()>50 && std::abs(jet.p4.Eta())<2.4);
        bool isGoodIso( jet.p4.Pt()>15 && std::abs(jet.p4.Eta())<2.4);

        if (!isGood && !isGoodIso) continue;

        jet.bdisc    = (*m_jet_bdisc)->at(i);
        jet.deepCSV  = (*m_jet_deepCSV)->at(i);
        jet.area     = (*m_jet_area)->at(i);
        jet.uncorrE  = (*m_jet_uncorrE)->at(i);
        jet.uncorrPt = (*m_jet_uncorrPt)->at(i);

        jet.index  = idx;
        jet.isGood = isGood;

        if (isGood){
            m_jets.push_back(jet);
            getBtaggedJets(jet);          // only care about b-tagging for 'real' AK4
            idx++;
        }
        if (isGoodIso){
            m_jets_iso.push_back(jet);    // used for 2D isolation
            idx_iso++;
        }
    }

    m_btag_jets_default = m_btag_jets.at(m_config->jet_btagWkpt());

    return;
}


void Event::initialize_ljets(){
    /* Setup struct of large-R jets and relevant information 
      0 :: Top      (lepton Q < 0)
      1 :: Anti-top (lepton Q > 0)
    */
    unsigned int nLjets = (*m_ljet_pt)->size();
    m_ljets.clear();

    unsigned int idx(0);
    for (unsigned int i=0; i<nLjets; i++){
        Ljet ljet;
        ljet.p4.SetPtEtaPhiM( (*m_ljet_pt)->at(i),(*m_ljet_eta)->at(i),(*m_ljet_phi)->at(i),(*m_ljet_m)->at(i));
        ljet.softDropMass = (*m_ljet_SDmass)->at(i);

        ljet.tau1   = (*m_ljet_tau1)->at(i);
        ljet.tau2   = (*m_ljet_tau2)->at(i);
        ljet.tau3   = (*m_ljet_tau3)->at(i);
        ljet.tau21  = ljet.tau2 / ljet.tau1;
        ljet.tau32  = ljet.tau3 / ljet.tau2;
        //bool toptag = (ljet.softDropMass>105. && ljet.softDropMass<210 && ljet.tau32<0.65);

        // check if the AK8 is 'good'
        bool isGood(ljet.p4.Pt()>400. && fabs(ljet.p4.Eta())<2.4); // && toptag);
        if (!isGood) continue;

        ljet.charge = (*m_ljet_charge)->at(i);

        ljet.BEST_t = (*m_ljet_BEST_t)->at(i);
        ljet.BEST_w = (*m_ljet_BEST_w)->at(i);
        ljet.BEST_z = (*m_ljet_BEST_z)->at(i);
        ljet.BEST_h = (*m_ljet_BEST_h)->at(i);
        ljet.BEST_j = (*m_ljet_BEST_j)->at(i);
        ljet.BEST_class = (*m_ljet_BEST_class)->at(i);

        ljet.subjet0_bdisc  = (*m_ljet_subjet0_bdisc)->at(i);
        ljet.subjet0_charge = (*m_ljet_subjet0_charge)->at(i);
        ljet.subjet0_mass   = (*m_ljet_subjet0_mass)->at(i);
        ljet.subjet0_pt     = (*m_ljet_subjet0_pt)->at(i);
        ljet.subjet1_bdisc  = (*m_ljet_subjet1_bdisc)->at(i);
        ljet.subjet1_charge = (*m_ljet_subjet1_charge)->at(i);
        ljet.subjet1_mass   = (*m_ljet_subjet1_mass)->at(i);
        ljet.subjet1_pt     = (*m_ljet_subjet1_pt)->at(i);

        float subjet0_tau1(-999.);
        float subjet0_tau2(-999.);
        float subjet0_tau3(-999.);
        float subjet1_tau1(-999.);
        float subjet1_tau2(-999.);
        float subjet1_tau3(-999.);

        if (m_config->isGridFile()){
            // older files will not have this option
            subjet0_tau1 = (*m_ljet_subjet0_tau1)->at(i);
            subjet0_tau2 = (*m_ljet_subjet0_tau2)->at(i);
            subjet0_tau3 = (*m_ljet_subjet0_tau3)->at(i);
            subjet1_tau1 = (*m_ljet_subjet1_tau1)->at(i);
            subjet1_tau2 = (*m_ljet_subjet1_tau2)->at(i);
            subjet1_tau3 = (*m_ljet_subjet1_tau3)->at(i);
        }

        ljet.subjet0_tau1 = subjet0_tau1; //(*m_ljet_subjet0_tau1)->at(i);
        ljet.subjet0_tau2 = subjet0_tau2; //(*m_ljet_subjet0_tau2)->at(i);
        ljet.subjet0_tau3 = subjet0_tau3; //(*m_ljet_subjet0_tau3)->at(i);
        ljet.subjet1_tau1 = subjet1_tau1; //(*m_ljet_subjet1_tau1)->at(i);
        ljet.subjet1_tau2 = subjet1_tau2; //(*m_ljet_subjet1_tau2)->at(i);
        ljet.subjet1_tau3 = subjet1_tau3; //(*m_ljet_subjet1_tau3)->at(i);

        ljet.target = -1;      // not used in this analysis
        ljet.isGood = isGood;
        ljet.index  = idx;

        ljet.area     = (*m_ljet_area)->at(i);
        ljet.uncorrE  = (*m_ljet_uncorrE)->at(i);
        ljet.uncorrPt = (*m_ljet_uncorrPt)->at(i);

        // Truth-matching to jet
        ljet.truth_partons.clear();
        if (m_useTruth){ // && m_config->isTtbar()) {
            cma::DEBUG("EVENT : Truth match AK8");          // match subjets (and then the AK8 jet) to truth tops

            m_truthMatchingTool->matchJetToTruthTop(ljet);  // match to partons

            cma::DEBUG("EVENT : ++ Ljet had top = "+std::to_string(ljet.isHadTop)+" for truth top "+std::to_string(ljet.matchId));
        } // end truth matching ljet to partons

        m_ljets.push_back(ljet);
        idx++;
    }

    return;
}


void Event::initialize_leptons(){
    /* Setup struct of lepton and relevant information */
    m_leptons.clear();
    m_electrons.clear();
    m_muons.clear();

    // Muons
    unsigned int nMuons = (*m_mu_pt)->size();
    for (unsigned int i=0; i<nMuons; i++){
        Lepton mu;
        mu.p4.SetPtEtaPhiE( (*m_mu_pt)->at(i),(*m_mu_eta)->at(i),(*m_mu_phi)->at(i),(*m_mu_e)->at(i));
        bool isMedium   = (*m_mu_id_medium)->at(i);
        bool isTight    = (*m_mu_id_tight)->at(i);

        bool iso = customIsolation(mu);    // 2D isolation cut between leptons & AK4 (need AK4 initialized first!)

        bool isGood(mu.p4.Pt()>60 && std::abs(mu.p4.Eta())<2.4 && isMedium && iso);
        if (!isGood) continue;

        mu.charge = (*m_mu_charge)->at(i);
        mu.loose  = (*m_mu_id_loose)->at(i);
        mu.medium = isMedium; 
        mu.tight  = isTight; 
        mu.iso    = (*m_mu_iso)->at(i);
        mu.isGood = isGood;

        mu.isMuon = true;
        mu.isElectron = false;

        m_leptons.push_back(mu);
    }

    // Electrons
    unsigned int nElectrons = (*m_el_pt)->size();
    for (unsigned int i=0; i<nElectrons; i++){
        Lepton el;
        el.p4.SetPtEtaPhiE( (*m_el_pt)->at(i),(*m_el_eta)->at(i),(*m_el_phi)->at(i),(*m_el_e)->at(i));
        bool isTightNoIso = (*m_el_id_tight_noIso)->at(i);

        bool iso = customIsolation(el);    // 2D isolation cut between leptons & AK4 (need AK4 initialized first!)

        bool isGood(el.p4.Pt()>60 && std::abs(el.p4.Eta())<2.4 && isTightNoIso && iso);
        if (!isGood) continue;

        el.charge = (*m_el_charge)->at(i);
        el.loose  = (*m_el_id_loose)->at(i);
        el.medium = (*m_el_id_medium)->at(i);
        el.tight  = (*m_el_id_tight)->at(i);
        el.loose_noIso  = (*m_el_id_loose_noIso)->at(i);
        el.medium_noIso = (*m_el_id_medium_noIso)->at(i);
        el.tight_noIso  = isTightNoIso;
        el.isGood = isGood;

        el.isMuon = false;
        el.isElectron = true;

        m_leptons.push_back(el);
    }

    return;
}


void Event::initialize_neutrinos(){
    /* Build the neutrinos */
    m_neutrinos.clear();

    Neutrino nu1;
    nu1.p4.SetPtEtaPhiM( m_met.p4.Pt(), 0, m_met.p4.Phi(), 0);   // "dummy" value pz=0

    int nlep = m_leptons.size();
    if (nlep<1){
        // not enough leptons to do reconstruction, so just create dummy value
        m_neutrinos.push_back(nu1);
        return;
    }

    m_neutrinoRecoTool->setObjects(m_leptons.at(0),m_met);
    if (m_neutrinoReco){
        // reconstruct neutrinos!
        float pz  = m_neutrinoRecoTool->execute(true);        // standard reco; tool assumes 1-lepton final state
        float nuE = sqrt( pow(m_met.p4.Px(),2) + pow(m_met.p4.Py(),2) + pow(pz,2));
        nu1.p4.SetPxPyPzE( m_met.p4.Px(), m_met.p4.Py(), pz, nuE );
        nu1.isImaginary = m_neutrinoRecoTool->isImaginary();

        float pz_samp   = m_neutrinoRecoTool->execute(false);
        nu1.pz_sampling = pz_samp;
        nu1.pz_samplings = m_neutrinoRecoTool->pzSolutions();

        m_neutrinos.push_back(nu1);
    }
    else{
        // Assign neutrinos from root file
        m_neutrinos.push_back(nu1);                 // dummy value for now
    }

    return;
}



void Event::initialize_weights(){
    /* Event weights */
    m_nominal_weight = 1.0;

    m_weight_btag.clear();
    if (m_isMC){
        m_nominal_weight  = 1.0; //(**m_weight_pileup) * (**m_weight_mc);
        m_nominal_weight *= (m_xsection) * (m_kfactor) * m_LUMI / (m_sumOfWeights);
/*      // event weights
        m_weight_btag["70"] = (**m_weight_btag_70);
        m_weight_btag["77"] = (**m_weight_btag_77);
        m_weight_btag_default = m_weight_btag[m_config->jet_btagWkpt()];
        m_nominal_weight *= m_weight_btag_default;
*/
    }

    return;
}



void Event::initialize_kinematics(){
    /* Kinematic variables (HT, ST, MET) */
    m_HT_ak4 = **m_HTAK4;
    m_HT_ak8 = **m_HTAK8;

    m_HT = 0.0;
    m_ST = 0.0;

    // Get hadronic transverse energy
    if (m_useJets){
        // include small-R jet pT
        for (auto &small_jet : m_jets ){
            m_HT += small_jet.p4.Pt();
        }
    }
    else{
        // include large-R jet pT
        for (auto &large_jet : m_ljets){
            m_HT += large_jet.p4.Pt();
        }
    }

    // set MET
    m_met.p4.SetPtEtaPhiM(**m_met_met,0.,**m_met_phi,0.);

    // Get MET and lepton transverse energy
    m_ST += m_HT;
    m_ST += m_met.p4.Pt();

    if (m_useLeptons){
        for (const auto& lep : m_leptons)
            m_ST += lep.p4.Pt(); 
    }

    // transverse mass of the W (only relevant for 1-lepton)
    float mtw(0.0);

    if (m_leptons.size()>0){
        Lepton lep = m_leptons.at(0);
        float dphi = m_met.p4.Phi() - lep.p4.Phi();
        mtw = sqrt( 2 * lep.p4.Pt() * m_met.p4.Pt() * (1-cos(dphi)) );
    }
    m_met.mtw = mtw;

    return;
}


bool Event::customIsolation( Lepton& lep ){
    /* 2D isolation cut for leptons 
       - Check that the lepton and nearest AK4 jet satisfies
         DeltaR() < 0.4 || pTrel>30
    */
    bool pass(false);
    //int min_index(-1);                    // index of AK4 closest to lep
    float drmin(100.0);                   // min distance between lep and AK4s
    float ptrel(0.0);                     // pTrel between lepton and AK4s

    if (m_jets_iso.size()<1) return false;    // no AK4 -- event will fail anyway

    for (const auto& jet : m_jets_iso){
        float dr = lep.p4.DeltaR( jet.p4 );
        if (dr < drmin) {
            drmin = dr;
            ptrel = cma::ptrel( lep.p4,jet.p4 );
            //min_index = jet.index;
        }
    }

    lep.drmin = drmin;
    lep.ptrel = ptrel;

    if (drmin > 0.4 || ptrel > 30) pass = true;

    return pass;
}



void Event::wprimeReconstruction(){
    /* Access Wprime reconstruction tool */
    m_wprime = {};
    m_wprime_smp = {};

    if (m_wprimeReco){
        if (m_leptons.size()>0 && m_jets.size()>1){
            Neutrino nu = m_neutrinos.at(0);
            m_wprimeTool->setLepton( m_leptons.at(0) );
            m_wprimeTool->setNeutrino( nu );
            m_wprimeTool->setJets( m_jets );
            m_wprimeTool->setBtagJets( m_btag_jets_default );
            m_wprime = m_wprimeTool->execute();

            Neutrino nu_smp;
            float nuE = sqrt( pow(nu.p4.Px(),2) + pow(nu.p4.Py(),2) + pow(nu.pz_sampling,2));
            nu_smp.p4.SetPxPyPzE( nu.p4.Px(), nu.p4.Py(), nu.pz_sampling, nuE );
            m_wprimeTool->setNeutrino( nu_smp );
            m_wprime_smp = m_wprimeTool->execute();
        }
    }
    else{
    }

    return;
}


void Event::getBtaggedJets( Jet& jet ){
    /* Determine the b-tagging */
    jet.isbtagged["L"] = false;
    jet.isbtagged["M"] = false;
    jet.isbtagged["T"] = false;

    if (jet.bdisc > m_CSVv2L){
        jet.isbtagged["L"] = true;
        m_btag_jets["L"].push_back(jet.index);  // 0 = index of this jet
        if (jet.bdisc > m_CSVv2M){
            jet.isbtagged["M"] = true;
            m_btag_jets["M"].push_back(jet.index);
            if (jet.bdisc > m_CSVv2T){
                jet.isbtagged["T"] = true;
                m_btag_jets["T"].push_back(jet.index);
            }
        }
    }

    return;
}


double Event::getSystEventWeight( const std::string &syst, const int weightIndex ){
    /* Calculate the event weight given some systematic
       -- only call for nominal events and systematic weights
       -- for non-nominal tree systematics, use the nominal event weight

       @param syst          Name of systematic (nominal or some weight systematic)
       @param weightIndex   Index of btagging SF; default to -1
    */
    double syst_event_weight(1.0);

    if (syst.compare("nominal")==0){
        // nominal event weight
        syst_event_weight  = m_nominal_weight;
    }
    else if (syst.find("pileup")!=std::string::npos){
        // pileup event weight
        syst_event_weight  = (**m_weight_mc);
        syst_event_weight *= m_weight_btag_default;
        syst_event_weight *= (m_xsection) * (m_kfactor) * (m_LUMI);
        syst_event_weight /= (m_sumOfWeights);

        syst_event_weight *= **m_weightSystematicsFloats.at(syst);
    }
    else if (syst.find("leptonSF")!=std::string::npos){
        // leptonSF event weight
        syst_event_weight  = (**m_weight_pileup) * (**m_weight_mc);
        syst_event_weight *= m_weight_btag_default;
        syst_event_weight *= (m_xsection) * (m_kfactor) * (m_LUMI);
        syst_event_weight /= (m_sumOfWeights);

        syst_event_weight *= **m_weightSystematicsFloats.at(syst);
    }
    else if (syst.find("bTagSF")!=std::string::npos){
        // bTagSF event weight -- check indices for eigenvector systematics
        syst_event_weight  = (**m_weight_pileup) * (**m_weight_mc);
        syst_event_weight *= (m_xsection) * (m_kfactor) * (m_LUMI);
        syst_event_weight /= (m_sumOfWeights);
    }
    else{
        // safety to catch something weird -- just return 1.0
        cma::WARNING("EVENT : Passed systematic variation, "+syst+", to Event::getSystEventWeight() ");
        cma::WARNING("EVENT : that is inconsistent with the CyMiniAna options of ");
        cma::WARNING("EVENT :     nominal, jvt, pileup, leptonSF, and bTagSF. ");
        cma::WARNING("EVENT : Returning a weight of 1.0. ");
        syst_event_weight = 1.0;
    }

    return syst_event_weight;
}


/*** RETURN PHYSICS INFORMATION ***/
std::vector<int> Event::btag_jets(const std::string &wkpt) const{
    /* Small-R Jet b-tagging */
    std::string tmp_wkpt(wkpt);
    if(m_btag_jets.find(wkpt) == m_btag_jets.end()){
        cma::WARNING("EVENT : B-tagging working point "+wkpt+" does not exist.");
        cma::WARNING("EVENT : Return vector of b-tagged jets for default working point "+m_config->jet_btagWkpt());
        tmp_wkpt = m_config->jet_btagWkpt();
    }
    return m_btag_jets.at(tmp_wkpt);
}

void Event::deepLearningPrediction(){
    /* Deep learning for neutrino eta -- VIPER 
       -- Call this after neutrinos are reconstructed
    */
    m_deepLearningTool->clear();

    if (m_DNNinference){
        cma::DEBUG("EVENT : Calculate DNN ");

        if (m_neutrinos.size()<1 || m_leptons.size()<1)  // nothing to do
            return;

        m_deepLearningTool->clear();
        m_deepLearningTool->setNeutrino( m_neutrinos.at(0) );
        m_deepLearningTool->setMET( m_met );
        m_deepLearningTool->setLepton( m_leptons.at(0) );
        m_deepLearningTool->setJets( m_jets );
        m_deepLearningTool->inference();      //m_leptons.at(0),m_met,m_jets
        m_neutrinos.at(0).viper = m_deepLearningTool->prediction();
    }
    else if (m_DNNtraining){
        // train on events with 1 truth-level neutrino from W boson
        // basic kinematic cuts on lepton, MET, jets
        cma::DEBUG("EVENT : DNN Training ");
        if (m_leptons.size()==1 && m_useTruth){
            cma::DEBUG("EVENT : Begin loop over truth partons ");
            Parton true_nu;
            int n_wdecays2leptons(0);

            for (const auto& p : m_truth_partons){
                if (p.isW && p.child0_idx>=0 && p.child1_idx>=0) {
                    Parton child0 = m_truth_partons.at( p.child0_idx );
                    Parton child1 = m_truth_partons.at( p.child1_idx );
                    if (child0.isNeutrino) {
                        n_wdecays2leptons++;
                        true_nu = child0;
                    }
                    else if (child1.isNeutrino) {
                        n_wdecays2leptons++;
                        true_nu = child1;
                    }
                }
            }

            // only want to train on single lepton events (reco & truth-level)
            if (n_wdecays2leptons==1){
                m_deepLearningTool->setNeutrino( m_neutrinos.at(0) );
                m_deepLearningTool->setTrueNeutrino( true_nu );
                m_deepLearningTool->setMET( m_met );
                m_deepLearningTool->setLepton( m_leptons.at(0) );
                m_deepLearningTool->setJets( m_jets );
                m_deepLearningTool->training();
            } // end training if truth-level neutrino found

        } // end if at least 1 lepton and useTruth
    } // end if training DNN

    return;
}

std::map<std::string,double> Event::deepLearningFeatures(){
    /* Return the DNN features to the outside world -- call after deepLearningPrediction() */
    return m_deepLearningTool->features();
}

/*** RETURN WEIGHTS ***/
float Event::weight_mc(){
    return 1.0; //**m_weight_mc;
}
float Event::weight_pileup(){
    return 1.0; //**m_weight_pileup;
}

float Event::weight_btag(const std::string &wkpt){
    std::string tmp_wkpt(wkpt);
    if(m_weight_btag.find(wkpt) == m_weight_btag.end()){
        cma::WARNING("EVENT : B-tagging working point "+wkpt+" does not exist");
        cma::WARNING("EVENT : Return calo-jet b-tag SF for default working point "+m_config->jet_btagWkpt());
        tmp_wkpt = m_config->jet_btagWkpt();
    }
    return m_weight_btag[tmp_wkpt];
}


// Get weight systematics
std::map<std::string,float> Event::weightSystematicsFloats(){
    /* systematics floats */
    std::map<std::string,float> tmp_weightSystematicsFloats;
    for (const auto& wsf : m_weightSystematicsFloats)
        tmp_weightSystematicsFloats[wsf.first] = **wsf.second;

    return tmp_weightSystematicsFloats;
}

std::map<std::string,std::vector<float> > Event::weightSystematicsVectorFloats(){
    /* weight systematics stored as vectors */
    std::map<std::string,std::vector<float> > tmp_weightSystematicsVectorFloats;
    return tmp_weightSystematicsVectorFloats;
}



/*** RETURN EVENT INFORMATION ***/
void Event::truth(){
    /* Do something with truth information (possibly change type and return information?) */
    return;
}


/*** DELETE VARIABLES ***/
void Event::finalize(){
    // delete variables
    cma::DEBUG("EVENT : Finalize() ");

    delete m_eventNumber;
    delete m_runNumber;
    delete m_lumiblock;

    if (m_useJets){
      delete m_jet_pt;
      delete m_jet_eta;
      delete m_jet_phi;
      delete m_jet_m;
      delete m_jet_bdisc;
      delete m_jet_deepCSV;
      delete m_jet_area;
      delete m_jet_uncorrPt;
      delete m_jet_uncorrE;
    }

    if (m_useLargeRJets){
      delete m_ljet_pt;
      delete m_ljet_eta;
      delete m_ljet_phi;
      delete m_ljet_m;
      delete m_ljet_tau1;
      delete m_ljet_tau2;
      delete m_ljet_tau3;
      delete m_ljet_charge;
      delete m_ljet_SDmass;
      delete m_ljet_subjet0_charge;
      delete m_ljet_subjet0_bdisc;
      delete m_ljet_subjet0_deepCSV;
      delete m_ljet_subjet0_pt;
      delete m_ljet_subjet0_mass;
      delete m_ljet_subjet1_charge;
      delete m_ljet_subjet1_bdisc;
      delete m_ljet_subjet1_deepCSV;
      delete m_ljet_subjet1_pt;
      delete m_ljet_subjet1_mass;
      delete m_ljet_area;
      delete m_ljet_uncorrPt;
      delete m_ljet_uncorrE;
    }

    if (m_useLeptons){
      delete m_el_pt;
      delete m_el_eta;
      delete m_el_phi;
      delete m_el_e;
      delete m_el_charge;
      //delete m_el_iso;
      delete m_el_id_loose;
      delete m_el_id_medium;
      delete m_el_id_tight;
      delete m_el_id_loose_noIso;
      delete m_el_id_medium_noIso;
      delete m_el_id_tight_noIso;

      delete m_mu_pt;
      delete m_mu_eta;
      delete m_mu_phi;
      delete m_mu_e;
      delete m_mu_charge;
      delete m_mu_iso;
      delete m_mu_id_loose;
      delete m_mu_id_medium;
      delete m_mu_id_tight;
    }

    delete m_met_met;
    delete m_met_phi;
    delete m_HTAK8;
    delete m_HTAK4;

    delete m_HLT_Ele45_CaloIdVT_GsfTrkIdT_PFJet200_PFJet50;
    delete m_HLT_Ele50_CaloIdVT_GsfTrkIdT_PFJet165;
    delete m_HLT_Ele115_CaloIdVT_GsfTrkIdT;
    delete m_HLT_Mu40_Eta2P1_PFJet200_PFJet50;
    delete m_HLT_Mu50;
    delete m_HLT_PFHT800;
    delete m_HLT_PFHT900;
    delete m_HLT_AK8PFJet450;
    delete m_HLT_PFHT700TrimMass50;
    delete m_HLT_PFJet360TrimMass30;

    delete m_Flag_goodVertices;
    delete m_Flag_eeBadScFilter;
    delete m_Flag_HBHENoiseFilter;
    delete m_Flag_HBHENoiseIsoFilter;
    delete m_Flag_globalTightHalo2016Filter;
    delete m_Flag_EcalDeadCellTriggerPrimitiveFilter;

    if (m_isMC){
      delete m_mc_pt;
      delete m_mc_eta;
      delete m_mc_phi;
      delete m_mc_e;
      delete m_mc_pdgId;
      delete m_mc_status;
      delete m_mc_isHadTop;
/*
        delete m_weight_mc;
        delete m_weight_pileup;
        delete m_weight_pileup_UP;
        delete m_weight_pileup_DOWN;

        delete m_mc_ht;

        delete m_truth_jet_pt;
        delete m_truth_jet_eta;
        delete m_truth_jet_phi;
        delete m_truth_jet_e;

        delete m_truth_ljet_pt;
        delete m_truth_ljet_eta;
        delete m_truth_ljet_phi;
        delete m_truth_ljet_m;
        delete m_truth_ljet_charge;
        delete m_truth_ljet_subjet0_charge;
        delete m_truth_ljet_subjet0_bdisc;
        delete m_truth_ljet_subjet1_charge;
        delete m_truth_ljet_subjet1_bdisc;
*/
    } // end isMC

    return;
}


// THE END
