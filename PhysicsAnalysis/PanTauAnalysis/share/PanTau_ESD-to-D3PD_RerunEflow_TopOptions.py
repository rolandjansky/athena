# //////////////////////////////////////////////////////////////////
#
# Job Options to... start from a (D)ESD, run eflowRec to create a eflowRecContainer in the transient
#                   AOD then run PanTau on that AOD and have it D3PD-alized by the TauD3PDMaker. Wow!
#
#To be verified:
#   These JO can be submitted to the GRID by the corresponding script in the scripts folder
# //////////////////////////////////////////////////////////////////



from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.EvtMax=100
#athenaCommonFlags.FilesInput=["/tmp/limbach/data11_7TeV.00187501.physics_Muons.recon.DESD_ZMUMU.f396_m716_r2764_tid530016_00/DESD_ZMUMU.530016._000004.pool.root.1"]
athenaCommonFlags.FilesInput=['/afs/cern.ch/user/l/limbach/scratch0/SavedAODs/data11_7TeV.00191933.physics_JetTauEtmiss.merge.AOD.r3047_r3053_p673_tid619449_00/AOD.619449._000002.pool.root.1']


## ////////////////////////////////////////////////
## eflowRec Part
from RecExConfig.RecFlags import rec
rec.doTruth.set_Value_and_Lock( False )
rec.doCBNT.set_Value_and_Lock(False)
rec.doWriteESD.set_Value_and_Lock(False)
rec.doWriteAOD.set_Value_and_Lock(False) #explicitly disable for GRID jobs, use offline in testing for checks
rec.doAOD.set_Value_and_Lock(True)

# enable eflowRec
from RecExConfig.RecAlgsFlags import recAlgs
recAlgs.doEFlow.set_Value_and_Lock(True)

# change some calo flags (Mark's JO)
from CaloRec.CaloRecFlags import jobproperties as CaloRecJP
CaloRecJP.CaloRecFlags.Enabled.set_Value_and_Lock(True)
CaloRecJP.CaloRecFlags.doCaloCluster.set_Value_and_Lock(True)
CaloRecJP.CaloRecFlags.doCaloTopoCluster.set_Value_and_Lock(True)

# set eflowRec to tauMode
from eflowRec.eflowRecFlags import jobproperties as eflowRecJP
eflowRecJP.eflowRecFlags.runTauMode.set_Value_and_Lock(True)



## ////////////////////////////////////////////////
## PanTau / D3PD Part
from PanTauAnalysis.PanTauDPDFlags import panTauDPDFlags
panTauDPDFlags.PanTauPerfD3PDName = "ValidationTest_ESD-to-D3PD"
panTauDPDFlags.PanTauPerfD3PDSize = "medium"

from PanTauAnalysis.PanTauFlags import panTauFlags
panTauFlags.GetImpactParameterFeatures = True # test to see whether FPEs vanish
panTauFlags.GetJetFeatures = True # test to see whether FPEs vanish

from PanTauAnalysis.PanTauSeedBuilderAlg_Flags import PanTauSeedBuilderAlgFlags as PSB_Flags
PSB_Flags.DoRunTimeHists = False # create root file with runtime hists from SeedBuilder

from ParticleBuilderOptions.AODFlags import AODFlags
AODFlags.McEventKeyStr.set_Value_and_Lock('GEN_AOD')

from JetRec.JetRecFlags import jetFlags
jetFlags.inputTruthColl_RECO.set_Value_and_Lock('GEN_AOD')

from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
D3PDMakerFlags.TruthSGKey.set_Value_and_Lock('GEN_AOD')
D3PDMakerFlags.CellsSGKey.set_Value_and_Lock('AODCellContainer')
D3PDMakerFlags.JetSGKey.set_Value_and_Lock('AntiKt4LCTopoJets')




## ////////////////////////////////////////////////
## Execution part

#Main JobOptions
include("RecExCommon/RecExCommon_topOptions.py")

#Add PanTau
from AthenaCommon.AlgSequence import AlgSequence
include("PanTauAnalysis/TauIdAnalysis_topOptions.py")

# Add eflowContainers
# Build AOD objects and write them out to a POOL file
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
StreamAOD = AthenaPoolOutputStream( "StreamAOD" )
import AthenaCommon.PropertiesManip as manip
manip.appendItemsToList(StreamAOD,'ItemList','eflowObjectContainer#eflowObjects_tauMode')
manip.appendItemsToList(StreamAOD,'ItemList','CaloClusterContainer#eflowClusters_tauMode')

#adjusting outputlevels to get rid of over abundant warnings from MuonMET something
from JetMissingEtTagTools.JetMissingEtTagToolsConf import JetMissingEtTagTool as ConfiguredJetMissingEtTagTool
ConfiguredJetMissingEtTagTool.OutputLevel = ERROR

#Silence the TrigDecision tool
ToolSvc.TrigDecisionTool.OutputLevel = ERROR

#Silence the StoreGateSvc
StoreGateSvc = Service("StoreGateSvc")
StoreGateSvc.OutputLevel = ERROR


#Add the D3PD algorithm
dpdname = panTauDPDFlags.PanTauPerfD3PDName()
dpdsize = panTauDPDFlags.PanTauPerfD3PDSize().lower()

if dpdsize.find("small") is not -1:
    from TauD3PDMaker.tauPerfSmall import tauPerfSmall
    alg = tauPerfSmall (file=dpdname+"Small.root", level=10, tuplename="tau", streamName="TauSMALL")
    #alg.AddRequireAlgs(["ZMuMuTriggerFilter", "ZMuMuMuonFilter", "ZMuMuZMuMuFilter", "ZMuMuTauFilter"])

elif dpdsize.find("medium") is not -1:
    from TauD3PDMaker.tauPerfMedium import tauPerfMedium
    alg = tauPerfMedium (file=dpdname+".root", doCl = True, doTrk = False, doHad = True, level=10, tuplename='tau', streamName="TauMEDIUM")
    #alg.AddRequireAlgs(["ZMuMuTriggerFilter", "ZMuMuMuonFilter", "ZMuMuZMuMuFilter", "ZMuMuTauFilter"])




# end of file
