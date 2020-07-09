# This is the job options file to create weight histos from ESD
###############################################################
#
# Job options file
#
# Authors: Sven Menke
#==============================================================
DetDescrVersion = 'ATLAS-GEO-08-00-01'
include("RecExCond/AllDet_detDescr.py")

import AthenaCommon.Constants as Lvl
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.AppMgr import theApp

## load POOL support
import AthenaPoolCnvSvc.ReadAthenaPool

## general job configuration
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## configure object key store to recognize calo cells 
from RecExConfig.ObjKeyStore import ObjKeyStore, objKeyStore
oks = ObjKeyStore()
oks.addStreamESD('CaloCellContainer', ['AllCalo'] )

## re-do topo clusters on EM scale
from CaloRec.CaloTopoClusterFlags import jobproperties
jobproperties.CaloTopoClusterFlags.doTopoClusterLocalCalib = False
jobproperties.CaloTopoClusterFlags.doCellWeightCalib = False
from CaloRec.CaloClusterTopoGetter import CaloClusterTopoGetter
CaloClusterTopoGetter()
topSequence.CaloTopoCluster.TopoCalibMoments.MomentsNames += ["ENG_CALIB_OUT_L"]

theApp.EvtMax = -1

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = svcMgr.MessageSvc
MessageSvc.OutputLevel = Lvl.INFO
MessageSvc.infoLimit = 1000000
svcMgr.EventSelector.InputCollections = inFileName

include ("CaloLocalHadCalib/GetLCWeights_jobOptions.py") 

print topSequence

ChronoStatSvc = Service( "ChronoStatSvc" )
ChronoStatSvc.ChronoDestinationCout = True

ChronoStatSvc.PrintUserTime     = True
ChronoStatSvc.PrintSystemTime   = True
ChronoStatSvc.PrintEllapsedTime = True
AthenaPoolCnvSvc = Service( "AthenaPoolCnvSvc" )
AthenaPoolCnvSvc.UseDetailChronoStat = True


