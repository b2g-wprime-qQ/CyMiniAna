#ifndef PHYSICSOBJECTS_H
#define PHYSICSOBJECTS_H

/* 
   Physics objects to be used in analyses
*/
#include "TLorentzVector.h"
#include <map>
#include <string>


// easily keep track of isolation and ID working points
enum class lep_id  {LOOSE, MEDIUM, TIGHT, VETO, NONE};
enum class lep_iso {LOOSE, MEDIUM, TIGHT, NONE};
enum class jet_id  {LOOSE, MEDIUM, TIGHT, TIGHTLEPVETO, NONE};


// base object (consistent reference to TLorentzVector)
struct CmaBase {
    TLorentzVector p4;
};


// Struct for particle flow jets
// -- common to all types of jets
struct Jet : CmaBase{
    float cMVAv2;
    float CSVv2;
    float CvsL;
    float CvsB;
    std::map<std::string, bool> isbtagged;
    int true_flavor;
    float btagSF;
    float btagSF_UP;
    float btagSF_DN;

    float partonFlavour;
    float hadronFlavour;
    float nMultip;
    float nHadEnergy;
    float nEMEnergy;
    float cHadEnergy;
    float cEMEnergy;
    float cMultip;
    float muonEnergy;

    float jecFactor0;
    float jetArea;
    float ptResolution;
    float smearedPt;

    float JERSF;
    float JERSF_UP;
    float JERSF_DN;

    std::vector<int> keys;

    float charge;
    int index;    // index in vector of jets

    bool loose;
    bool medium;
    bool tight;
    bool tightlepveto;
};

struct Ljet : Jet{
    // extra ljet attributes
    int isGood;
    // substructure
    float tau1;
    float tau2;
    float tau3;
    float tau21;
    float tau32;
    float softDropMass;
    std::vector<Jet> subjets;
    float charge;
    float vSubjetIndex0;
    float vSubjetIndex1;
};



// Extra lepton attributes
struct Lepton : CmaBase{
    // common to electrons and muons
    int charge;
    bool isElectron;
    bool isMuon;
    int index;       // index in vector of leptons

    float key;
    float miniIso;
    bool loose;
    bool medium;
    bool tight;
};

struct Electron : Lepton{
    // extra electron attributes
    Electron() {
        isElectron = true;
        isMuon     = false;
    }

    float charge;
    float iso03;
    float iso03db;
    float SCEta;
    float SCPhi;
    bool vidHEEP;
    bool vidHEEPnoiso;
    bool vidVeto;
    bool vidVetonoiso;
    bool vidLoose;
    bool vidLoosenoiso;
    bool vidMedium;
    bool vidMediumnoiso;
    bool vidTight;
    bool vidTightnoiso;
    float vidMvaGPvalue;
    float vidMvaGPcateg;
    float vidMvaHZZvalue;
    float vidMvaHZZcateg;

    float vetoSF;
    float looseSF;
    float mediumSF;
    float tightSF;
    float recoSF;
    float vetoSF_UP;
    float looseSF_UP;
    float mediumSF_UP;
    float tightSF_UP;
    float recoSF_UP;
    float vetoSF_DN;
    float looseSF_DN;
    float mediumSF_DN;
    float tightSF_DN;
    float recoSF_DN;

    // electron ID variables (to re-calculate)
    float Dz;
    float Dxy;
    float HoE;
    float scEta;
    float dPhiIn;
    float ooEmooP;
    int missHits;
    float RelIsoEA;
    float dEtaInSeed;
    float full5x5siee;
    bool hasMatchedConVeto;


};
struct Muon : Lepton{
    // extra muon attributes
    Muon() {
        isElectron = false;
        isMuon     = true;
    }

    float iso04;
    float soft;
    float medium2016;
    float hightPt;
};

struct Neutrino : CmaBase{
    // extra neutrino attributes
};

struct MET : CmaBase{
    // extra MET attributes
    float uncorrPhi;
    float uncorrPt;
    float uncorrSumEt;
};


// Truth information
struct Parton : CmaBase {
    int pdgId;
    int index;        // index in vector of truth partons
    int parent0_idx;  // index in truth record of parent0
    int parent1_idx;  // index in truth record of parent1
    int child0_idx;   // index in truth record of child0
    int child1_idx;   // index in truth record of child1
    int charge;

    // Heavy Object Booleans
    bool isTop;
    bool isW;
    bool isZ;
    bool isHiggs;
    // Lepton Booleans
    bool isLepton;
    bool isTau;
    bool isElectron;
    bool isMuon;
    bool isNeutrino;
    // Quark Booleans
    bool isQuark;
    bool isBottom;
    bool isLight;
};

// VLQ (assuming T->bW)
struct VLQ : CmaBase{
};

// Wprime (assuming W'->bT for now)
struct Wprime : CmaBase{
    VLQ vlq;
    Jet jet;
};

#endif

