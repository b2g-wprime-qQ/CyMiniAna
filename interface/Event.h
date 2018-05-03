#ifndef EVENT_H
#define EVENT_H

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TSystem.h"
#include "TEfficiency.h"
#include "TMath.h"
#include "TLorentzVector.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TTreeReaderArray.h"
#include "TParameter.h"
#include "TEnv.h"
#include "TF1.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <memory>
#include <set>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "Analysis/CyMiniAna/interface/physicsObjects.h"
#include "Analysis/CyMiniAna/interface/configuration.h"
#include "Analysis/CyMiniAna/interface/truthMatching.h"
#include "Analysis/CyMiniAna/interface/deepLearning.h"
#include "Analysis/CyMiniAna/interface/neutrinoReco.h"
#include "Analysis/CyMiniAna/interface/wprimeReco.h"


// Event Class
class Event {
  public:
    // Constructor
    Event( TTreeReader &myReader, configuration &cmaConfig);
    Event( const Event &obj);

    // Destructor
    virtual ~Event();

    // create hash tables in truth/reco TTree to match truth <-> reco events
    // uses configuration option matchTruthToReco to match truth to reco (reco loop)
    // OR match reco to truth (truth loop, for acceptance studies)
    void matchTruthWithReco();
    // check during looping over truth events, if reco event match is found
    bool isValidRecoEntry() const {return (m_entry > (long long)-1);}

    // Execute the event (load information and setup objects)
    virtual void execute(Long64_t entry);
    virtual void updateEntry(Long64_t entry);

    // Setup physics information
    void initialize_leptons();
    void initialize_neutrinos();
    void initialize_jets();
    void initialize_ljets();
    void initialize_eventWeights();
    void initialize_weights();
    void initialize_kinematics();
    void initialize_truth();
    void initialize_filters();
    void initialize_triggers();

    virtual double getSystEventWeight(const std::string &syst, const int weightIndex=-1);

    // Clear stuff;
    virtual void finalize();
    virtual void clear();

    // Get physics information
    std::vector<Lepton> leptons() const {return m_leptons;}
    std::vector<Muon> muons() const {return m_muons;}
    std::vector<Electron> electrons() const {return m_electrons;}
    std::vector<Neutrino> neutrinos() const {return m_neutrinos;}
    std::vector<Ljet> ljets() const {return m_ljets;}
    std::vector<Jet>  jets() const {return m_jets;}

    // Get truth physics information 
    void truth();
    std::vector<Lepton> truth_leptons() const {return m_truth_leptons;}
    std::vector<Neutrino> truth_neutrinos() const {return m_truth_neutrinos;}
    std::vector<Ljet> truth_ljets() const {return m_truth_ljets;}
    std::vector<Jet>  truth_jets() const {return m_truth_jets;}

    virtual MET met() const {return m_met;}
    virtual float HT() const {return m_HT;}
    virtual float ST() const {return m_ST;}

    virtual void getBtaggedJets( Jet& jet );
    virtual std::vector<int> btag_jets(const std::string &wkpt) const;
    virtual std::vector<int> btag_jets() const {return m_btag_jets_default;}  // using configured b-tag WP

    long long entry() const { return m_entry; }
    virtual unsigned long long eventNumber() const {return **m_eventNumber;}
    virtual unsigned int runNumber() const {return **m_runNumber;}
    virtual unsigned int lumiblock() const {return **m_lumiblock;}
    virtual std::string treeName() {return m_treeName;}
    virtual float xsection() const {return m_xsection;}
    virtual float kfactor() const {return m_kfactor;}
    virtual float sumOfWeights() const {return m_sumOfWeights;}

    std::map<std::string,unsigned int> filters() const {return m_filters;}
    std::map<std::string,unsigned int> triggers() const {return m_triggers;}

    // Functions for external tools/information
    void wprimeReconstruction();    // reconstructing Wprime (interface with tool)
    bool customIsolation( Lepton& lep );
    void deepLearningPrediction();

