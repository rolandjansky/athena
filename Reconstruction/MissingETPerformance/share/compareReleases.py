#####################################################
#Re-run the reconstruction,
#but change the MissingET output key names
#so that we have both the ones from the
#original reconstruction as well as
#the ones from the re-running
#then run some code  in MissingETPerformance
#to get the keys we want to compare
#and make some plots comparing
#also write both sets of keys to the AOD output file
#####################################################
SkipEvents = 0
EvtMax = 1
doCBNT=False
DetDescrVersion="ATLAS-GEO-02-01-00"
doESD=False
doMissingET=True
PoolESDInput=["/scratch/ayurkewi/mc08.105403.SU3_jimmy_susy.recon.ESD.e352_s462_r541/ESD.026108._00001.pool.root.1"
             ]

readESD = True
doWriteESD=False

include ("RecExCommon/RecExCommon_flags.py")
# Switch on TopoCluster reconstruction
from CaloRec.CaloRecFlags import jobproperties
jobproperties.CaloRecFlags.Enabled = True
jobproperties.CaloRecFlags.doCaloCluster = True
jobproperties.CaloRecFlags.doCaloTopoCluster = True
# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

#suffix to be added to all the storegate keys for the reprocessing
newrelease = '_15_0_0'

#these are the keys that are used within the MET algs
topSequence.METAlg.MuFinal.muonBOYTrack    += newrelease
topSequence.METAlg.MuFinal.muonBOYSpectro  += newrelease
topSequence.METAlg.Final.caloKey           += newrelease
topSequence.METAlg.Final.muonKey           += newrelease
topSequence.METAlg.Final.cryoKey           += newrelease
topSequence.METAlg.RefFinal.EleRefKey      += newrelease
topSequence.METAlg.RefFinal.GammaRefKey    += newrelease
topSequence.METAlg.RefFinal.MuoRefKey      += newrelease
topSequence.METAlg.RefFinal.TauRefKey      += newrelease
topSequence.METAlg.RefFinal.JetRefKey      += newrelease
topSequence.METAlg.RefFinal.CellOutKey     += newrelease
topSequence.METAlg.RefFinal.muonKey        += newrelease
topSequence.METAlg.RefFinal.cryoKey        += newrelease

#these are the keys that are going to be written out
topSequence.METAlg.CellTool.Calib.outKey                    += newrelease
topSequence.METAlg.CellTool.Calo.outKey                     += newrelease
topSequence.METAlg.CellTool.CaloAll.outKey                  += newrelease
topSequence.METAlg.CellTopo.outKey                          += newrelease
topSequence.METAlg.CellTopo.CalibTopo.outKey                += newrelease
topSequence.METAlg.CellTopo.CaloTopo.outKey                 += newrelease
topSequence.METAlg.CryoCone.outKey                          += newrelease
topSequence.METAlg.CryoTopo.outKey                          += newrelease
topSequence.METAlg.DeadMaterialAllCracks.outKey             += newrelease
topSequence.METAlg.DeadMaterialCrack1.outKey                += newrelease
topSequence.METAlg.DeadMaterialCrack2.outKey                += newrelease
topSequence.METAlg.DeadMaterialCryo.outKey                  += newrelease
topSequence.METAlg.Final.outKey                             += newrelease
topSequence.METAlg.LocHadTopo.outKey                        += newrelease
topSequence.METAlg.LocHadTopo.LocHadCalibTopo.outKey        += newrelease
topSequence.METAlg.MuFinal.outKey                           += newrelease
topSequence.METAlg.Muon.outKey                              += newrelease
topSequence.METAlg.MuonBoySpectro.outKey                    += newrelease
topSequence.METAlg.MuonBoyTrack.outKey                      += newrelease
topSequence.METAlg.RefFinal.outKey                          += newrelease
topSequence.METAlg.Truth.outKey                             += newrelease
topSequence.METAlg.Truth_PileUp.outKey                      += newrelease
topSequence.METRefAlg.METRefEle.MissingETOutKey             += newrelease
topSequence.METRefAlg.METRefGamma.MissingETOutKey           += newrelease
topSequence.METRefAlg.METRefTau.MissingETOutKey             += newrelease
topSequence.METRefAlg.METRefJet.MissingETOutKey             += newrelease
#topSequence.METRefAlg.METRefMuonTrack.MissingETOutKey       += newrelease
#topSequence.METRefAlg.METRefMuonSpectro.MissingETOutKey     += newrelease
topSequence.METRefAlg.METRefCluster.MissingETOutKey         += newrelease

