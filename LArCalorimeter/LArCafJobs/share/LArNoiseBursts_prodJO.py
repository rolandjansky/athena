# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)

# Event selector
import AthenaPoolCnvSvc.ReadAthenaPool

from AthenaServices.AthenaServicesConf import AthenaEventLoopMgr
AthenaEventLoopMgr.OutputLevel = WARNING

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

#Necessary DLL's 
theApp.Dlls += [ "LArRawUtils","LArROD", "LArTools"]
theApp.Dlls += [ "LArByteStream"]

# set up trigger configuration service
from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
cfg =  TriggerConfigGetter("ReadPool")

## set up trigger decision tool
from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
ToolSvc += Trig__TrigDecisionTool( "TrigDecisionTool" )

# --- CaloNoise configuration ---
from CaloTools.CaloNoiseCondAlg import CaloNoiseCondAlg
CaloNoiseCondAlg ('totalNoise')

# --- BunchCrossing Tool configuration ---
from TrigBunchCrossingTool.BunchCrossingTool import BunchCrossingTool
theBCTool = BunchCrossingTool()
ToolSvc += theBCTool

# --- SCTClusteringTool configuration ---
from SiClusterizationTool.SiClusterizationToolConf import InDet__SCT_ClusteringTool
InDetSCT_ClusteringTool = InDet__SCT_ClusteringTool()


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
 
from AthenaCommon.GlobalFlags import globalflags
if globalflags.DataSource()=='data' :
   topSequence.LArCollisionTimeAlg.isMC = False
else :
   topSequence.LArCollisionTimeAlg.isMC = True
 
# dont require anymore explicitely that the time iteration converges
topSequence.LArCollisionTimeAlg.cutIteration = False
topSequence.LArCollisionTimeAlg.OutputLevel = INFO
###################################################################

from PyUtils.MetaReaderPeeker import metadata
if 'IS_SIMULATION' not in metadata['eventTypes']:
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

# Condition algorithm for SiTrackerSpacePointFinder
from AthenaCommon.AlgSequence import AthSequencer
condSeq = AthSequencer("AthCondSeq")
if not hasattr(condSeq, "InDetSiElementPropertiesTableCondAlg"):
   from SiSpacePointFormation.SiSpacePointFormationConf import InDet__SiElementPropertiesTableCondAlg
   condSeq += InDet__SiElementPropertiesTableCondAlg(name = "InDetSiElementPropertiesTableCondAlg")

from LArCafJobs.LArCafJobsConf import LArNoiseBursts
topSequence += LArNoiseBursts( "LArNoiseBursts" )
topSequence.LArNoiseBursts.BCTool = theBCTool
topSequence.LArNoiseBursts.SCTClusteringTool = InDetSCT_ClusteringTool
topSequence.LArNoiseBursts.SigmaCut = 3.0
topSequence.LArNoiseBursts.MBTSCellChargeThreshold = 44./222.
topSequence.LArNoiseBursts.NumberOfBunchesInFront = 30
topSequence.LArNoiseBursts.OutputLevel = DEBUG

prodFlags = jobproperties.LArNoiseBurstFlags

from GaudiSvc.GaudiSvcConf import THistSvc
svcMgr += THistSvc()
svcMgr.THistSvc.Output = ["TTREE DATAFILE='"+prodFlags.outputFile()+"' OPT='RECREATE'"]

