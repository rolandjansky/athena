# Athena job to run user analysis algorithm on H6 combined testbeam ESD
# (not able to write ESD yet)
# (runs with 'athena RecExTBH6_esdtoesd.py' )


if not 'inFileName' in dir():
  inFileName=["/ltmp/gdp/h6prod/rdo2esd/ESD_2.root"]

if not 'outFileName' in dir():
  outFileName="LCSinglePionsPerformance.root"

if not 'doBeamQuality' in dir():
  doBeamQuality=False


import AthenaCommon.Constants as Lvl
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.AppMgr import theApp

theApp.EvtMax = -1


## load POOL support
import AthenaPoolCnvSvc.ReadAthenaPool
svcMgr.AthenaSealSvc.OutputLevel = Lvl.ERROR

## general job configuration
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_ctbh6()
GlobalFlags.Luminosity.set_zero()
DetDescrVersion = 'ATLAS-H6-2004-00'
from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.all_setOff()
DetFlags.pileup.all_setOff()
DetFlags.makeRIO.all_setOff()
DetFlags.readRDOPool.all_setOff()
DetFlags.readRDOBS.all_setOff()
GlobalFlags.InputFormat.set_pool()
include( "LArAthenaPool/LArAthenaPool_joboptions.py" )
DetFlags.detdescr.LAr_setOn()
from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = "ATLAS-H6-2004-00"
#need also sim Flags unfortunatelly
from G4AtlasApps.SimFlags import SimFlags
SimFlags.SimLayout='tb_LArH6_2004'
SimFlags.LArTB_H6Hec.set_Value(True)
SimFlags.LArTB_H6Hec.set_On()
SimFlags.LArTB_H6Emec.set_Value(True)
SimFlags.LArTB_H6Emec.set_On()
SimFlags.LArTB_H6Fcal.set_Value(True)
SimFlags.LArTB_H6Fcal.set_On()
SimFlags.LArTB_H6Coldnose.set_Value(True)
SimFlags.LArTB_H6Coldnose.set_On()
SimFlags.LArTB_H6Run1.set_Value(False)
SimFlags.LArTB_H6Run1.set_On()
# Bloct the trigger tower mapping, doesn't exist for TB
include.block ( "CaloConditions/LArTTCellMap_ATLAS_jobOptions.py" )
include.block ( "CaloConditions/CaloTTIdMap_ATLAS_jobOptions.py" )
include ("LArDetDescr/LArDetDescr_H6_joboptions.py")
include("LArConditionsCommon/LArConditionsCommon_H6G4_jobOptions.py")
include( "LArConditionsCommon/LArIdMap_H6_jobOptions.py" )
include( "CaloCondAthenaPool/CaloCondAthenaPool_joboptions.py")
PoolSvc.ReadCatalog += [ "prfile:PoolCat_H6_tbcond.xml" ]
print "FileCatalog: ", PoolSvc.ReadCatalog


# - GeoModel ---
include("AtlasGeoModel/GeoModelInit.py")
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc.AtlasVersion='ATLAS-H6-2004-00'

from RecExConfig.ObjKeyStore import ObjKeyStore, objKeyStore
oks = ObjKeyStore()
oks.addStreamESD('CaloCellContainer', ['AllCalo'] )


# ----------------- new topo clustering --------------
from CaloRec.CaloTopoClusterFlags import jobproperties
jobproperties.CaloTopoClusterFlags.doCellWeightCalib = False
jobproperties.CaloTopoClusterFlags.doTopoClusterLocalCalib = True
jobproperties.CaloTopoClusterFlags.doCalibHitMoments = True

include ("CaloRec/CaloTopoCluster_jobOptions.py" )

TopoCalibMoments=topSequence.CaloTopoCluster.TopoCalibMoments
TopoCalibMoments.MomentsNames += ["ENG_CALIB_OUT_L"]
topSequence.CaloTopoCluster.TopoCalibMoments.MatchDmType = 1 # 1=loose, 2=medium (default), 3=tight
TopoCalibMoments.CalibrationHitContainerNames = ["LArCalibrationHitInactive","LArCalibrationHitActive"]
TopoCalibMoments.DMCalibrationHitContainerNames = ["LArCalibrationHitDeadMaterial"]

topSequence.CaloTopoCluster.KeepCorrectionToolAndContainerNames += [
    "CaloClusterLocalCalib/OOCCalib","CaloWTopoCluster",
    "CaloClusterLocalCalib/DMCalib", "CaloOOCTopoCluster"]

