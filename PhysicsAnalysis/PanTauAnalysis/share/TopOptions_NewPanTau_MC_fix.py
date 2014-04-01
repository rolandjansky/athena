#////////////////////////////////////////////////////////////////////
#
#  topOptions for PanTau:
#       Input : An ESD that contains the eflowRec container eflowObjects_tauMode and the tauRecContainer
#       Output: A TauD3PD that contains an enhanced tau block:
#               - tau_pantau block:
#                   These variables are available in official AOD
#
#               - tau_pantauFeature block:
#                   These variables are not available in the official AOD
#                   However, these are all the variables from the
#                   PanTau::TauSeed that are available
#                   These variables should be used for performance studies
#
#   This jobOption file will work when submitting GRID jobs!
#   Simply use the script 
#           submit-pathena-job_AOD_to_PanTauD3PD.sh
#   to submit the jobs for a single sample, or use
#           Launch_GridJobs.sh
#   to submit jobs for all desired samples
#
#////////////////////////////////////////////////////////////////////


#//////////////////////////////////
#   Input Files
#//////////////////////////////////

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
if athenaCommonFlags.FilesInput.isDefault(): # check if not already set upstream
#    athenaCommonFlags.FilesInput=['/afs/cern.ch/work/b/bwinter/septemberProduction/september_production_2ESD_fromRDO/ESD2ESD/OUT.ESD.pool.root'] #septemberESD
    athenaCommonFlags.FilesInput=['/afs/cern.ch/user/b/bwinter/november_production_2ESD/ESD2ESD/myESD.pool_november.root'] #novemberESD
    
athenaCommonFlags.EvtMax=-1  # number of events to process run on all file

## turn off CellBased for old data file to check duplicate EFO bug
#from tauRec.tauRecFlags import jobproperties as taujp
#taujp.tauRecFlags.doBonnPi0.set_Value_and_Lock(False)

from PanTauAnalysis.PanTauFlags import panTauFlags
panTauFlags.ExtractAllFeatures.set_Value_and_Lock(True)

###########################################################

#preExec MC
#...add truth info in D3PD
from RecExConfig.RecFlags import rec
rec.doTruth.set_Value_and_Lock( True )


# ==== PreExec
rec.Commissioning.set_Value_and_Lock(True)
rec.AutoConfiguration = [ "everything" ]

from AthenaCommon.JobProperties import jobproperties
jobproperties.Beam.energy.set_Value_and_Lock(4000*Units.GeV)
jobproperties.Beam.bunchSpacing.set_Value_and_Lock(50)
jobproperties.Beam.numberOfCollisions.set_Value_and_Lock(20.0)

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.AODEDMSet="AODSLIM"

from MuonRecExample.MuonRecFlags import muonRecFlags
muonRecFlags.writeSDOs=True

# ==== end PreExec
"""
############################################################
# preExec data
# PreExec
from RecExConfig.RecFlags import rec
#rec.doTruth.set_Value_and_Lock( True )
#rec.Commissioning.set_Value_and_Lock(True)
rec.AutoConfiguration = [ "everything" ]
from AthenaCommon.JobProperties import jobproperties
#jobproperties.Beam.energy.set_Value_and_Lock(4000*Units.GeV)
#jobproperties.Beam.bunchSpacing.set_Value_and_Lock(50)
#jobproperties.Beam.numberOfCollisions.set_Value_and_Lock(20.0)
from TriggerJobOpts.TriggerFlags import TriggerFlags
#TriggerFlags.AODEDMSet="AODSLIM"
from MuonRecExample.MuonRecFlags import muonRecFlags
#muonRecFlags.writeSDOs=True

############################################################
"""

#//////////////////////////////////
#   Main Job Options
#//////////////////////////////////

# !!!                                            !!! #
# !!! You must not modify flags below this point !!! #
# !!!                                            !!! #

#emulate the old PanTau to check if everything works
from PanTauAnalysis.Config_PanTau import config_PanTau
#config_PanTau.Names_InputAlgorithms = ["eflowRec"]

rec.readAOD      = False
rec.readESD      = True
rec.readRDO      = False
rec.doCBNT       = False
rec.doWriteESD   = False
rec.doWriteTAG   = False
rec.doPerfMon    = False
rec.doHist       = False

rec.doAOD.set_Value_and_Lock(False)
rec.doWriteAOD.set_Value_and_Lock(False)

