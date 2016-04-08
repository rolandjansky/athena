# ------------------------------------------------------------
# RunTriggerJpsiAnalysis.py
# ------------------------------------------------------------
#
# Daniel Scheirich (daniel.scheirich@cern.ch)
#
# THIS SCRIPT MUST BE RUN FROM THE BPhysAnalysisMasterAuto.py
# ------------------------------------------------------------

################################################

#-------------------------------------------------------------
# User analysis steering algorithm
#-------------------------------------------------------------

from TrigBphysValidation.TrigBphysValidationConf import TriggerJpsiAnalysis
from AthenaCommon.AlgSequence import AlgSequence, AthSequencer
job = AlgSequence()
job += AthSequencer("ModSequence1")

#-------------------------------------------------------------
# Good Run List (GRL) selector
#-------------------------------------------------------------
#
# !!!!!!!!!!!!!! UNCOMMENT THIS FOR REAL DATA !!!!!!!!!!!!
#

# Configure the goodrunslist selector tool

from GoodRunsLists.GoodRunsListsConf import *
goodRunsListSelectorTool = GoodRunsListSelectorTool()
goodRunsListSelectorTool.GoodRunsListVec = [ 'GRL_7TeV_init.xml' ]

ToolSvc += goodRunsListSelectorTool

## GRL selector

from GoodRunsListsUser.GoodRunsListsUserConf import *
job.ModSequence1 += GRLTriggerSelectorAlg( name = 'GRLTriggerAlg1' )

job.ModSequence1.GRLTriggerAlg1.GoodRunsListArray = ['MyLBCollection']
# NOTE - variable above MUST match name inside GRL xml file


# ------------------------
# SET UP INNER DETECTOR TOOLS
# ------------------------
# IMPORTANT: the cuts on the tracks are coded in the configureServices.py tool
# If you wish to modify these cuts, you need to modify configureServices.py

# NOTE TriggerJpsiAnalysis doesn't use default configuration of track selector tool. Special
#      configuration of the tool is defined in selectCandidates.py

include("configureServices.py")


# ----------------------------------
# JpsiFinder algorithms to reconstruct
# J/psi, Lambda0, anti-Lambda0, and K_s
# ----------------------------------

#this J.O. fragment will create instance of JpsiAlg class
#that must be added to the algSequence before TriggerJpsiAnalysis

include( "selectJpsiCandidates.py" )

job.ModSequence1 += jpsiAlg


# ----------------------------------
# TriggerJpsiAnalysis algorithm:
# ----------------------------------

# create instance of TriggerJpsiAnalysis

triggerJpsiAnalysis = TriggerJpsiAnalysis( name = "TriggerJpsiAnalysis" )
job.ModSequence1 += triggerJpsiAnalysis


# ----------------------------------
# TriggerJpsiAnalysis requirements:
# ----------------------------------

# output

triggerJpsiAnalysis.OutputLevel               = DEBUG

triggerJpsiAnalysis.OutputFileName            = "bphys.root"


# Trigger chains
triggerJpsiAnalysis.TrigDiMuonChains          = [ ]

## Topological
triggerJpsiAnalysis.TrigDiMuonChains         += [ "L2_2mu4" ]
triggerJpsiAnalysis.TrigDiMuonChains         += [ "L2_2mu6" ]
triggerJpsiAnalysis.TrigDiMuonChains         += [ "L2_2mu4_DiMu" ]
triggerJpsiAnalysis.TrigDiMuonChains         += [ "L2_2mu6_DiMu" ]
triggerJpsiAnalysis.TrigDiMuonChains         += [ "L2_2mu4_DiMu_SiTrk" ]
triggerJpsiAnalysis.TrigDiMuonChains         += [ "L2_2mu4_DiMu_noVtx" ]
triggerJpsiAnalysis.TrigDiMuonChains         += [ "L2_2mu4_DiMu_noVtx_noOS" ]
triggerJpsiAnalysis.TrigDiMuonChains         += [ "L2_2mu4_Jpsimumu" ]
triggerJpsiAnalysis.TrigDiMuonChains         += [ "L2_2mu4_Upsimumu" ]
triggerJpsiAnalysis.TrigDiMuonChains         += [ "L2_2mu6_Jpsimumu" ]
triggerJpsiAnalysis.TrigDiMuonChains         += [ "L2_2mu6_Upsimumu" ]
triggerJpsiAnalysis.TrigDiMuonChains         += [ "EF_2mu4" ]
triggerJpsiAnalysis.TrigDiMuonChains         += [ "EF_2mu6" ]
triggerJpsiAnalysis.TrigDiMuonChains         += [ "EF_2mu4_DiMu" ]
triggerJpsiAnalysis.TrigDiMuonChains         += [ "EF_2mu6_DiMu" ]
triggerJpsiAnalysis.TrigDiMuonChains         += [ "EF_2mu4_DiMu_SiTrk" ]
triggerJpsiAnalysis.TrigDiMuonChains         += [ "EF_2mu4_DiMu_noVtx" ]
triggerJpsiAnalysis.TrigDiMuonChains         += [ "EF_2mu4_DiMu_noVtx_noOS" ]
triggerJpsiAnalysis.TrigDiMuonChains         += [ "EF_2mu4_Jpsimumu" ]
triggerJpsiAnalysis.TrigDiMuonChains         += [ "EF_2mu4_Upsimumu" ]
triggerJpsiAnalysis.TrigDiMuonChains         += [ "EF_2mu6_Jpsimumu" ]
triggerJpsiAnalysis.TrigDiMuonChains         += [ "EF_2mu6_Upsimumu" ]