    // Get weights
    virtual float nominal_weight() const {return m_nominal_weight;}
    float weight_mc();
    float truth_weight_mc();
    float weight_pileup();
    float weight_lept_eff();
    float weight_btag() const {return m_weight_btag_default;}
    float weight_btag(const std::string &wkpt);

    // Get weight systematics
    virtual std::map<std::string,float > weightSystematicsFloats();
    virtual std::map<std::string,std::vector<float> > weightSystematicsVectorFloats();

  protected:

    // general information
    configuration *m_config;
    TTreeReader &m_ttree;
    TTreeReader m_truth_tree;
    std::string m_treeName;
    std::string m_fileName;
    bool m_grid;
    bool m_isMC;
    long long m_entry;
    long long m_truth_entry;

    bool m_useTruth;
    bool m_useJets;
    bool m_useLargeRJets;
    bool m_useLeptons;
    bool m_useNeutrinos;
    bool m_neutrinoReco;
    bool m_useWprime;
    bool m_wprimeReco;
    bool m_DNNinference;
    bool m_DNNtraining;
    bool m_useDNN;
    bool m_getDNN;
    float m_DNN;                       // DNN score
    bool m_kinematicReco;

    // event weight information
    double m_nominal_weight;
    double m_xsection;
    double m_kfactor;
    double m_sumOfWeights;
    double m_LUMI;
    std::map<int, float> m_mapXSection; // map DSID to XSection
    std::map<int, float> m_mapKFactor;  // map DSID to KFactor
    std::map<int, float> m_mapAMI;      // map DSID to sum of weights

    // physics object information
    std::vector<Lepton> m_leptons;
    std::vector<Muon> m_muons;
    std::vector<Electron> m_electrons;
    std::vector<Neutrino> m_neutrinos;
    std::vector<Ljet> m_ljets;
    std::vector<Jet>  m_jets;
    MET m_met;

    // truth physics object information
    std::vector<Parton> m_truth_partons;
    std::vector<Lepton> m_truth_leptons;
    std::vector<Neutrino> m_truth_neutrinos;
    std::vector<Ljet> m_truth_ljets;
    std::vector<Jet>  m_truth_jets;

    // b-tagged calo jets with various WP
    std::map<std::string, std::vector<int> > m_btag_jets;
    std::vector<int> m_btag_jets_default;
    float m_cMVAv2L;
    float m_cMVAv2M;
    float m_cMVAv2T;
    float m_CSVv2L;
    float m_CSVv2M;
    float m_CSVv2T;

    // kinematics
    float m_HT_ak4;
    float m_HT_ak8;
    float m_HT;
    float m_ST;

    // nominal b-tagging weight maps
    std::map<std::string, float> m_weight_btag;
    float m_weight_btag_default;
    // Maps to keep track of weight systematics
    std::map<std::string,TTreeReaderValue<float> * > m_weightSystematicsFloats;
    std::map<std::string,TTreeReaderValue<std::vector<float>> * > m_weightSystematicsVectorFloats;
    std::vector<std::string> m_listOfWeightSystematics;

    // External tools
    NeutrinoReco* m_neutrinoRecoTool;
    WprimeReco* m_wprimeTool;
    DeepLearning* m_deepLearningTool;
    truthMatching* m_truthMatchingTool;

    std::map<std::string,unsigned int> m_filters;
    std::map<std::string,unsigned int> m_triggers;

    // ***********************************
    // TTree variables [all possible ones]
    // ***********************************
    // Event info 
    TTreeReaderValue<unsigned long long> * m_eventNumber;
    TTreeReaderValue<unsigned int> * m_runNumber;
    TTreeReaderValue<unsigned int> * m_lumiblock;
    TTreeReaderValue<float> * m_treeXSection;
    TTreeReaderValue<float> * m_treeKFactor;
    TTreeReaderValue<float> * m_treeSumOfWeights;
    TTreeReaderValue<unsigned int> * m_npv;
    TTreeReaderValue<float> * m_rho;
    TTreeReaderValue<unsigned int> * m_true_pileup;

