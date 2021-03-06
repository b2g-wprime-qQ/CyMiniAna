#ifndef FLATTREE_H_
#define FLATTREE_H_

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TSystem.h"
#include "TMath.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TTreeReaderArray.h"

#include <memory>
#include <set>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "Analysis/CyMiniAna/interface/Event.h"
#include "Analysis/CyMiniAna/interface/physicsObjects.h"
#include "Analysis/CyMiniAna/interface/eventSelection.h"
#include "Analysis/CyMiniAna/interface/configuration.h"


class flatTree4ML {
  public:
    // Default - so root can load based on a name;
    flatTree4ML(configuration &cmaConfig);

    // Default - so we can clean up;
    virtual ~flatTree4ML();

    // Run once at the start of the job;
    virtual void initialize(TFile& outputFile);

    // Run for every event (in every systematic) that needs saving;
    virtual void saveEvent(const std::map<std::string,double> features);

    // Clear stuff;
    virtual void finalize();


  protected:

    TTree * m_ttree;
    TTree * m_metadataTree;
    configuration * m_config;

    /**** Training branches ****/
    // weights for inputs
    float m_xsection;
    float m_kfactor;
    float m_sumOfWeights;
    float m_weight;
    float m_nominal_weight;

    // Deep learning features
    float m_target;    // neutrino pz

    float m_met_met;
    float m_met_phi;
    float m_mtw;
    float m_lepton_pt;
    float m_lepton_eta;
    float m_deltaPhi_lep_met;

    int m_n_jets;
    float m_deltaPhi_j0_met_phi;
    float m_deltaPhi_j1_met_phi;
    float m_deltaPhi_j2_met_phi;
    float m_deltaPhi_j3_met_phi;

    float m_jet0_bdisc;
    float m_jet1_bdisc;
    float m_jet2_bdisc;
    float m_jet3_bdisc;

    float m_jet0_ptrel;
    float m_jet1_ptrel;
    float m_jet2_ptrel;
    float m_jet3_ptrel;

    float m_nu_pz_standard;
    float m_nu_pz_sampling;

    /**** Metadata ****/
    // which sample has which target value
    // many ROOT files will be merged together to do the training!
    std::string m_name;
    unsigned int m_target_value;
    unsigned int m_nEvents;
};

#endif

