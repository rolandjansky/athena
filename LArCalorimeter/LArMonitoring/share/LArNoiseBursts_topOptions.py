import os

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

# etse a handle on the ServiceManager which holds all the services
from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)

# Event selector
import AthenaPoolCnvSvc.ReadAthenaPool

# Particle Properties
from PartPropSvc.PartPropSvcConf import PartPropSvc

from AthenaServices.AthenaServicesConf import AthenaEventLoopMgr
AthenaEventLoopMgr.OutputLevel = WARNING

from PyJobTransformsCore.runargs import RunArguments
runArgs = RunArguments()
runArgs.inputESDFile = [
"rfio:/castor/cern.ch/grid/atlas/tzero/prod1/perm/data11_7TeV/physics_CosmicCalo/00177929/data11_7TeV.00177929.physics_CosmicCalo.recon.ESD.x100/data11_7TeV.00177929.physics_CosmicCalo.recon.ESD.x100._lb0098._SFO-ALL._0001.1"
#"rfio:/castor/cern.ch/grid/atlas/tzero/prod1/perm/data11_7TeV/physics_CosmicCalo/00177964/data11_7TeV.00177964.physics_CosmicCalo.recon.ESD.x100/data11_7TeV.00177964.physics_CosmicCalo.recon.ESD.x100._lb0003._SFO-ALL._0001.1"
#"rfio:/castor/cern.ch/grid/atlas/tzero/prod1/perm/data11_7TeV/physics_CosmicCalo/00178109/data11_7TeV.00178109.physics_CosmicCalo.recon.ESD.x103/data11_7TeV.00178109.physics_CosmicCalo.recon.ESD.x103._lb0864._SFO-ALL._0001.1"
#"rfio:/castor/cern.ch/grid/atlas/tzero/prod1/perm/data10_7TeV/physics_CosmicCalo/0168143/data10_7TeV.00168143.physics_CosmicCalo.recon.ESD.f301/data10_7TeV.00168143.physics_CosmicCalo.recon.ESD.f301._lb0098._SFO-ALL._0001.1"
#"rfio:/castor/cern.ch/grid/atlas/tzero/prod1/perm/data10_7TeV/physics_CosmicCalo/0167627/data10_7TeV.00167627.physics_CosmicCalo.recon.ESD.x49/data10_7TeV.00167627.physics_CosmicCalo.recon.ESD.x49._lb0002._SFO-ALL._0001.1"
]

athenaCommonFlags.FilesInput.set_Value_and_Lock( runArgs.inputESDFile )

athenaCommonFlags.EvtMax.set_Value_and_Lock(-1)

from RecExConfig.RecFlags import rec
rec.readRDO.set_Value_and_Lock(False)
rec.readTAG.set_Value_and_Lock(False)
rec.readESD.set_Value_and_Lock(True)
rec.readAOD.set_Value_and_Lock(False)
rec.doWriteESD.set_Value_and_Lock(False)
rec.doWriteAOD.set_Value_and_Lock(False)
rec.doWriteTAG.set_Value_and_Lock(False)
rec.doHist.set_Value_and_Lock(False)
rec.doCBNT.set_Value_and_Lock(False)
rec.doAOD.set_Value_and_Lock(False)
rec.doTrigger.set_Value_and_Lock(False)
rec.doPerfMon.set_Value_and_Lock(False)
rec.doDPD.set_Value_and_Lock(False)

include ("RecExCommon/RecExCommon_topOptions.py")

from LArCellRec.LArNoisyROSummaryGetter import LArNoisyROSummaryGetter
LArNoisyROSummaryGetter.ignoreExistingDataObject = lambda x: True
LArNoisyROSummaryGetter()
#topSequence.LArNoisyROAlg.BadFEBCut = 2000
#topSequence.LArNoisyROAlg.SaturatedCellQualityCut = 111165535

from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)

#include("LArMonitoring/LArMonCommonHeader_jobOptions.py")

#include("LArMonitoring/LArMonVariables.py")
#include('LArMonitoring/LArMonManager.py')
#include('LArMonitoring/LArNoisyROMon_jobOptions.py')
#include('LArMonitoring/LArNoiseBurst_jobOptions.py')

#Necessary DLL's 
theApp.Dlls += [ "LArRawUtils","LArROD", "LArTools"]
theApp.Dlls += [ "LArByteStream"]

# set up trigger configuration service
from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
cfg =  TriggerConfigGetter("ReadPool")

