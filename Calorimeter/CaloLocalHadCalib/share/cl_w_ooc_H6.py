# Athena job to extract classification, weighting, and out-of-cluster
# constants on H6

from math import ( pi as m_pi, log10 as m_log10 )

#inFileName


if not 'inFileName' in dir():
  inFileName=["h6prod.100002.piminus_logE.simul.qgspbert.ESD.v15520._00340.pool.root"]

if not 'outFileNameLCC' in dir():
  outFileNameLCC='classify.root'

if not 'outFileNameLCW' in dir():
  outFileNameLCW='weights.root'

if not 'outFileNameLCO' in dir():
  outFileNameLCO='ooc.root'

#outFileNameLCC='classify.root'
#outFileNameLCW='weights.root'
#outFileNameLCO='ooc.root'


doClassification=True 
doWeighting=True
doOutOfCluster=True


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

  include ("CaloRec/CaloTopoCluster_jobOptions.py" )

  TopoCalibMoments=topSequence.CaloTopoCluster.TopoCalibMoments
  TopoCalibMoments.MomentsNames += ["ENG_CALIB_OUT_L"]
  topSequence.CaloTopoCluster.TopoCalibMoments.MatchDmType = 1 # 1=loose, 2=medium (default), 3=tight
  TopoCalibMoments.CalibrationHitContainerNames = ["LArCalibrationHitInactive","LArCalibrationHitActive"]
  #TopoCalibMoments.DMCalibrationHitContainerNames = ["LArCalibrationHitDeadMaterial"]
  TopoCalibMoments.DMCalibrationHitContainerNames = ["LArCalibrationHitDeadMaterial_Calo"]

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


# =============================================================================
#
# Beam Quality
#
# =============================================================================
if doBeamQuality:
  from TBRec.TBRecConf import TBBeamQualityMC
  TBBeamQuality = TBBeamQualityMC()
  # for piplusminus
  TBBeamQuality.ReadFileforXcryo = False
  TBBeamQuality.CheckTrackParams = False
  TBBeamQuality.TrackCutX = 35.
  TBBeamQuality.TrackCutY = 35.
  TBBeamQuality.CheckPrimaryTrack = True
  TBBeamQuality.ScintForPrimaryTrack = [6,7]
  TBBeamQuality.CheckVetoScint = True
  ##
  TBBeamQuality.CheckClusters = True
  TBBeamQuality.CheckTrackReco = True

  # for pizero
  #TBBeamQuality.ReadFileforXcryo = False
  #TBBeamQuality.CheckTrackParams = False
  #TBBeamQuality.TrackCutX = 35.
  #TBBeamQuality.TrackCutY = 35.
  #TBBeamQuality.CheckPrimaryTrack = False
  #TBBeamQuality.ScintForPrimaryTrack = [6,7]
  #TBBeamQuality.CheckVetoScint = False
  
  #TBBeamQuality.CheckClusters = True
  #TBBeamQuality.CheckTrackReco = False

  athMasterSeq  += TBBeamQuality

# =============================================================================
#
#
#                                 user analysis
#
#
# =============================================================================
#include ("H6CTBHadCalib/H6ReadESDWriteTree_jobOptions.py") 


if not 'doClassification' in dir():
    doClassification=False
pass
if not 'doWeighting' in dir():
    doWeighting=False
pass
if not 'doOutOfCluster' in dir():
    doOutOfCluster=False
pass

nPhiBin = 5

if doClassification:
    include ("CaloLocalHadCalib/GetLCClassification_jobOptions.py") 
    topSequence.GetLCC.ClassificationDimensions = {
    'side': ('side',-1.5,1.5,1),
    '|eta|': ('|eta|',2.0,5.,15),
    'phi': ('phi',m_pi/2,m_pi,nPhiBin),
    'log10(E_clus (MeV))': ('log10(E_clus (MeV))',m_log10(1*GeV),m_log10(200*GeV),9),
    'log10(<rho_cell (MeV/mm^3)>)-log10(E_clus (MeV))': ('log10(<rho_cell (MeV/mm^3)>)-log10(E_clus (MeV))',-9.0,-4.0,20),
    'log10(lambda_clus (mm))': ('log10(lambda_clus (mm))',0.0,4.0,20)}
    #topSequence.GetLCC.NormalizationType = "const"
    pass
