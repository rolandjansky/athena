# This is the job options file to create classifcation, weighting and
# out-of-cluster histos from ESD
###############################################################
#
# Job options file
#
# Authors: Sven Menke
#==============================================================
doClassification=True 
doWeighting=True
doOutOfCluster=True
doDeadMaterial=True

if not 'ClassificationType' in dir():
  ClassificationType="None"
  #ClassificationType="ParticleID_HAD"

if not 'inFileName' in dir():
  inFileName=['ESD.371530._000247.pool.root.1']

if not 'outFileNameLCC' in dir():
  outFileNameLCC='classify.root'
  #outFileNameLCC=ClassificationType+'_'+'classify.root'

if not 'outFileNameLCW' in dir():
  outFileNameLCW='weights.root'
  #outFileNameLCW=ClassificationType+'_'+'weights.root'

if not 'outFileNameLCO' in dir():
  outFileNameLCO='ooc.root'
  #outFileNameLCO=ClassificationType+'_'+'ooc.root'

if not 'outFileNameLCDM' in dir():
  outFileNameLCDM='dmc.root'
  #outFileNameLCDM=ClassificationType+'_'+'DeadMaterialTree.root'


#DetDescrVersion = 'ATLAS-GEO-08-00-01'
#DetDescrVersion = 'ATLAS-GEO-16-00-00'
DetDescrVersion = 'ATLAS-GEO-18-01-00'

include("RecExCond/AllDet_detDescr.py")

import AthenaCommon.Constants as Lvl
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.AppMgr import theApp

## load POOL support
import AthenaPoolCnvSvc.ReadAthenaPool

## general job configuration
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#from AthenaCommon.BeamFlags import jobproperties
#jobproperties.Beam.numberOfCollisions = 8.0


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
svcMgr.EventSelector.InputCollections = inFileName

if not 'doClassification' in dir():
    doClassification=False
pass
if not 'doWeighting' in dir():
    doWeighting=False
pass
if not 'doOutOfCluster' in dir():
    doOutOfCluster=False
pass
if not 'doDeadMaterial' in dir():
    doDeadMaterial=False
pass

if doClassification:
    include ("CaloLocalHadCalib/GetLCClassification_jobOptions.py") 
    GetLCC.ClassificationType=ClassificationType
pass
if doWeighting:
    include ("CaloLocalHadCalib/GetLCWeights_jobOptions.py") 
    GetLCW.ClassificationType=ClassificationType
pass
if doOutOfCluster:
    include ("CaloLocalHadCalib/GetLCOutOfCluster_jobOptions.py") 
    GetLCO.ClassificationType=ClassificationType
pass
if doDeadMaterial:
    include ("CaloLocalHadCalib/GetLCDeadMaterialTree_jobOptions.py") 
pass
print topSequence

ChronoStatSvc = Service( "ChronoStatSvc" )
ChronoStatSvc.ChronoDestinationCout = True

ChronoStatSvc.PrintUserTime     = True
ChronoStatSvc.PrintSystemTime   = True
ChronoStatSvc.PrintEllapsedTime = True
AthenaPoolCnvSvc = Service( "AthenaPoolCnvSvc" )
AthenaPoolCnvSvc.UseDetailChronoStat = True