## set up trigger decision tool
from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
ToolSvc += Trig__TrigDecisionTool( "TrigDecisionTool" )

# --- CaloNoiseTool configuration ---
from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
theCaloNoiseTool = CaloNoiseToolDefault()
ToolSvc += theCaloNoiseTool

# --- BunchCrossing Tool configuration ---
from TrigBunchCrossingTool.BunchCrossingTool import BunchCrossingTool
theBCTool = BunchCrossingTool()
ToolSvc += theBCTool

# --- SCTClusteringTool configuration ---
from SiClusterizationTool.SiClusterizationToolConf import InDet__SCT_ClusteringTool
InDetSCT_ClusteringTool = InDet__SCT_ClusteringTool()
ToolSvc += InDetSCT_ClusteringTool


include ( "ParticleBuilderOptions/ESD_PoolCnv_jobOptions.py" )
include ( "ParticleBuilderOptions/AOD_PoolCnv_jobOptions.py" )
include ( "ParticleBuilderOptions/McAOD_PoolCnv_jobOptions.py" )
include ( "EventAthenaPool/EventAthenaPool_joboptions.py" )

# Athena-Aware NTuple making Tools
CBNTAthenaAware = False

################################ LArTimeAlg###########################################
#David Cote: added include block to avoid duplication ERROR when TAG_COMM and DESDs are executed in the same job (both including this fragment).
include.block("LArCellRec/LArCollisionTime_jobOptions.py") 

from LArCellRec.LArCellRecConf import LArCollisionTimeAlg
topSequence += LArCollisionTimeAlg("LArCollisionTimeAlg")
topSequence.LArCollisionTimeAlg.NoiseTool = theCaloNoiseTool
 
from AthenaCommon.GlobalFlags import globalflags
if globalflags.DataSource()=='data' :
   topSequence.LArCollisionTimeAlg.isMC = False
else :
   topSequence.LArCollisionTimeAlg.isMC = True
 
# dont require anymore explicitely that the time iteration converges
topSequence.LArCollisionTimeAlg.cutIteration = False
topSequence.LArCollisionTimeAlg.OutputLevel = INFO
###################################################################

if not 'IS_SIMULATION' in inputFileSummary['evt_type']:
   
   from IOVDbSvc.CondDB import conddb
   
   if not conddb.folderRequested('/TDAQ/RunCtrl/DataTakingMode'):
      conddb.addFolder('TDAQ', '/TDAQ/RunCtrl/DataTakingMode')
   if not conddb.folderRequested('/TDAQ/OLC/LHC/FILLPARAMS'):
      conddb.addFolder('TDAQ', '/TDAQ/OLC/LHC/FILLPARAMS')
   if not conddb.folderRequested('/LHC/DCS/FILLSTATE'):
      conddb.addFolder('DCS_OFL', '/LHC/DCS/FILLSTATE')


from SiSpacePointTool.SiSpacePointToolConf import InDet__SiSpacePointMakerTool
InDetSiSpacePointMakerTool = InDet__SiSpacePointMakerTool(name = "InDetSiSpacePointMakerTool")
ToolSvc+=InDetSiSpacePointMakerTool

from SiSpacePointFormation.SiSpacePointFormationConf import InDet__SiTrackerSpacePointFinder
InDetSiTrackerSpacePointFinder = InDet__SiTrackerSpacePointFinder(name = "InDetSiTrackerSpacePointFinder")
topSequence += InDetSiTrackerSpacePointFinder


from LArMonitoring.LArMonitoringConf import LArNoiseBursts
topSequence += LArNoiseBursts( "LArNoiseBursts" )
topSequence.LArNoiseBursts.ICaloNoiseTool = theCaloNoiseTool
topSequence.LArNoiseBursts.BCTool = theBCTool
topSequence.LArNoiseBursts.SCTClusteringTool = InDetSCT_ClusteringTool
topSequence.LArNoiseBursts.SigmaCut = 3.0
topSequence.LArNoiseBursts.MBTSCellChargeThreshold = 44./222.
topSequence.LArNoiseBursts.NumberOfBunchesInFront = 30
topSequence.LArNoiseBursts.OutputLevel = DEBUG

from GaudiSvc.GaudiSvcConf import THistSvc
svcMgr += THistSvc()
svcMgr.THistSvc.Output = ["TTREE DATAFILE='LArNoiseBursts_ESD.root' OPT='RECREATE'"]