## DiMu
triggerJpsiAnalysis.TrigDiMuonChains         += [ "L2_MU4_DiMu" ]
triggerJpsiAnalysis.TrigDiMuonChains         += [ "L2_MU4_DiMu_FS" ]
triggerJpsiAnalysis.TrigDiMuonChains         += [ "L2_MU4_DiMu_SiTrk_FS" ]
triggerJpsiAnalysis.TrigDiMuonChains         += [ "L2_mu4"]
triggerJpsiAnalysis.TrigDiMuonChains         += [ "L2_mu4_DiMu" ]
triggerJpsiAnalysis.TrigDiMuonChains         += [ "L2_mu6_DiMu" ]
triggerJpsiAnalysis.TrigDiMuonChains         += [ "L2_mu4_DiMu_MG" ]
triggerJpsiAnalysis.TrigDiMuonChains         += [ "L2_mu4_DiMu_SiTrk" ]
triggerJpsiAnalysis.TrigDiMuonChains         += [ "L2_mu4_DiMu_FS" ]
triggerJpsiAnalysis.TrigDiMuonChains         += [ "L2_mu4_DiMu_MG_FS" ]
triggerJpsiAnalysis.TrigDiMuonChains         += [ "L2_mu4_DiMu_SiTrk_FS" ]
triggerJpsiAnalysis.TrigDiMuonChains         += [ "L2_mu4_Jpsimumu"]
triggerJpsiAnalysis.TrigDiMuonChains         += [ "L2_mu6_Jpsimumu"]
triggerJpsiAnalysis.TrigDiMuonChains         += [ "L2_mu4_Jpsimumu_FS"]
triggerJpsiAnalysis.TrigDiMuonChains         += [ "L2_mu4_Upsimumu_FS" ]
triggerJpsiAnalysis.TrigDiMuonChains         += [ "L2_mu6_Upsimumu_FS" ]
triggerJpsiAnalysis.TrigDiMuonChains         += [ "EF_MU4_DiMu" ]
triggerJpsiAnalysis.TrigDiMuonChains         += [ "EF_MU4_DiMu_FS" ]
triggerJpsiAnalysis.TrigDiMuonChains         += [ "EF_MU4_DiMu_SiTrk_FS" ]
triggerJpsiAnalysis.TrigDiMuonChains         += [ "EF_mu4" ]
triggerJpsiAnalysis.TrigDiMuonChains         += [ "EF_mu4_DiMu" ]
triggerJpsiAnalysis.TrigDiMuonChains         += [ "EF_mu6_DiMu" ]
triggerJpsiAnalysis.TrigDiMuonChains         += [ "EF_mu4_DiMu_MG" ]
triggerJpsiAnalysis.TrigDiMuonChains         += [ "EF_mu4_DiMu_SiTrk" ]
triggerJpsiAnalysis.TrigDiMuonChains         += [ "EF_mu4_DiMu_FS" ]
triggerJpsiAnalysis.TrigDiMuonChains         += [ "EF_mu4_DiMu_MG_FS" ]
triggerJpsiAnalysis.TrigDiMuonChains         += [ "EF_mu4_DiMu_SiTrk_FS" ]
triggerJpsiAnalysis.TrigDiMuonChains         += [ "EF_mu4_Jpsimumu"]
triggerJpsiAnalysis.TrigDiMuonChains         += [ "EF_mu6_Jpsimumu"]
triggerJpsiAnalysis.TrigDiMuonChains         += [ "EF_mu4_Jpsimumu_FS"]
triggerJpsiAnalysis.TrigDiMuonChains         += [ "EF_mu4_Upsimumu_FS" ]
triggerJpsiAnalysis.TrigDiMuonChains         += [ "EF_mu6_Upsimumu_FS" ]