    // MET
    TTreeReaderValue<float> * m_met_met;
    TTreeReaderValue<float> * m_met_phi;
    TTreeReaderValue<float> * m_HTAK8;
    TTreeReaderValue<float> * m_HTAK4;

    // Leptons
    TTreeReaderValue<std::vector<float>> * m_el_pt;
    TTreeReaderValue<std::vector<float>> * m_el_eta;
    TTreeReaderValue<std::vector<float>> * m_el_phi;
    TTreeReaderValue<std::vector<float>> * m_el_e;
    TTreeReaderValue<std::vector<float>> * m_el_charge;
    TTreeReaderValue<std::vector<float>> * m_el_iso;
    TTreeReaderValue<std::vector<unsigned int>> * m_el_id_loose;
    TTreeReaderValue<std::vector<unsigned int>> * m_el_id_medium;
    TTreeReaderValue<std::vector<unsigned int>> * m_el_id_tight;
    TTreeReaderValue<std::vector<unsigned int>> * m_el_id_loose_noIso;
    TTreeReaderValue<std::vector<unsigned int>> * m_el_id_medium_noIso;
    TTreeReaderValue<std::vector<unsigned int>> * m_el_id_tight_noIso;

    TTreeReaderValue<std::vector<float>> * m_mu_pt;
    TTreeReaderValue<std::vector<float>> * m_mu_eta;
    TTreeReaderValue<std::vector<float>> * m_mu_phi;
    TTreeReaderValue<std::vector<float>> * m_mu_e;
    TTreeReaderValue<std::vector<float>> * m_mu_charge;
    TTreeReaderValue<std::vector<float>> * m_mu_iso;
    TTreeReaderValue<std::vector<unsigned int>> * m_mu_id_loose;
    TTreeReaderValue<std::vector<unsigned int>> * m_mu_id_medium;
    TTreeReaderValue<std::vector<unsigned int>> * m_mu_id_tight;

    // Reconstructed neutrinos
    TTreeReaderValue<std::vector<float>> * m_nu_pt;
    TTreeReaderValue<std::vector<float>> * m_nu_eta;
    TTreeReaderValue<std::vector<float>> * m_nu_phi;

    // large-R jet info
    TTreeReaderValue<float> * m_dnn_score;

