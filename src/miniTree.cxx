/*
Created:        --
Last Updated:   12 April 2018

Dan Marley
daniel.edison.marley@cernSPAMNOT.ch
Texas A&M University
-----

Create and fill TTree.
*/
#include "Analysis/CyMiniAna/interface/miniTree.h"


miniTree::miniTree(configuration &cmaConfig) : 
  m_config(&cmaConfig){
    m_selections = m_config->selections();
  }

miniTree::~miniTree() {}



void miniTree::initialize(TTree* t, TFile& outputFile, const std::string directory, const int cloneFactor) {
    /* Setup the new tree

       @param t    TTree from the original file
       @param directory    Directory the tree may be stored under
       @param cloneFactor  Value used in cloning the tree 
                           (0=clone no events, just branch names; -1=clone all data)
    */
    outputFile.cd(directory.c_str());
    m_oldTTree = t;

    m_ttree = m_oldTTree->CloneTree(cloneFactor);
    cma::getListOfBranches(m_oldTTree,m_listOfBranches);

    createBranches();
    disableBranches();

    return;
}


void miniTree::createBranches(){
    /* Setup new branches if they don't already exist! */
    m_passSelection.resize( m_selections.size() );     // values based on the selection(s)
    unsigned int ss(0);
    for (const auto& sel : m_selections){
        if (branch_exists(sel)) continue;
        m_passSelection.at(ss) = 0;
        m_ttree->Branch( sel.c_str(), &m_passSelection.at(ss), (sel+"/i").c_str() ); // unsigned int 0,1
        ss++;
    }

    return;
}


bool miniTree::branch_exists(const std::string& br){
    /* Check if branch exists in the tree already 
       > True if it exists; False if it does not exist
    */
    return (std::find(m_listOfBranches.begin(), m_listOfBranches.end(), br) != m_listOfBranches.end());
}


void miniTree::disableBranches(){
    /* Disable branches in output file 
       > m_ttree->SetBranchStatus(branch, 0);
    */
    return;
} 


void miniTree::saveEvent(Event& event, const std::vector<unsigned int>& evtsel_decisions) {
    /* Save the event to the ttree! */
    cma::DEBUG("MINITREE : Load the entry to be saved");
    m_oldTTree->GetEntry( event.entry() );  // make sure the original values are loaded for this event
                                            // otherwise only the branches accessed in Event are copied (!?)

    // set all decisions to false if they aren't passed here
    unsigned int n_sels = m_selections.size();
    bool generateDecisions = (evtsel_decisions.size()<1);

    for (unsigned int idx=0; idx<n_sels; idx++)
        m_passSelection.at(idx) = (generateDecisions) ? 0 : evtsel_decisions.at(idx); // set to 0 by default


    cma::DEBUG("MINITREE : Fill the tree");
    m_ttree->Fill();

    return;
}


void miniTree::finalize(){
    /* Finalize the class */
}

// THE END
