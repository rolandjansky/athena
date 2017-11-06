#-----------------------------------------------------------------------------
# Athena imports
#-----------------------------------------------------------------------------
from AthenaCommon.Constants import *
from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr
import AthenaPoolCnvSvc.ReadAthenaPool
from AthenaCommon.AlgSequence import AlgSequence
alg = AlgSequence()

OutputLevel = INFO
doJiveXML       = False
doVP1           = False
doWriteESD      = False
doWriteAOD      = False
doReadBS        = False
doAuditors      = True

import os
doEdmMonitor    = False
doNameAuditor   = False

#-----------------------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
ServiceMgr.MessageSvc.OutputLevel = WARNING
import AthenaServices
AthenaServices.AthenaServicesConf.AthenaEventLoopMgr.OutputLevel = WARNING


#-----------------------------------------------------------------------------
# Input Datasets
#-----------------------------------------------------------------------------

ServiceMgr.EventSelector.InputCollections = ['rdo.pool.root']

event=10000
theApp.EvtMax = event # -1 means all events

from InDetBeamSpotService.InDetBeamSpotServiceConf import BeamCondSvc
ServiceMgr+=BeamCondSvc(useDB=False,
                        posX=0.00,
                        posY=0.00,
                        posZ=0.00,
                        sigmaX=0.000001,
                        sigmaY=0.000001,
                        sigmaZ=0.01,
                        tiltX=0.0,
                        tiltY=0.0
# width average from run 161242 - 162347
#                        sigmaX=0.0378,
#                        sigmaY=0.0373,
#                        sigmaZ=66.644,
#                        tiltX=0.0,
#                        tiltY=0.0
#  for run #152166, except for angle info !!! wrong run number !!!
#                        pqosX=-0.353,
#                        pqosY=0.667,
#                        pqosZ=3.05,
#                        sigmaX=0.0444,
#                        sigmaY=0.0757,
#                        sigmaZ=25.6,
#                        tiltX=0.0,
#                        tiltY=-0.0
#  for run #167776, except for angle info
#                        posX=0.0776,
#                        posY=1.13,
#                        posZ=12.,
#                        sigmaX=0.0395,
#                        sigmaY=0.0394,
#                        sigmaZ=0.000025,
#                        tiltX=0.0,
#                        tiltY=0.0
                        )
#

# Reconstruction setup

# DetDescrVersion = "ATLAS-GEO-16-00-00"
# ConditionsTag = "OFLCOND-DR-BS7T-A100-04"
#DetDescrVersion = "ATLAS-GEO-18-01-03"
#ConditionsTag = "OFLCOND-SDR-BS14T-IBL-11"
DetDescrVersion = "ATLAS-IBL-03-00-00"
ConditionsTag = "OFLCOND-MC12-IBL-20-30-25"


#ConditionsTag = "OFLCOND-SDR-BS14T-IBL-03"

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo = 'atlas'
globalflags.DataSource = 'geant4'
globalflags.InputFormat = 'pool'
globalflags.DetDescrVersion = DetDescrVersion
globalflags.ConditionsTag = ConditionsTag
globalflags.print_JobProperties()
from AthenaCommon.BeamFlags import jobproperties
jobproperties.Beam.numberOfCollisions = 0.0  
from CaloRec.CaloCellFlags import jobproperties
jobproperties.CaloCellFlags.doLArCellEmMisCalib=False;

if len(globalflags.ConditionsTag())!=0:
   print 'DEBUG -- If statement entered (code=01234567)'
   from IOVDbSvc.CondDB import conddb
   conddb.setGlobalTag(globalflags.ConditionsTag()) 

from RecExConfig.RecFlags import rec

rec.doCalo=False
rec.doInDet=True
rec.doMuon=False
rec.doJetMissingETTag=False
rec.doEgamma=False
rec.doMuonCombined=False
rec.doTau=False
rec.doTrigger=False

rec.Commissioning.set_Value_and_Lock(True)

from AthenaCommon.DetFlags import DetFlags 
# --- switch on InnerDetector, turn off TRT
DetFlags.ID_setOn()
#DetFlags.makeRIO.pixel_setOff()
#DetFlags.makeRIO.SCT_setOff()
DetFlags.makeRIO.TRT_setOff()

# --- and switch off all the rest
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
# ---- switch parts of ID off/on as follows (always use both lines)
#DetFlags.pixel_setOff()
#DetFlags.detdescr.pixel_setOn()
#DetFlags.SCT_setOff()
#DetFlags.detdescr.SCT_setOn()
DetFlags.TRT_setOff()
DetFlags.detdescr.TRT_setOn()
# --- printout
DetFlags.Print()

#--------------------------------------------------------------
# Load InDet configuration
#--------------------------------------------------------------

