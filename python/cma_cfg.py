"""
Created:      13 December 2017
Last Updated: 14 January  2018

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
  cmsRun python/cma_cfg.py <config.txt>

where <config.txt> is the configuration file
for basic options.
"""
import os
import sys

import FWCore.ParameterSet.Config as cms

from Analysis.EventCounter.eventcounter_cfi import eventCounter
from Analysis.CyMiniAna.CMAProducer_cfi import *  # 'cma'
from Analysis.CyMiniAna.configuration import Configuration
from Analysis.CyMiniAna.histogrammer_cfi import hist
from Analysis.CyMiniAna.eventSelection_cfi import evtSel
from Analysis.CyMiniAna.EventSaverFlatNtuple_cfi import flat

## Configuration options ##
print " Setup configuration "



argument = 2 if sys.argv[0]=="cmsRun" else 1  # name of script to pass to configuration

config = Configuration( sys.argv[argument] )
config.initialize()

nEventsToProcess = config.NEvents()
outputFileName   = config.outputFileName()
filenames = config.filenames()   # list of files
isMC      = config.isMC( filenames[0] )   # check the first file to see if we are running over MC or Data

## PROCESS
print " Begin the process "
process = cms.Process("CyMiniAna")

process.source       = cms.Source("PoolSource",fileNames = cms.untracked.vstring(filenames) )
process.maxEvents    = cms.untracked.PSet( input = cms.untracked.int32(nEventsToProcess) )
#process.TFileService = cms.Service("TFileService",
#                                   fileName = cms.string(outputFileName),
#                                   closeFileFast = cms.untracked.bool(True) )
#process.content      = cms.EDAnalyzer('EventContentAnalyzer')

## -- Load Modules
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger = cms.Service("MessageLogger",
                            destinations = cms.untracked.vstring('cout'),
                            cout         = cms.untracked.PSet(
                                           threshold = cms.untracked.string('WARNING') ),
                        )

## -- Histogrammer
print " Histogrammer "
process.histogrammer = hist.clone()
process.histogrammer.isMC = cms.bool(isMC)

process.histogrammer.useTruth = cms.bool(config.useTruth())
process.histogrammer.useJets  = cms.bool(config.useJets())
process.histogrammer.useLargeRJets  = cms.bool(config.useLargeRJets())
process.histogrammer.useNeutrinos   = cms.bool(config.useNeutrinos())
process.histogrammer.useLeptons     = cms.bool(config.useLeptons())
process.histogrammer.useSystWeights = cms.bool(config.useSystWeights())
process.histogrammer.weightSystematicsFile = cms.string(config.weightSystematicsFile())
process.histogrammer.weightVectorSystematicsFile = cms.string(config.weightVectorSystematicsFile())

## -- Event Selection
print " Event selection "
process.evtSel = evtSel.clone()
process.evtSel.selection = config.selection()
process.evtSel.cutsfile  = config.cutsfile()

## -- EventSaverFlatNtuple
print " Event saver to flat ntuple "
process.EventSaverFlatNtuple = flat.clone()
process.EventSaverFlatNtuple.isMC = cms.bool(isMC)
process.EventSaverFlatNtuple.useTruth = cms.bool(config.useTruth())
process.EventSaverFlatNtuple.useJets  = cms.bool(config.useJets())
process.EventSaverFlatNtuple.useLargeRJets  = cms.bool(config.useLargeRJets())
process.EventSaverFlatNtuple.useNeutrinos   = cms.bool(config.useNeutrinos())
process.EventSaverFlatNtuple.useLeptons     = cms.bool(config.useLeptons())


## -- Setup CMAProducer
process.ana = cma.clone()
process.ana.isMC = cms.bool(isMC)
process.ana.useJets  = cms.bool(config.useJets())
process.ana.useLargeRJets  = cms.bool(config.useLargeRJets())
process.ana.useLeptons     = cms.bool(config.useLeptons())
process.ana.useNeutrinos   = cms.bool(config.useNeutrinos())
process.ana.buildNeutrinos = cms.bool(config.buildNeutrinos())
process.ana.kinematicReco  = cms.bool(config.kinematicReco())
process.ana.metadataFile   = cms.string(config.metadataFile())
process.ana.LUMI = cms.double(config.LUMI())
process.ana.useTruth = cms.bool(config.useTruth())

## -- Count events before & after selection
##    https://github.com/dmajumder/EventCounter
process.initial = eventCounter.clone( isData=(not isMC) )
process.final   = eventCounter.clone( isData=(not isMC) )


## PATH
print " Set the path "

process.p = cms.Path(
    process.initial*
    process.ana*
#    process.evtSel*
#    process.histogrammer*
    process.EventSaverFlatNtuple*
    process.final
)
# Add to process.p to inspect content:
#     process.content*


## OUTPUTMODULE
#print " Set the output "
#process.out = cms.OutputModule("PoolOutputModule",
#                               outputCommands = cms.untracked.vstring('drop *'))
#print " Set the schedule"
#process.schedule = cms.Schedule(process.p)
#process.outpath  = cms.EndPath(process.out)


print " Dump python "
open('dump.py','w').write(process.dumpPython())


process.TFileService = cms.Service("TFileService", fileName = cms.string("histo.root") )

#process.out = cms.OutputModule("PoolOutputModule",
#                               fileName = cms.untracked.string("output.root"),
#                               SelectEvents   = cms.untracked.PSet( SelectEvents = cms.vstring('p') ),
#                               outputCommands = cms.untracked.vstring('drop *')
#                               )
#process.outpath = cms.EndPath(process.out)

## THE END ##