## MuTrk
#triggerJpsiAnalysis.TrigDiMuonChains         += [ "L2_MU4_Trk_Jpsi" ]
#triggerJpsiAnalysis.TrigDiMuonChains         += [ "L2_MU4_Trk_Jpsi_FS" ]
#triggerJpsiAnalysis.TrigDiMuonChains         += [ "L2_MU4_Trk_Jpsi_loose" ]
#triggerJpsiAnalysis.TrigDiMuonChains         += [ "L2_MU4_Trk_Jpsi_loose_FS" ]
#triggerJpsiAnalysis.TrigDiMuonChains         += [ "EF_MU4_Trk_Jpsi" ]
#triggerJpsiAnalysis.TrigDiMuonChains         += [ "EF_MU4_Trk_Jpsi_FS" ]
#triggerJpsiAnalysis.TrigDiMuonChains         += [ "EF_MU4_Trk_Jpsi_loose" ]
#triggerJpsiAnalysis.TrigDiMuonChains         += [ "EF_MU4_Trk_Jpsi_loose_FS" ]

## some MC chains
#triggerJpsiAnalysis.TrigDiMuonChains         += [ "L2_2MU4_DiMu" ]
#triggerJpsiAnalysis.TrigDiMuonChains         += [ "L2_2MU4_Jpsimumu" ]
#triggerJpsiAnalysis.TrigDiMuonChains         += [ "L2_2MU4_Upsimumu" ]
#triggerJpsiAnalysis.TrigDiMuonChains         += [ "L2_MU4_Jpsimumu_FS" ]
#triggerJpsiAnalysis.TrigDiMuonChains         += [ "L2_MU4_Upsimumu_FS" ]

#triggerJpsiAnalysis.TrigDiMuonChains         += [ "EF_2MU4_DiMu" ]
#triggerJpsiAnalysis.TrigDiMuonChains         += [ "EF_2MU4_Jpsimumu" ]
#triggerJpsiAnalysis.TrigDiMuonChains         += [ "EF_2MU4_Upsimumu" ]
#triggerJpsiAnalysis.TrigDiMuonChains         += [ "EF_MU4_Jpsimumu_FS" ]
#triggerJpsiAnalysis.TrigDiMuonChains         += [ "EF_MU4_Upsimumu_FS" ]

## cuts

## this is part of the baseline Muon Combined Performance group cuts - James Catmore 10/4/2010
## NOTE not used at the moment !!!
triggerJpsiAnalysis.MuonEnergyCut             = 3000.           # minimal muon energy
triggerJpsiAnalysis.SegmentDeltaEtaCut        = 3.              # MuTagIMO segmens cuts
triggerJpsiAnalysis.SegmentDeltaPhiCut        = 3.              # MuTagIMO segmens cuts


## Flags

triggerJpsiAnalysis.DumpAllTracks             = False           # dumps all tracks to the ntuple. Makes output file big
triggerJpsiAnalysis.SaveAllEvents             = False           # saves trigger objects for all events. Makes ntuple BIG

## cuts

triggerJpsiAnalysis.L1Trigger                 = "L1_.*"         # level-1 trigger. To disable use "L1_.*"
triggerJpsiAnalysis.OfflineTrigDR             = 0.1             # offline -- trigger track matching cut


## switch on/off different parts of the algorithm

triggerJpsiAnalysis.DoTrigger                 = True            # save trigger decision
triggerJpsiAnalysis.DoMCTruth                 = False           # switch entire Truth analysis on/off

## Container names

triggerJpsiAnalysis.TruthCollName             = "TrackParticleTruthCollection"
triggerJpsiAnalysis.McEventCollectionName     = "GEN_AOD"
triggerJpsiAnalysis.VxContainerName           = "VxPrimaryCandidate"

triggerJpsiAnalysis.TrkCollName               = jpsiFinder.TrackParticleCollection
triggerJpsiAnalysis.MuonCollName              = jpsiFinder.muonCollectionKey

triggerJpsiAnalysis.JpsiCandidates            = jpsiAlg.JpsiCandidatesOutputName

##--------------------------------------------------------------
## TruthFinder tool
##--------------------------------------------------------------

from BPhysAnalysisTools.BPhysAnalysisToolsConf import BPhys__TruthFinder

jpsiTruthFinder = BPhys__TruthFinder( name = "jpsiTruthFinder" )
ToolSvc += jpsiTruthFinder
print      jpsiTruthFinder

## pass to the algorithm
triggerJpsiAnalysis.JpsiTruthFinder        = jpsiTruthFinder

##--------------------------------------------------------------
## configure TriggerDecisionTool
##--------------------------------------------------------------

include("trigger.py")