# --- setup InDetJobProperties
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doTruth       = True

InDetFlags.doBackTracking.set_Value_and_Lock(True)
InDetFlags.doTRTStandalone.set_Value_and_Lock(True)
InDetFlags.doPixelClusterSplitting.set_Value_and_Lock(False)
InDetFlags.writeRDOs.set_Value_and_Lock(True)
from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.doLVL2.set_Value_and_Lock(True)
TriggerFlags.doEF.set_Value_and_Lock(True)

# --- uncomment to change the default of one of the following options:
#InDetFlags.doNewTracking  = False
#InDetFlags.doLowPt        = True
#InDetFlags.doBeamGas      = True
#InDetFlags.doBeamHalo     = True
#InDetFlags.doxKalman      = False
#InDetFlags.doiPatRec      = False
#InDetFlags.doBackTracking = False
#InDetFlags.doTRTStandalone = False
#InDetFlags.doSingleSpBackTracking = True

# --- Turn off track slimming
#InDetFlags.doSlimming = False

# --- possibility to run tracking on subdetectors separately (and independent from each other)
InDetFlags.doTrackSegmentsPixel = True
InDetFlags.doTrackSegmentsSCT   = True
InDetFlags.doTrackSegmentsTRT   = True

# --- possibility to change the trackfitter
#InDetFlags.trackFitterType = 'KalmanFitter'

# --- activate monitorings
InDetFlags.doMonitoringGlobal    = False
InDetFlags.doMonitoringPixel     = False
InDetFlags.doMonitoringSCT       = False
InDetFlags.doMonitoringTRT       = False
InDetFlags.doMonitoringAlignment = False

# --- activate (memory/cpu) monitoring
#InDetFlags.doPerfMon = True

# --- activate ntuple writing for IDRecStat (if it is on)
#InDetFlags.doStatNtuple    = True

# --- activate creation of standard plots
#InDetFlags.doStandardPlots  = True

# --- produce various ntuples (all in one root file)
#InDetFlags.doTrkNtuple      = True
#InDetFlags.doPixelTrkNtuple = True
#InDetFlags.doVtxNtuple      = True
#InDetFlags.doConvVtxNtuple  = True

# activate the print InDetXYZAlgorithm statements
InDetFlags.doPrintConfigurables = True

from IOVDbSvc.CondDB import conddb;
conddb.addOverride("/PIXEL/PixMapOverlay","PixMapOverlay-IBL-PERFECT");
conddb.addOverride("/SCT/DAQ/Configuration/Geog","SctDaqConfigurationGeog-PERFECT-Aug10-01");
conddb.addOverride("/SCT/DAQ/Configuration/ROD","SctDaqConfigurationRod-PERFECT-Aug10-01");
conddb.addOverride("/SCT/DAQ/Configuration/RODMUR","SctDaqConfigurationRodmur-PERFECT-Aug10-01");
conddb.addOverride("/SCT/DAQ/Configuration/MUR","SctDaqConfigurationMur-PERFECT-Aug10-01");
conddb.addOverride("/SCT/DAQ/Configuration/Module","SctDaqConfigurationModule-PERFECT-Aug10-01");
conddb.addOverride("/SCT/DAQ/Configuration/Chip","SctDaqConfigurationChip-PERFECT-Aug10-01");
conddb.addOverride("/SCT/DAQ/Calibration/NPtGainDefects","SctDaqCalibrationNPtGainDefects-NOMINAL");
conddb.addOverride("/SCT/DAQ/Calibration/NoiseOccupancyDefects","SctDaqCalibrationNoiseOccupancyDefects-NOMINAL") 

include("InDetIBL_Example/preInclude.IBL_Rec.py")
include("InDetIBL_Example/NewLayerMaterial_03.py")
include("InDetIBL_Example/preInclude.AllDetOn.py")

# IMPORTANT NOTE: initialization of the flags and locking them is done in InDetRec_jobOptions.py!
# This way RecExCommon just needs to import the properties without doing anything else!
# DO NOT SET JOBPROPERTIES AFTER THIS LINE! The change will be ignored!
include("InDetRecExample/InDetRec_all.py")

include("InDetIBL_Example/postInclude.IBL_Rec.py")
include("InDetIBL_Example/postInclude.IBL_NoAlign.py")

#--------------------------------------------------------------
# FTK algorithm inclusions
#--------------------------------------------------------------
from AthenaCommon.AppMgr import ToolSvc 
from TrigFTKSim.TrigFTKSimConf import *


print "Add FTK_SGHitInput tool"
FTKSGHit = FTK_SGHitInput()
FTKSGHit.ReadTruthTracks=True
ToolSvc += FTKSGHit