if doWeighting:
  include ("CaloLocalHadCalib/GetLCWeights_jobOptions.py") 
  topSequence.GetLCW.SamplingDimensions = {
    'EME2:EME2': ('EME2',5.5,6.5,1),
    'EME2:side': ('side',-1.5,1.5,1),
    'EME2:|eta|': ('|eta|',2.5,3.2,7),
    'EME2:phi': ('phi',m_pi/2,m_pi,nPhiBin),
    'EME2:log10(E_clus (MeV))': ('log10(E_clus (MeV))',m_log10(1*GeV),m_log10(200*GeV),14),
    'EME2:log10(rho_cell (MeV/mm^3))': ('log10(rho_cell (MeV/mm^3))',-7.0,1.0,20),
    'EME2:weight': ('weight',-2.0,3.0,1),
    'EME3:EME3': ('EME3',6.5,7.5,1),
    'EME3:side': ('side',-1.5,1.5,1),
    'EME3:|eta|': ('|eta|',2.5,3.2,7),
    'EME3:phi': ('phi',m_pi/2,m_pi, nPhiBin),
    'EME3:log10(E_clus (MeV))': ('log10(E_clus (MeV))',m_log10(1*GeV),m_log10(200*GeV),14),
    'EME3:log10(rho_cell (MeV/mm^3))': ('log10(rho_cell (MeV/mm^3))',-7.0,1.0,20),
    'EME3:weight': ('weight',-2.0,3.0,1),
    'HEC0:HEC0': ('HEC0',7.5,8.5,1),
    'HEC0:side': ('side',-1.5,1.5,1),
    'HEC0:|eta|': ('|eta|',2.1,3.3,6),
    'HEC0:phi': ('phi',m_pi/2,m_pi, nPhiBin),
    'HEC0:log10(E_clus (MeV))': ('log10(E_clus (MeV))',m_log10(1*GeV),m_log10(200*GeV),14),
    'HEC0:log10(rho_cell (MeV/mm^3))': ('log10(rho_cell (MeV/mm^3))',-7.0,1.0,20),
    'HEC0:weight': ('weight',-2.0,3.0,1),
    'HEC1:HEC1': ('HEC1',8.5,9.5,1),
    'HEC1:side': ('side',-1.5,1.5,1),
    'HEC1:|eta|': ('|eta|',2.1,3.3,6),
    'HEC1:phi': ('phi',m_pi/2,m_pi, nPhiBin),
    'HEC1:log10(E_clus (MeV))': ('log10(E_clus (MeV))',m_log10(1*GeV),m_log10(200*GeV),14),
    'HEC1:log10(rho_cell (MeV/mm^3))': ('log10(rho_cell (MeV/mm^3))',-7.0,1.0,20),
    'HEC1:weight': ('weight',-2.0,3.0,1),
    'HEC2:HEC2': ('HEC2',9.5,10.5,1),
    'HEC2:side': ('side',-1.5,1.5,1),
    'HEC2:|eta|': ('|eta|',2.1,3.3,6),
    'HEC2:phi': ('phi',m_pi/2,m_pi, nPhiBin),
    'HEC2:log10(E_clus (MeV))': ('log10(E_clus (MeV))',m_log10(1*GeV),m_log10(200*GeV),14),
    'HEC2:log10(rho_cell (MeV/mm^3))': ('log10(rho_cell (MeV/mm^3))',-7.0,1.0,20),
    'HEC2:weight': ('weight',-2.0,3.0,1),
    'HEC3:HEC3': ('HEC3',10.5,11.5,1),
    'HEC3:side': ('side',-1.5,1.5,1),
    'HEC3:|eta|': ('|eta|',2.1,3.3,6),
    'HEC3:phi': ('phi',m_pi/2,m_pi, nPhiBin),
    'HEC3:log10(E_clus (MeV))': ('log10(E_clus (MeV))',m_log10(1*GeV),m_log10(200*GeV),14),
    'HEC3:log10(rho_cell (MeV/mm^3))': ('log10(rho_cell (MeV/mm^3))',-7.0,1.0,20),
    'HEC3:weight': ('weight',-2.0,3.0,1),
    'FCal1:FCal1': ('FCal1',20.5,21.5,1),
    'FCal1:side': ('side',-1.5,1.5,1),
    'FCal1:|eta|': ('|eta|',2.8,5.0,11),
    'FCal1:phi': ('phi',m_pi/2,m_pi, nPhiBin),
    'FCal1:log10(E_clus (MeV))': ('log10(E_clus (MeV))',m_log10(1*GeV),m_log10(200*GeV),14),
    'FCal1:log10(rho_cell (MeV/mm^3))': ('log10(rho_cell (MeV/mm^3))',-7.0,1.0,20),
    'FCal1:weight': ('weight',-2.0,3.0,1),
    'FCal2:FCal2': ('FCal2',21.5,22.5,1),
    'FCal2:side': ('side',-1.5,1.5,1),
    'FCal2:|eta|': ('|eta|',2.8,5.0,11),
    'FCal2:phi': ('phi',m_pi/2,m_pi, nPhiBin),
    'FCal2:log10(E_clus (MeV))': ('log10(E_clus (MeV))',m_log10(1*GeV),m_log10(200*GeV),14),
    'FCal2:log10(rho_cell (MeV/mm^3))': ('log10(rho_cell (MeV/mm^3))',-7.0,1.0,20),
    'FCal2:weight': ('weight',-2.0,3.0,1)}
  topSequence.GetLCW.CalibrationHitContainerNames = ["LArCalibrationHitInactive"
                                                     ,"LArCalibrationHitActive"]
  #topSequence.GetLCW.NormalizationType = "const"
  pass
