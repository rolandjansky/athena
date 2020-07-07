# Athena job to get performance plots for local hadronic calibration on single pions
# (runs with 'athena CaloLocalHadCalib/GetLCSinglePionsPerf.py' )

#outFileName="LCSinglePionsPerformance.root"

if not 'inFileName' in dir():
  #inFileName=['/tmp/pospelov/data/ESD.170972._001733.pool.root.1']
  #inFileName=['ESD.371531._000576.pool.root.1']
  inFileName=['ESD.371530._000247.pool.root.1']


import AthenaCommon.Constants as Lvl
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.AppMgr import theApp


## load POOL support
import AthenaPoolCnvSvc.ReadAthenaPool

## general job configuration
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#DetDescrVersion = 'ATLAS-CSC-05-00-00'
#DetDescrVersion = 'ATLAS-GEO-08-00-01'
#DetDescrVersion = 'ATLAS-GEO-16-00-00'
#DetDescrVersion = 'ATLAS-GEO-18-01-00'
DetDescrVersion = 'ATLAS-R2-2015-03-01-00'
include("RecExCond/AllDet_detDescr.py")

#from AthenaCommon.BeamFlags import jobproperties
#jobproperties.Beam.numberOfCollisions = 8.0

## configure object key store to recognize calo cells
from RecExConfig.ObjKeyStore import ObjKeyStore, objKeyStore
oks = ObjKeyStore()
oks.addStreamESD('CaloCellContainer', ['AllCalo'] )

## re-do topo clusters on EM scale
from CaloRec.CaloTopoClusterFlags import jobproperties
jobproperties.CaloTopoClusterFlags.doTopoClusterLocalCalib = True
jobproperties.CaloTopoClusterFlags.doCellWeightCalib = False
from CaloRec.CaloClusterTopoGetter import CaloClusterTopoGetter, addSnapshot
CaloClusterTopoGetter()

from CaloRec.CaloRecConf import CaloClusterSnapshot 

TopoCalibMoments=topSequence.CaloTopoCluster.TopoCalibMoments
TopoCalibMoments.MomentsNames += ["ENG_CALIB_OUT_L"]
topSequence.CaloTopoCluster.TopoCalibMoments.MatchDmType = 1 # 1=loose, 2=medium (default), 3=tight

addSnapshot("OOCPi0Calib","CaloOOCTopoCluster")
addSnapshot("LocalCalib","CaloWTopoCluster")


theApp.EvtMax = -1

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = svcMgr.MessageSvc
MessageSvc.OutputLevel = Lvl.INFO
MessageSvc.infoLimit = 100000
svcMgr.EventSelector.InputCollections = inFileName

include ("CaloLocalHadCalib/GetLCSinglePionsPerf_jobOptions.py") 
lcPerf.useRecoEfficiency = True

print topSequence

ChronoStatSvc = Service( "ChronoStatSvc" )
ChronoStatSvc.ChronoDestinationCout = True

ChronoStatSvc.PrintUserTime     = True
ChronoStatSvc.PrintSystemTime   = True
ChronoStatSvc.PrintEllapsedTime = True
AthenaPoolCnvSvc = Service( "AthenaPoolCnvSvc" )
AthenaPoolCnvSvc.UseDetailChronoStat = True

#include("RecExCommon/RecoUtils.py")

svcMgr.StoreGateSvc.Dump=True
