# This is the job options file to create DeadMaterialTree from ESD
###############################################################
#
# Job options file
#
# Authors: Gennady Pospelov
#==============================================================

#
# athena.py -c 'inFileName=["ESD.pool.root"];outFileName="dmtreeX.root"' CaloLocalHadCalib/deadmaterialtree.py
#
import AthenaCommon.Constants as Lvl
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.AppMgr import theApp

#inFileName=["/ltmp/gdp/aaa/ESD.081901._001996.pool.root.1"]
#outFileNameLCDM="dmtreeX.root"

## load POOL support
import AthenaPoolCnvSvc.ReadAthenaPool

## general job configuration
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#from AthenaCommon.DetFlags import DetFlags
#DetFlags.ID_setOff()
#DetFlags.Muon_setOff()
#DetFlags.LVL1_setOff()

#DetDescrVersion = 'ATLAS-CSC-05-00-00'
DetDescrVersion = 'ATLAS-GEO-08-00-01'
include("RecExCond/AllDet_detDescr.py")

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
topSequence.CaloTopoCluster.TopoCalibMoments.MomentsNames += ["ENG_CALIB_OUT_L","ENG_CALIB_FRAC_EM"]
topSequence.CaloTopoCluster.TopoCalibMoments.MatchDmType = 1 # 1=loose, 2=medium (default), 3=tight

theApp.EvtMax = -1

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = svcMgr.MessageSvc
MessageSvc.OutputLevel = Lvl.INFO
MessageSvc.infoLimit = 1000000
if vars().has_key('inFileName'):
  svcMgr.EventSelector.InputCollections = inFileName

include ("CaloLocalHadCalib/GetLCDeadMaterialTree_jobOptions.py") 

print topSequence

ChronoStatSvc = Service( "ChronoStatSvc" )
ChronoStatSvc.ChronoDestinationCout = True

ChronoStatSvc.PrintUserTime     = True
ChronoStatSvc.PrintSystemTime   = True
ChronoStatSvc.PrintEllapsedTime = True
AthenaPoolCnvSvc = Service( "AthenaPoolCnvSvc" )
AthenaPoolCnvSvc.UseDetailChronoStat = True


