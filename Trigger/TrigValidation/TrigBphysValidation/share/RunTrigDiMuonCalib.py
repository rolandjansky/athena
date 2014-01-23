## ------------------------------------------------------------
## RunTrigDiMuonCalib.py
## ------------------------------------------------------------
##
## Daniel Scheirich (daniel.scheirich@cern.ch)
##
## THIS SCRIPT MUST BE RUN FROM THE BPhysAnalysisMasterAuto.py
## ------------------------------------------------------------

#################################################

##-------------------------------------------------------------
## User analysis steering algorithm
##-------------------------------------------------------------

from TrigBphysValidation.TrigBphysValidationConf import TrigDiMuonCalib
from AthenaCommon.AlgSequence import AlgSequence, AthSequencer
job = AlgSequence()
job += AthSequencer("ModSequence1")

##-------------------------------------------------------------
## Good Run List (GRL) selector
##-------------------------------------------------------------
##
## !!!!!!!!!!!!!! UNCOMMENT THIS FOR REAL DATA !!!!!!!!!!!!
##

## Configure the goodrunslist selector tool

from GoodRunsLists.GoodRunsListsConf import *
goodRunsListSelectorTool = GoodRunsListSelectorTool()
goodRunsListSelectorTool.GoodRunsListVec = [ 'MyLBCollection.xml' ]

ToolSvc += goodRunsListSelectorTool

## GRL selector

from GoodRunsListsUser.GoodRunsListsUserConf import *
job.ModSequence1 += GRLTriggerSelectorAlg( name = 'GRLTriggerAlg1' )

job.ModSequence1.GRLTriggerAlg1.GoodRunsListArray = ['MyLBCollection']
## NOTE - variable above MUST match name inside GRL xml file


## ------------------------
## SET UP INNER DETECTOR TOOLS
## ------------------------
## IMPORTANT: the cuts on the tracks are coded in the configureServices.py tool
## If you wish to modify these cuts, you need to modify configureServices.py

## NOTE TriggerJpsiAnalysis doesn't use default configuration of track selector tool. Special
##      configuration of the tool is defined in selectCandidates.py

include("configureServices.py")


## ----------------------------------
## JpsiFinder algorithms to reconstruct J/psi
## ----------------------------------

# this J.O. fragment will create instance of JpsiAlg class
# that must be added to the algSequence before TriggerJpsiAnalysis

include( "selectJpsiCandidates.py" )

job.ModSequence1 += jpsiAlg


## ----------------------------------
## TrigDiMuonCalib algorithm:
## ----------------------------------

## create instance of TrigDiMuonCalib

trigDiMuonCalib = TrigDiMuonCalib( name = "TrigDiMuonCalib" )
job.ModSequence1 += trigDiMuonCalib


## ----------------------------------
## TrigDiMuonCalib requirements:
## ----------------------------------

## output

trigDiMuonCalib.OutputLevel               = DEBUG

## flags

trigDiMuonCalib.DumpAllTracks             = False           # dumps all tracks to the ntuple. Makes output file big
trigDiMuonCalib.DoTrigger                 = True            # save trigger decision
trigDiMuonCalib.DoMCTruth                 = False           # switch entire Truth analysis on/off

## cuts

trigDiMuonCalib.L1Trigger                 = "L1_.*"         # level-1 trigger. To disable use "L1_.*"
trigDiMuonCalib.OfflineTrigDR             = 0.1             # offline -- trigger track matching cut

## container names
trigDiMuonCalib.VxContainerName           = "VxPrimaryCandidate"
trigDiMuonCalib.OutputFileName            = "bphys.root"
trigDiMuonCalib.TrkCollName               = "TrackParticleCandidate"
trigDiMuonCalib.TruthCollName             = "TrackParticleTruthCollection"
trigDiMuonCalib.McEventCollectionName     = "GEN_AOD"
trigDiMuonCalib.JpsiCandidates            = jpsiAlg.JpsiCandidatesOutputName

trigDiMuonCalib.MuonCollectionNames       = [ "MuidMuonCollection", "StacoMuonCollection" ]

## TrigDiMuon chains to be analyzed
trigDiMuonCalib.TrigDiMuonChains          = [ ]             # list of TrigDiMuon and muon+trk chains

trigDiMuonCalib.TrigDiMuonChains         += [ "L2_MU4_DiMu" ]
trigDiMuonCalib.TrigDiMuonChains         += [ "L2_MU4_DiMu_FS" ]
trigDiMuonCalib.TrigDiMuonChains         += [ "L2_MU4_DiMu_SiTrk_FS" ]

##--------------------------------------------------------------
## TruthFinder tool
##--------------------------------------------------------------

from BPhysAnalysisTools.BPhysAnalysisToolsConf import BPhys__TruthFinder

jpsiTruthFinder = BPhys__TruthFinder( name = "jpsiTruthFinder" )
ToolSvc += jpsiTruthFinder
print      jpsiTruthFinder

## pass to the algorithm
trigDiMuonCalib.JpsiTruthFinder        = jpsiTruthFinder

##--------------------------------------------------------------
## configure TriggerDecisionTool
##--------------------------------------------------------------

include("trigger.py")