TopoSeed = 4.0
TopoNeighbour = 2.0
TopoCell = 0.0
topSequence.CaloTopoCluster.TopoMaker.CellThresholdOnAbsEinSigma     =    TopoCell
topSequence.CaloTopoCluster.TopoMaker.NeighborThresholdOnAbsEinSigma =    TopoNeighbour
topSequence.CaloTopoCluster.TopoMaker.SeedThresholdOnEorAbsEinSigma  =    TopoSeed
# FCAL2 is excluded
topSequence.CaloTopoCluster.TopoMaker.SeedSamplingNames = ["PreSamplerB", "EMB1", "EMB2", "EMB3",
                                                           "PreSamplerE", "EME1", "EME2", "EME3",
                                                           "HEC0", "HEC1","HEC2", "HEC3",
                                                           "TileBar0", "TileBar1", "TileBar2",
                                                           "TileExt0", "TileExt1", "TileExt2",
                                                           "TileGap1", "TileGap2", "TileGap3",
                                                           "FCAL0", "FCAL1"] 

doSimpleNoiseTool = True
if doSimpleNoiseTool:
   SimpleNoiseFile = "sigmas_ave.dat"
   from CaloTools.CaloToolsConf import SimpleNoiseToolFromTextFile
   SimpleNoiseTool = SimpleNoiseToolFromTextFile("SimpleNoiseTool")
   SimpleNoiseTool.CellNoiseFileName = SimpleNoiseFile
   ToolSvc += SimpleNoiseTool

   topSequence.CaloTopoCluster.TopoMaker.CaloNoiseTool           = SimpleNoiseTool
   topSequence.CaloTopoCluster.LocalCalib.LCWeight.CaloNoiseTool = SimpleNoiseTool
   topSequence.CaloTopoCluster.DMCalib.LCDeadMaterial.CaloNoiseTool = SimpleNoiseTool

# remove annoying BadChannel tool
tlist=[]
for x in topSequence.CaloTopoCluster.ClusterCorrectionTools:
  if x.find("BadChan") == -1:
     tlist += [x]
  else:   
     print "Removing: ",x," from CaloTopoCluster.ClusterCorrectionTools" 
topSequence.CaloTopoCluster.ClusterCorrectionTools = tlist


# =============================================================================
#
# Beam Quality
#
# =============================================================================
if doBeamQuality:
  from TBRec.TBRecConf import TBBeamQualityMC
  TBBeamQuality = TBBeamQualityMC()
  TBBeamQuality.ReadFileforXcryo = False
  TBBeamQuality.CheckTrackParams = False
  TBBeamQuality.TrackCutX = 35.
  TBBeamQuality.TrackCutY = 35.
  TBBeamQuality.CheckPrimaryTrack = True
  TBBeamQuality.ScintForPrimaryTrack = [6,7]
  TBBeamQuality.CheckVetoScint = True
  from AthenaCommon.AppMgr import athMasterSeq
  athMasterSeq  += TBBeamQuality
  #from AthenaServices.AthenaServicesConf import AthenaEventLoopMgr
  #AthenaEventLoopMgr.FailureMode=2

# =============================================================================
#
#
#                                 user analysis
#
#
# =============================================================================
#include ("H6CTBHadCalib/H6ReadESDWriteTree_jobOptions.py") 

include ("CaloLocalHadCalib/GetLCSinglePionsPerf_jobOptions.py") 
lcPerf.CalibrationHitContainerNames= ["LArCalibrationHitInactive","LArCalibrationHitActive"]
lcPerf.DMCalibrationHitContainerNames= ["LArCalibrationHitDeadMaterial"]
lcPerf.doRecoEfficiency = True
lcPerf.isTestbeam = True


print "topSequence",topSequence
print "dumpMasterSequence"
from AthenaCommon.AlgSequence import dumpMasterSequence
dumpMasterSequence()

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = svcMgr.MessageSvc
MessageSvc.OutputLevel = Lvl.WARNING
#MessageSvc.OutputLevel = Lvl.DEBUG
MessageSvc.infoLimit = 500000
svcMgr.EventSelector.InputCollections = inFileName
#svcMgr.EventSelector.SkipEvents = 4070

#--------------------------------------------------------------
# statistics
#--------------------------------------------------------------
ChronoStatSvc = Service( "ChronoStatSvc" )
ChronoStatSvc.ChronoDestinationCout = True
ChronoStatSvc.PrintUserTime     = True
ChronoStatSvc.PrintSystemTime   = True
ChronoStatSvc.PrintEllapsedTime = True
AthenaPoolCnvSvc = Service( "AthenaPoolCnvSvc" )
AthenaPoolCnvSvc.UseDetailChronoStat = True