    TTreeReaderValue<std::vector<float>> * m_ljet_pt;
    TTreeReaderValue<std::vector<float>> * m_ljet_eta;
    TTreeReaderValue<std::vector<float>> * m_ljet_phi;
    TTreeReaderValue<std::vector<float>> * m_ljet_m;
    TTreeReaderValue<std::vector<float>> * m_ljet_tau1;
    TTreeReaderValue<std::vector<float>> * m_ljet_tau2;
    TTreeReaderValue<std::vector<float>> * m_ljet_tau3;
    TTreeReaderValue<std::vector<float>> * m_ljet_BEST_t;
    TTreeReaderValue<std::vector<float>> * m_ljet_BEST_w;
    TTreeReaderValue<std::vector<float>> * m_ljet_BEST_z;
    TTreeReaderValue<std::vector<float>> * m_ljet_BEST_h;
    TTreeReaderValue<std::vector<float>> * m_ljet_BEST_j;
    TTreeReaderValue<std::vector<int>> * m_ljet_BEST_class;
    TTreeReaderValue<std::vector<float>> * m_ljet_charge;
    TTreeReaderValue<std::vector<float>> * m_ljet_SDmass;
    TTreeReaderValue<std::vector<float>> * m_ljet_bdisc;
    TTreeReaderValue<std::vector<float>> * m_ljet_area;
    TTreeReaderValue<std::vector<float>> * m_ljet_subjet0_charge;
    TTreeReaderValue<std::vector<float>> * m_ljet_subjet0_bdisc;
    TTreeReaderValue<std::vector<float>> * m_ljet_subjet0_deepCSV;
    TTreeReaderValue<std::vector<float>> * m_ljet_subjet0_pt;
    TTreeReaderValue<std::vector<float>> * m_ljet_subjet0_mass;
    TTreeReaderValue<std::vector<float>> * m_ljet_subjet0_tau1;
    TTreeReaderValue<std::vector<float>> * m_ljet_subjet0_tau2;
    TTreeReaderValue<std::vector<float>> * m_ljet_subjet0_tau3;
    TTreeReaderValue<std::vector<float>> * m_ljet_subjet1_charge;
    TTreeReaderValue<std::vector<float>> * m_ljet_subjet1_bdisc;
    TTreeReaderValue<std::vector<float>> * m_ljet_subjet1_deepCSV;
    TTreeReaderValue<std::vector<float>> * m_ljet_subjet1_pt;
    TTreeReaderValue<std::vector<float>> * m_ljet_subjet1_mass;
    TTreeReaderValue<std::vector<float>> * m_ljet_subjet1_tau1;
    TTreeReaderValue<std::vector<float>> * m_ljet_subjet1_tau2;
    TTreeReaderValue<std::vector<float>> * m_ljet_subjet1_tau3;
    TTreeReaderValue<std::vector<float>> * m_ljet_uncorrPt;
    TTreeReaderValue<std::vector<float>> * m_ljet_uncorrE;

    // truth large-R jet info
    TTreeReaderValue<std::vector<float>> * m_truth_ljet_pt;
    TTreeReaderValue<std::vector<float>> * m_truth_ljet_eta;
    TTreeReaderValue<std::vector<float>> * m_truth_ljet_phi;
    TTreeReaderValue<std::vector<float>> * m_truth_ljet_m;
    TTreeReaderValue<std::vector<float>> * m_truth_ljet_tau1;
    TTreeReaderValue<std::vector<float>> * m_truth_ljet_tau2;
    TTreeReaderValue<std::vector<float>> * m_truth_ljet_tau3;
    TTreeReaderValue<std::vector<float>> * m_truth_ljet_SDmass;
    TTreeReaderValue<std::vector<float>> * m_truth_ljet_charge;
    TTreeReaderValue<std::vector<float>> * m_truth_ljet_area;
    TTreeReaderValue<std::vector<float>> * m_truth_ljet_subjet0_charge;
    TTreeReaderValue<std::vector<float>> * m_truth_ljet_subjet0_bdisc;
    TTreeReaderValue<std::vector<float>> * m_truth_ljet_subjet1_charge;
    TTreeReaderValue<std::vector<float>> * m_truth_ljet_subjet1_bdisc;
    TTreeReaderValue<std::vector<float>> * m_truth_ljet_subjet1_deepCSV;
    TTreeReaderValue<std::vector<float>> * m_truth_ljet_subjet1_pt;
    TTreeReaderValue<std::vector<float>> * m_truth_ljet_subjet1_mass;


    // Jet info
    TTreeReaderValue<std::vector<float>> * m_jet_pt;
    TTreeReaderValue<std::vector<float>> * m_jet_eta;
    TTreeReaderValue<std::vector<float>> * m_jet_phi;
    TTreeReaderValue<std::vector<float>> * m_jet_m;
    TTreeReaderValue<std::vector<float>> * m_jet_bdisc;
    TTreeReaderValue<std::vector<float>> * m_jet_deepCSV;
    TTreeReaderValue<std::vector<float>> * m_jet_area;
    TTreeReaderValue<std::vector<float>> * m_jet_uncorrPt;
    TTreeReaderValue<std::vector<float>> * m_jet_uncorrE;