#here is where we specify the keys that are going to be written out by adding them to the list
StreamAOD.ItemList += ["MissingET#" + topSequence.METAlg.CellTool.Calib.outKey]
StreamAOD.ItemList += ["MissingET#" + topSequence.METAlg.CellTool.Calo.outKey]
StreamAOD.ItemList += ["MissingET#" + topSequence.METAlg.CellTool.CaloAll.outKey]
StreamAOD.ItemList += ["MissingET#" + topSequence.METAlg.CellTopo.outKey]
StreamAOD.ItemList += ["MissingET#" + topSequence.METAlg.CellTopo.CalibTopo.outKey]
StreamAOD.ItemList += ["MissingET#" + topSequence.METAlg.CellTopo.CaloTopo.outKey]
StreamAOD.ItemList += ["MissingET#" + topSequence.METAlg.CryoCone.outKey]
StreamAOD.ItemList += ["MissingET#" + topSequence.METAlg.CryoTopo.outKey]
StreamAOD.ItemList += ["MissingET#" + topSequence.METAlg.DeadMaterialAllCracks.outKey]
StreamAOD.ItemList += ["MissingET#" + topSequence.METAlg.DeadMaterialCrack1.outKey]
StreamAOD.ItemList += ["MissingET#" + topSequence.METAlg.DeadMaterialCrack2.outKey]
StreamAOD.ItemList += ["MissingET#" + topSequence.METAlg.DeadMaterialCryo.outKey]
StreamAOD.ItemList += ["MissingET#" + topSequence.METAlg.Final.outKey]
StreamAOD.ItemList += ["MissingET#" + topSequence.METAlg.LocHadTopo.outKey]
StreamAOD.ItemList += ["MissingET#" + topSequence.METAlg.LocHadTopo.LocHadCalibTopo.outKey]
StreamAOD.ItemList += ["MissingET#" + topSequence.METAlg.MuFinal.outKey]
StreamAOD.ItemList += ["MissingET#" + topSequence.METAlg.Muon.outKey]
StreamAOD.ItemList += ["MissingET#" + topSequence.METAlg.MuonBoySpectro.outKey]
StreamAOD.ItemList += ["MissingET#" + topSequence.METAlg.MuonBoyTrack.outKey]
StreamAOD.ItemList += ["MissingET#" + topSequence.METAlg.RefFinal.outKey]
StreamAOD.ItemList += ["MissingET#" + topSequence.METAlg.Truth.outKey]
StreamAOD.ItemList += ["MissingET#" + topSequence.METAlg.Truth_PileUp.outKey]
StreamAOD.ItemList += ["MissingET#" + topSequence.METRefAlg.METRefEle.MissingETOutKey]
StreamAOD.ItemList += ["MissingET#" + topSequence.METRefAlg.METRefGamma.MissingETOutKey]
StreamAOD.ItemList += ["MissingET#" + topSequence.METRefAlg.METRefTau.MissingETOutKey]
StreamAOD.ItemList += ["MissingET#" + topSequence.METRefAlg.METRefJet.MissingETOutKey]
#StreamAOD.ItemList += ["MissingET#" + topSequence.METRefAlg.METRefMuonTrack.MissingETOutKey]
#StreamAOD.ItemList += ["MissingET#" + topSequence.METRefAlg.METRefMuonSpectro.MissingETOutKey]
StreamAOD.ItemList += ["MissingET#" + topSequence.METRefAlg.METRefCluster.MissingETOutKey]

from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.AppMgr import ServiceMgr
import AthenaPoolCnvSvc.ReadAthenaPool
from PartPropSvc.PartPropSvcConf import PartPropSvc

# the POOL converters
include( "ParticleBuilderOptions/ESD_PoolCnv_jobOptions.py" )
include( "ParticleBuilderOptions/AOD_PoolCnv_jobOptions.py")
include( "ParticleBuilderOptions/McAOD_PoolCnv_jobOptions.py")
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )

include ( "MissingETPerformance/MissingETData_jobOptions.py" )
include ( "MissingETPerformance/ContainerComparatorTool_jobOptions.py" )
MissingETData.IgnoreMissingContainers = True

# Athena-Aware NTuple making Tools
CBNTAthenaAware = True
include ("CBNT_Athena/CBNT_AthenaAware_jobOptions.py")
include ("CBNT_Athena/CBNT_EventInfo_jobOptions.py")

from MissingETPerformance.MissingETPerformanceConf import MissingETPerformance
topSequence.CBNT_AthenaAware += MissingETPerformance() 
MissingETPerformance = MissingETPerformance()

#only want ContainerComparatorTool on
MissingETPerformance.doContainerComparatorTool          = True
MissingETPerformance.FilterOnTrigger                    = False
MissingETPerformance.doBasicPlotsTool                   = False
MissingETPerformance.doResolutionTool                   = False
MissingETPerformance.doLinearityTool                    = False
MissingETPerformance.doEtaRingsTool                     = False
MissingETPerformance.doTrigMissingETTool                = False
MissingETPerformance.doTrigVsOfflineMissingETTool       = False
MissingETPerformance.doFakeMissingETTool                = False
MissingETPerformance.doMuonTool                         = False

##########################################
# setup TTree registration Service
# save ROOT histograms and Tuple
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output = [ "AANT DATAFILE='METPerformance.root' OPT='RECREATE'" ]
from AnalysisTools.AthAnalysisToolsConf import AANTupleStream
topSequence += AANTupleStream()
AANTupleStream = AANTupleStream()
AANTupleStream.OutputName = 'METPerformance.root'
AANTupleStream.WriteInputDataHeader = True

AthenaEventLoopMgr = Service ( "AthenaEventLoopMgr")
try:
    AthenaEventLoopMgr.EventPrintoutInterval = 1
except Exception:
    print 'EventPrintoutInterval exception, defaults to 1'