# commented out in AOD->D3PD
from AthenaServices.AthenaServicesConf import AthenaEventLoopMgr
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr.MessageSvc.infoLimit   = 50
svcMgr += AthenaEventLoopMgr()
svcMgr.AthenaEventLoopMgr.EventPrintoutInterval = 50


from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#from tauRec.tauRecFlags import jobproperties
#from JetRec.JetRecFlags import jobproperties
#jobproperties.tauRecFlags.doTauRec = True
#jobproperties.JetRecFlags.Enabled = True
#rec.doTau = True
#fastPoolMerge=False

#from tauRec.TauRecAODBuilder import TauRecAODProcessor
#TauRecAODProcessor(inAODmode=True)

#from TauDiscriminant.TauDiscriGetter import TauDiscriGetter
#TauDiscriGetter()


include( "RecExCommon/RecExCommon_topOptions.py" )

#include pantau
include("PanTauAnalysis/JobOptions_Main_PanTau.py")


if rec.doTruth():
    from TauD3PDMaker.TauD3PDMakerConf import TruthTausToSG
    if rec.readESD():
        TruthTausToSG.truth_container="INav4MomTruthEvent"
    print("climbach: updated TruthTausToSG to be INav4MomTruthEvent")
    topSequence += TruthTausToSG()



## Rerun Jet moments for jet area variables
from JetRec.JetRecFlags import jetFlags
jetFlags.jetPerformanceJob = True

from JetMomentTools.JetMomentsConfigHelpers import recommendedAreaAndJVFMoments
jetmomAlg = recommendedAreaAndJVFMoments('AntiKt4LCTopoJets')
                           
## Rerun B-tagging for MV3
#preExec='from BTagging.BTaggingFlags import BTaggingFlags',,'BTaggingFlags.CalibrationTag=\"BTagCalibALL-07-05\"'
from BTagging.BTaggingFlags import BTaggingFlags
BTaggingFlags.CalibrationTag="BTagCalibALL-07-05" # up to now it is not linked to the global flag
BTaggingFlags.Jets=['AntiKt4LCTopo']
BTaggingFlags.CalibrationChannelAliases+=[ 'AntiKt4LCTopo->AntiKt4TopoEM' ]
BTaggingFlags.Active=True
BTaggingFlags.JetFitterCharm=True
BTaggingFlags.MV3_bVSu=False
BTaggingFlags.MV3_bVSc=False
BTaggingFlags.MV3_cVSu=False

include( "BTagging/BTagging_LoadTools.py" )
include( "BTagging/BTagging_jobOptions.py" )

from JetTagD3PDMaker.JetTagD3PDMakerFlags import JetTagD3PDFlags
JetTagD3PDFlags.Taggers+=["JetFitterCharm"]#,"MV3_bVSu","MV3_bVSc","MV3_cVSu"]
JetTagD3PDFlags.JetFitterCharmTagInfo=True

#re-run MET
include('TauD3PDMaker/metAlgs.py')
from RecExConfig.RecAlgsFlags import recAlgs
recAlgs.doMissingET.set_Value_and_Lock(True)


#//////////////////////////////////
#   D3PD Output
#//////////////////////////////////

from TauD3PDMaker.tauPerf import tauPerf
alg = tauPerf ( level = 10,
                   file="TauD3PD_FullPanTauInfo.root",
                   doPhys = True,
                   doCl = True,
                   doCell = False,
                   doPi0BonnDevVars = True,
                   doTrk = True,
                   doHad = True,
                   doOtherTrk = True,
                   tuplename = 'tau',
                   seq = topSequence,
                   D3PDSvc = 'D3PD::RootD3PDSvc',
                   doMETSkimVariables = False,
                   streamName = "TauPERF")



#//////////////////////////////////
#   Silence over abundant messages
#//////////////////////////////////
############################
#Silence Trig Navigation Warnings
from AthenaCommon.AppMgr import ToolSvc
if not hasattr( ToolSvc, "TrigDecisionTool" ):
    from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
    ToolSvc += Trig__TrigDecisionTool( "TrigDecisionTool" )
ToolSvc.TrigDecisionTool.Navigation.OutputLevel = ERROR
ToolSvc.TrigDecisionTool.OutputLevel = ERROR




#############################
StoreGateSvc.OutputLevel = FATAL


#end of file