#FTKSGHit.OutputLevel=DEBUG
# print "Add FTKDetector tool"
# FTKDet = FTKDetectorTool()
# FTKDet.doBadModuleMap = False # True to produce or False to read an existing one
# FTKDet.useBadModuleMap = False # may disable the use of dead modules mapping
# #FTKDet.BadSSMapPath = "badSSmap.bmap"
# #FTKDet.FTK_BadModuleMapPath = "badModulemap_FTK.bmap"
# #FTKDet.ATLAS_BadModuleMapPath = "badModulemap_ATLAS.bmap"
# ToolSvc += FTKDet
# #FTKDet.OutputLevel=DEBUG

from TrigFTKBankGen.TrigFTKBankGenConf import *

#FTKBankGen= FTKBankGenAlgo("FTKBankGenAlgo", OutputLevel=DEBUG)
FTKBankGen= FTKBankGenAlgo("FTKBankGenAlgo", OutputLevel=FATAL)
#MC11 for 8L
#FTKBankGen.pmap_path = "../../../TrigFTKSim/config/map_file/raw_8Lc.pmap"
#FTKBankGen.rmap_path = "../../../TrigFTKSim/config/map_file/raw_11L.tmap"

#MC11 for 11L
#FTKBankGen.pmap_path = "../../../TrigFTKSim/config/map_file/raw_11L.pmap"
#FTKBankGen.rmap_path = "../../../TrigFTKSim/config/map_file/raw_11L.tmap"

#MC11 for IBL
FTKBankGen.pmap_path = "../../../TrigFTKSim/config/map_file/raw_12Libl.pmap"
#FTKBankGen.pmap_path = "../../../TrigFTKSim/config/map_file/raw_8LcIbl123.pmap"
FTKBankGen.rmap_path = "../../../TrigFTKSim/config/map_file/raw_11L.imap2"

#MC12 for IBL
#FTKBankGen.pmap_path = "../../../TrigFTKSim/config/map_file/raw_8LcIblEXPtoIBL.pmap"
#FTKBankGen.rmap_path = "../../../TrigFTKSim/config/map_file/raw_11L.imap2"

FTKBankGen.const_test_mode = False;
#FTKBankGen.const_test_mode = True;
#FTKBankGen.sector_dir_path = "/afs/cern.ch/work/t/tomoya/public/my_const_11L"
#FTKBankGen.gcon_dir_path = "/afs/cern.ch/work/t/tomoya/public/my_const_11L"
FTKBankGen.sector_dir_path = "/afs/cern.ch/work/t/tomoya/public/user.tompkins.tdaqtdr_base_8LcIbl123_20130805"
FTKBankGen.gcon_dir_path = "/afs/cern.ch/work/t/tomoya/public/user.tompkins.tdaqtdr_base_8LcIbl123_20130805"

FTKBankGen.Clustering = False;
FTKBankGen.SaveClusterContent = False;
FTKBankGen.DiagClustering = True;
#FTKBankGen.SctClustering = False;
FTKBankGen.PixelClusteringMode = 1;
FTKBankGen.Ibl3DRealistic = False;
FTKBankGen.DuplicateGanged = True;
FTKBankGen.GangedPatternRecognition = False;

FTKBankGen.PT_THRESHOLD = 0.8;#GeV
FTKBankGen.TRAIN_PDG = 13;#Traing samples particle ID  
FTKBankGen.NEvent = event;#event number
FTKBankGen.NBanks = 64;
FTKBankGen.BarrelOnly = False;
FTKBankGen.rawMode = 1;
FTKBankGen.UseIdentifierHash = True;

FTKBankGen.par_c_max=0.0007;
FTKBankGen.par_c_min=-0.0007;
FTKBankGen.par_c_slices=250;

FTKBankGen.par_phi_max=1;
FTKBankGen.par_phi_min=-1;
FTKBankGen.par_phi_slices=100;

FTKBankGen.par_d0_max=2.2;
FTKBankGen.par_d0_min=-2.2;
FTKBankGen.par_d0_slices=250;

FTKBankGen.par_z0_max=120;
FTKBankGen.par_z0_min=-120;
FTKBankGen.par_z0_slices=100;

FTKBankGen.par_eta_max=3;
FTKBankGen.par_eta_min=-3;
FTKBankGen.par_eta_slices=100;

alg += FTKBankGen  # 1 alg, named "MyNewPackage"

#-----------------------------------------------------------------------------
# save ROOT histograms and Tuple
#-----------------------------------------------------------------------------
#from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
#Stream1 = AthenaPoolOutputStream( "Stream1" )
#Stream1.OutputFile =   "SimplePoolFile_Complete.root"

#from GaudiSvc.GaudiSvcConf import THistSvc
#ServiceMgr += THistSvc()
#ServiceMgr.THistSvc.Output = [ "MyNewPackage DATAFILE='testAug21.root' OPT='RECREATE'" ]
