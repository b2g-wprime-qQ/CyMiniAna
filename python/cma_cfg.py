"""
Created:      13 December 2017
Last Updated: 30 January  2018

Dan Marley
daniel.edison.marley@cernSPAMNOT.ch
Texas A&M University
---

Primary script for running CyMiniAna
over EDMNtuples (from B2GAnaFW) in CMSSW.
Based on VLQAna:
  https://github.com/dmajumder/VLQAna

  Requires a CMSSW environment!

To run:
  cmsRun python/cma_cfg.py
"""
import os
import sys

import FWCore.ParameterSet.Config as cms
from FWCore.ParameterSet.VarParsing import VarParsing
from Analysis.EventCounter.eventcounter_cfi import eventCounter
from Analysis.CyMiniAna.physObjects_cfi import *
from Analysis.CyMiniAna.physObjectsEDMLabels import *


## STANDARD OPTIONS
options = VarParsing('analysis')

options.register('isMC', True,
    VarParsing.multiplicity.singleton,
    VarParsing.varType.bool,
    "Monte Carlo sample" )
options.register('useTruth', False,
    VarParsing.multiplicity.singleton,
    VarParsing.varType.bool,
    "Use truth information" )
options.register('useJets', True,
    VarParsing.multiplicity.singleton,
    VarParsing.varType.bool,
    "Use AK4 jets" )
options.register('useLargeRJets', True,
    VarParsing.multiplicity.singleton,
    VarParsing.varType.bool,
    "Use AK8 jets" )
options.register('useLeptons', True,
    VarParsing.multiplicity.singleton,
    VarParsing.varType.bool,
    "Use leptons" )
options.register('useNeutrinos', False,
    VarParsing.multiplicity.singleton,
    VarParsing.varType.bool,
    "Use neutrinos" )
options.parseArguments()


## PROCESS

process = cms.Process("CyMiniAna")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(options.maxEvents) )
process.source    = cms.Source("PoolSource",
        fileNames = cms.untracked.vstring(
#          'root://cmsxrootd.fnal.gov//store/user/oiorio/samples/June/05June/B2GAnaFW_80X_V3p2_June/TT_TuneCUETP8M2T4_13TeV-powheg-pythia8/RunIISummer16MiniAODv2/TT_TuneCUETP8M2T4_13TeV-powheg-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1_B2GAnaFW_80X_V3p2_June/170605_115340/0000/B2GEDMNtuple_1.root'
#           'file:config/B2GEDMNtuple_1.root'
           'root://cmsxrootd.fnal.gov//store/user/oiorio/samples/May/17May/B2GAnaFW_80X_V3p1/SingleMuon/Run2016B/SingleMuon/Run2016B-03Feb2017_ver2-v2_B2GAnaFW_80X_V3p1/170517_122621/0000/B2GEDMNtuple_105.root'
	)
)


process.TFileService = cms.Service("TFileService", fileName = cms.string("output.root") )


## CMA Producer
process.CMAProducer = cms.EDProducer('CMAProducer',
    isMC = cms.bool(options.isMC),
    useJets  = cms.bool(options.useJets),
    useTruth = cms.bool(options.useTruth),
    useLargeRJets = cms.bool(options.useLargeRJets),
    useLeptons    = cms.bool(options.useLeptons),
    useNeutrinos  = cms.bool(options.useNeutrinos),
    buildNeutrinos = cms.bool(False),
    kinematicReco  = cms.bool(False),
    LUMI = cms.double(1.0),
    cleanEvents  = cms.bool(False),
    metadataFile = cms.string(""),

    # Physics Objects
    # - labels to access data
    neutrinoLabels  = neutrinoLabels,
    muonLabels      = muonLabels,
    electronLabels  = electronLabels,
    jetLabels       = jetLabels,
    largeRjetLabels = largeRJetLabels,
    METLabels = METLabels,

    # - selection on objects (pT,eta,ID,b-tagging,etc.)
    objSelectionParams = objectSelectionParams,
)


## EVENT SELECTION
#  triggers
#  https://twiki.cern.ch/twiki/bin/view/CMS/TopTrigger#TOP_trigger_80X_reHLT_samples
#  looking at isolated triggers for now, will need higher-pT for non-iso triggers

hltPaths = ["HLT_Ele32_eta2p1_WPTight_Gsf_v8",
            "HLT_IsoMu24_v4",
            "HLT_IsoTkMu24_v4"
           ]
if not options.isMC:
    hltPaths = [i.replace(i[-1],"*") for i in hltPaths]

process.selection = cms.EDFilter("eventSelection",
    selection = cms.string("pre"),
    cutsfile  = cms.string("config/cuts_pre.txt"),
    trigNameLabel = cms.InputTag("TriggerUserData", "triggerNameTree"),
    trigBitLabel  = cms.InputTag("TriggerUserData", "triggerBitTree"),
    HLTPaths = cms.vstring(hltPaths)  
)


## HISTOGRAMMER
process.histograms = cms.EDAnalyzer("histogrammer",
    isMC = cms.bool(options.isMC),
    useTruth = cms.bool(options.useTruth),
    useJets  = cms.bool(options.useJets),
    useLargeRJets = cms.bool(options.useLargeRJets),
    useLeptons    = cms.bool(options.useLeptons),
    useNeutrinos  = cms.bool(options.useNeutrinos),
    useSystWeights = cms.bool(False),
    weightSystematicsFile       = cms.string("config/weightSystematics.txt"),
    weightVectorSystematicsFile = cms.string("config/weightVectorSystematics.txt"),
)


## EVENT SAVER FLAT NTUPLE
process.tree = cms.EDAnalyzer("EventSaverFlatNtuple",
    isMC = cms.bool(options.isMC),
    useTruth = cms.bool(options.useTruth),
    useJets  = cms.bool(options.useJets),
    useLargeRJets = cms.bool(options.useLargeRJets),
    useLeptons    = cms.bool(options.useLeptons),
    useNeutrinos  = cms.bool(options.useNeutrinos),
    rhoLabel = cms.InputTag("vertexInfo","rho"),
    npvLabel = cms.InputTag("vertexInfo","npv"),
    runnoLabel      = cms.InputTag("eventInfo", "evtInfoRunNumber"),
    lumisecLabel    = cms.InputTag("eventInfo", "evtInfoLumiBlock"),
    evtnoLabel      = cms.InputTag("eventInfo", "evtInfoEventNumber"),
    puNtrueIntLabel = cms.InputTag("eventUserData", "puNtrueInt"),
)


## -- Count events before & after selection
##    https://github.com/dmajumder/EventCounter
process.initial = eventCounter.clone( isData=(not options.isMC) )
process.final   = eventCounter.clone( isData=(not options.isMC) )


#process.out = cms.OutputModule("PoolOutputModule",
#                               fileName = cms.untracked.string("ana_out.root"),
#                               SelectEvents   = cms.untracked.PSet( SelectEvents = cms.vstring('p') ),
#                               outputCommands = cms.untracked.vstring('drop *')
#                               )
#process.outpath = cms.EndPath(process.out)

process.p = cms.Path(
    process.initial*
    process.CMAProducer*
    process.selection*
    process.histograms*
    process.tree*
    process.final
)


## THE END
