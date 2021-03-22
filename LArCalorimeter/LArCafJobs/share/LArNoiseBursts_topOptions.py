# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)

# Event selector
import AthenaPoolCnvSvc.ReadAthenaPool

from AthenaServices.AthenaServicesConf import AthenaEventLoopMgr
AthenaEventLoopMgr.OutputLevel = WARNING

from PyJobTransformsCore.runargs import RunArguments
runArgs = RunArguments()
runArgs.inputESDFile = [
"rfio:/castor/cern.ch/grid/atlas/tzero/prod1/perm/data14_cos/physics_CosmicCalo/00247390/data14_cos.00247390.physics_CosmicCalo.recon.ESD.x289/data14_cos.00247390.physics_CosmicCalo.recon.ESD.x289._lb0084._SFO-ALL._0001.1"
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

# 11/2014 : 3 lines below commented by B.Trocme to avoid crash with 19.1.3.7 
# ("duplicated object"). Not sure if LArNoisyRO_Std/SatTight are properly 
# computed in a different place. 
#from LArCellRec.LArNoisyROSummaryGetter import LArNoisyROSummaryGetter
#LArNoisyROSummaryGetter.ignoreExistingDataObject = lambda x: True
#LArNoisyROSummaryGetter()
# End of B.Trocme 11/2014 modification
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

from LArCafJobs.LArCafJobsConf import LArNoiseBursts
topSequence += LArNoiseBursts( "LArNoiseBursts" )
topSequence.LArNoiseBursts.BCTool = theBCTool
topSequence.LArNoiseBursts.SigmaCut = 3.0
topSequence.LArNoiseBursts.NumberOfBunchesInFront = 30
topSequence.LArNoiseBursts.OutputLevel = INFO

from GaudiSvc.GaudiSvcConf import THistSvc
svcMgr += THistSvc()
svcMgr.THistSvc.Output = ["TTREE DATAFILE='LArNoiseBursts_TTree.root' OPT='RECREATE'"]