if doOutOfCluster:
  include ("CaloLocalHadCalib/GetLCOutOfCluster_jobOptions.py") 
  topSequence.GetLCO.OutOfClusterDimensions = {
    'side': ('side',-1.5,1.5,1),
    '|eta|': ('|eta|',2.0,5.,15),
    'phi': ('phi',m_pi/2,m_pi, nPhiBin),
    'log10(E_clus (MeV))': ('log10(E_clus (MeV))',m_log10(1*GeV),m_log10(200*GeV),14),
    'log10(lambda_clus (mm))': ('log10(lambda_clus (mm))',0.0,4.0,20),
    'weight': ('weight',0.,5.,1)}
  #topSequence.GetLCO.NormalizationType = "const"
  pass

# include ("CaloLocalHadCalib/GetLCSinglePionsPerf_jobOptions.py") 
# lcPerf.CalibrationHitContainerNames= ["LArCalibrationHitInactive","LArCalibrationHitActive"]
# #lcPerf.DMCalibrationHitContainerNames= ["LArCalibrationHitDeadMaterial"]
# lcPerf.DMCalibrationHitContainerNames= ["LArCalibrationHitDeadMaterial_Calo"]
# lcPerf.etamin = 2.5
# lcPerf.etamax = 5.0
# lcPerf.netabin = 25
# lcPerf.phimin = m_pi*3./4. - 0.15
# lcPerf.phimax = m_pi*3./4. + 0.15
# lcPerf.nphibin = 1
# lcPerf.logenermin = 2.1
# lcPerf.logenermax = 5.5
# lcPerf.nlogenerbin = 17
# lcPerf.doRecoEfficiency = True
# lcPerf.useRecoEfficiency = True
# lcPerf.isTestbeam = True
# lcPerf.useGoodClus = True


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
