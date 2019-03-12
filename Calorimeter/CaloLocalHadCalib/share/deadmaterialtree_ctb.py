# This is the job options file to create DeadMaterialTree from ESD
###############################################################
#
# Job options file
#
# Authors: Gennady Pospelov
#==============================================================

#
# athena.py -c 'inFileName=["ESD.pool.root"];outFileName="dmtree.root"' CaloLocalHadCalib/deadmaterialtree.py
#

if not 'inFileName' in dir():
  #inFileName=["h6prod.100004.piplus_logE.simul.qgspbert.ESD.v15520._00001.pool.root"]
  #inFileName=["h6prod.100004.piplus_logE.simul.qgspbert.ESD.v15520._00001.pool.root"]
  #inFileName=["h6prod.100004.piplus_logE.simul.qgspbert.ESD.v15520._00032.pool.root"]
  inFileName=["h6prod.100004.pizero_logE.simul.qgspbert.ESD.v15520._00001.pool.root"]

if not 'outFileNameLCDM' in dir():
  outFileNameLCDM="dmtreeX.root"

if not 'doBeamQuality' in dir():
  doBeamQuality=True

if not 'doTopoClusterAgain' in dir():
  doTopoClusterAgain=True




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


from AthenaCommon.AppMgr import athMasterSeq

# splitting DM container into two parts to get rid from DM outside of our quadrant
from TBRec.TBRecConf import TBDMContainerSplitter
DMContSplit = TBDMContainerSplitter()
DMContSplit.InputDMContainer = "LArCalibrationHitDeadMaterial"
DMContSplit.OutputDM_Calo = "LArCalibrationHitDeadMaterial_Calo"
DMContSplit.OutputDM_Leak = "LArCalibrationHitDeadMaterial_Leakage"
athMasterSeq += DMContSplit
 

# ----------------- new topo clustering --------------
if doTopoClusterAgain:
  from CaloRec.CaloTopoClusterFlags import jobproperties
  jobproperties.CaloTopoClusterFlags.doCellWeightCalib = False
  jobproperties.CaloTopoClusterFlags.doTopoClusterLocalCalib = True
  jobproperties.CaloTopoClusterFlags.doCalibHitMoments = True
  jobproperties.CaloTopoClusterFlags.printTopoClusters = True

  include ("CaloRec/CaloTopoCluster_jobOptions.py" )

  TopoCalibMoments=topSequence.CaloTopoCluster.TopoCalibMoments
#  TopoCalibMoments.MomentsNames += ["ENG_CALIB_OUT_L"]
#  topSequence.CaloTopoCluster.TopoCalibMoments.MatchDmType = 1 # 1=loose, 2=medium (default), 3=tight
  TopoCalibMoments.CalibrationHitContainerNames = ["LArCalibrationHitInactive","LArCalibrationHitActive"]
  TopoCalibMoments.DMCalibrationHitContainerNames = ["LArCalibrationHitDeadMaterial"]
  #TopoCalibMoments.DMCalibrationHitContainerNames = ["LArCalibrationHitDeadMaterial_Calo"]

  topSequence.CaloTopoCluster.KeepCorrectionToolAndContainerNames += [
      "CaloClusterLocalCalib/OOCCalib","CaloWeightTopoCluster",
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
  # simple noise tool
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

  #include("moreclusters.py")
  #topSequence.CaloTopoClusterEMAll.KeepCorrectionToolAndContainerNames += [
    #"CaloClusterLocalCalib/OOCCalib","CaloWeightTopoCluster_EMAll",
    #"CaloClusterLocalCalib/DMCalib", "CaloOOCTopoCluster_EMAll"
  #]
  ##topSequence.CaloTopoClusterEMAll.LockVariables.FixBasicEnergy = True

  #topSequence.CaloTopoClusterHadAll.KeepCorrectionToolAndContainerNames += [
    #"CaloClusterLocalCalib/OOCCalib","CaloWeightTopoCluster_HadAll",
    #"CaloClusterLocalCalib/DMCalib", "CaloOOCTopoCluster_HadAll"
  #]


# =============================================================================
#
# Beam Quality
#
# =============================================================================
if doBeamQuality:
  from TBRec.TBRecConf import TBBeamQualityMC
  TBBeamQuality = TBBeamQualityMC()
  # for piplusminus
  #TBBeamQuality.ReadFileforXcryo = False
  #TBBeamQuality.CheckTrackParams = False
  #TBBeamQuality.TrackCutX = 35.
  #TBBeamQuality.TrackCutY = 35.
  #TBBeamQuality.CheckPrimaryTrack = True
  #TBBeamQuality.ScintForPrimaryTrack = [6,7]
  #TBBeamQuality.CheckVetoScint = True

  #TBBeamQuality.CheckClusters = True
  #TBBeamQuality.CheckTrackReco = True

  # for pizero
  TBBeamQuality.ReadFileforXcryo = False
  TBBeamQuality.CheckTrackParams = False
  TBBeamQuality.TrackCutX = 35.
  TBBeamQuality.TrackCutY = 35.
  TBBeamQuality.CheckPrimaryTrack = False
  TBBeamQuality.ScintForPrimaryTrack = [6,7]
  TBBeamQuality.CheckVetoScint = False

  TBBeamQuality.CheckClusters = True
  TBBeamQuality.CheckTrackReco = False

  print TBBeamQuality
  athMasterSeq  += TBBeamQuality

# =============================================================================
#
#
#                                 user analysis
#
#
# =============================================================================
include ("CaloLocalHadCalib/GetLCDeadMaterialTree_jobOptions.py") 
GetLCDM.HadDMCoeffInitFile = "CaloHadDMCoeff_H6_init_v2.txt"
GetLCDM.isTestbeam = True

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