    // Truth jet info
    TTreeReaderValue<std::vector<float>> * m_truth_jet_pt;
    TTreeReaderValue<std::vector<float>> * m_truth_jet_eta;
    TTreeReaderValue<std::vector<float>> * m_truth_jet_phi;
    TTreeReaderValue<std::vector<float>> * m_truth_jet_e;


    TTreeReaderValue<int> * m_leptop_jet;
    TTreeReaderValue<int> * m_hadtop_ljet;

    // Reconstructed VLQ, Wprime
    TTreeReaderValue<float> * m_vlq_pt;
    TTreeReaderValue<float> * m_vlq_eta;
    TTreeReaderValue<float> * m_vlq_phi;
    TTreeReaderValue<float> * m_vlq_e;
    TTreeReaderValue<int> * m_jet_vlq_index;
    TTreeReaderValue<float> * m_wprime_pt;
    TTreeReaderValue<float> * m_wprime_eta;
    TTreeReaderValue<float> * m_wprime_phi;
    TTreeReaderValue<float> * m_wprime_e;
    TTreeReaderValue<int> * m_jet_wprime_index;

    // Truth info
    TTreeReaderValue<float> * m_weight_mc;
    TTreeReaderValue<float> * m_weight_pileup;
    TTreeReaderValue<float> * m_weight_lept_eff;
    TTreeReaderValue<float> * m_weight_pileup_UP;
    TTreeReaderValue<float> * m_weight_pileup_DOWN;

    TTreeReaderValue<std::vector<float>> * m_mc_ht;
    TTreeReaderValue<std::vector<float>> * m_mc_pt;
    TTreeReaderValue<std::vector<float>> * m_mc_eta;
    TTreeReaderValue<std::vector<float>> * m_mc_phi;
    TTreeReaderValue<std::vector<float>> * m_mc_e;
    TTreeReaderValue<std::vector<int>> * m_mc_pdgId;
    TTreeReaderValue<std::vector<int>> * m_mc_status;
    TTreeReaderValue<std::vector<int>> * m_mc_isHadTop;
    TTreeReaderValue<std::vector<int>> * m_mc_parent_idx;
    TTreeReaderValue<std::vector<int>> * m_mc_child0_idx;
    TTreeReaderValue<std::vector<int>> * m_mc_child1_idx;

    // HLT 
    TTreeReaderValue<unsigned int> * m_HLT_Ele45_CaloIdVT_GsfTrkIdT_PFJet200_PFJet50;
    TTreeReaderValue<unsigned int> * m_HLT_Ele50_CaloIdVT_GsfTrkIdT_PFJet165;
    TTreeReaderValue<unsigned int> * m_HLT_Ele115_CaloIdVT_GsfTrkIdT;
    TTreeReaderValue<unsigned int> * m_HLT_Mu40_Eta2P1_PFJet200_PFJet50;
    TTreeReaderValue<unsigned int> * m_HLT_Mu50;
    TTreeReaderValue<unsigned int> * m_HLT_TkMu50;
    TTreeReaderValue<unsigned int> * m_HLT_PFHT800;
    TTreeReaderValue<unsigned int> * m_HLT_PFHT900;
    TTreeReaderValue<unsigned int> * m_HLT_AK8PFJet450;
    TTreeReaderValue<unsigned int> * m_HLT_PFHT700TrimMass50;
    TTreeReaderValue<unsigned int> * m_HLT_PFJet360TrimMass30;

    // Filters
    TTreeReaderValue<unsigned int> * m_Flag_goodVertices;
    TTreeReaderValue<unsigned int> * m_Flag_eeBadScFilter;
    TTreeReaderValue<unsigned int> * m_Flag_HBHENoiseFilter;
    TTreeReaderValue<unsigned int> * m_Flag_HBHENoiseIsoFilter;
    TTreeReaderValue<unsigned int> * m_Flag_globalTightHalo2016Filter;
    TTreeReaderValue<unsigned int> * m_Flag_EcalDeadCellTriggerPrimitiveFilter;
};

#endif
