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

DetDescrVersion = "ATLAS-GEO-16-00-00"
#ConditionsTag = "OFLCOND-DR-BS7T-A100-04"
ConditionsTag = "OFLCOND-SDR-BS14T-2E34-00"
#ConditionsTag = "OFLCOND-SDR-BS7T-02-02"

#-----------------------------------------------------------------------------
# Message Service
#-----------------------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
ServiceMgr.MessageSvc.OutputLevel = WARNING
import AthenaServices
AthenaServices.AthenaServicesConf.AthenaEventLoopMgr.OutputLevel = WARNING


#-----------------------------------------------------------------------------
# Input Datasets
#-----------------------------------------------------------------------------
#ServiceMgr.EventSelector.InputCollections = ['/home/volpig/ftkrelease/16.0.3/PhysicsAnalysis/AnalysisCommon/UserAnalysis/run/g4hits.pool.root']
ServiceMgr.EventSelector.InputCollections = ['/afs/cern.ch/user/k/kshohei/public/FTK/data/rdo.pool.root']

theApp.EvtMax = 1 # -1 means all events



from InDetBeamSpotService.InDetBeamSpotServiceConf import BeamCondSvc
ServiceMgr+=BeamCondSvc(useDB=False,
#                        posX=0.00,
#                        posY=0.00,
#                        posZ=0.00,
#                        sigmaX=0.000001,
#                        sigmaY=0.000001,
#                        sigmaZ=0.01,
#                        tiltX=0.0,
#                        tiltY=0.0
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
                        posX=0.0776,
                        posY=1.13,
                        posZ=12.,
                        sigmaX=0.0395,
                        sigmaY=0.0394,
                        sigmaZ=0.000025,
                        tiltX=0.0,
                        tiltY=0.0
                        )
#
# setup OutputConditionsAlg to take data in TDS and write out
from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
myOCA=OutputConditionsAlg("BeamPosWriter",outputFile="mydummy1.root",
                          ObjectList=[ "AthenaAttributeList#/Indet/Beampos" ],
                          WriteIOV=True,IOVTagList=["nominal"])

# Reconstruction setup

DetDescrVersion = "ATLAS-GEO-16-00-00"
#ConditionsTag = "OFLCOND-DR-BS7T-A100-04"
ConditionsTag = "OFLCOND-SDR-BS14T-2E34-00"
#ConditionsTag = "OFLCOND-SDR-BS7T-02-02"

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo = 'atlas'
globalflags.DataSource = 'geant4'
globalflags.InputFormat = 'pool'
globalflags.DetDescrVersion = DetDescrVersion
globalflags.ConditionsTag = ConditionsTag
globalflags.print_JobProperties()
from AthenaCommon.BeamFlags import jobproperties
jobproperties.Beam.numberOfCollisions = 0.0  

if len(globalflags.ConditionsTag())!=0:
   print 'DEBUG -- If statement entered (code=01234567)'
   from IOVDbSvc.CondDB import conddb
   conddb.setGlobalTag(globalflags.ConditionsTag()) 

#include( "./jo.ZeroedBeamWidth.py" )
    
from RecExConfig.RecFlags import rec
rec.Commissioning=False

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

# IMPORTANT NOTE: initialization of the flags and locking them is done in InDetRec_jobOptions.py!
# This way RecExCommon just needs to import the properties without doing anything else!
# DO NOT SET JOBPROPERTIES AFTER THIS LINE! The change will be ignored!
include("InDetRecExample/InDetRec_all.py")


#--------------------------------------------------------------
# FTK algorithm inclusions
#--------------------------------------------------------------
from AthenaCommon.AppMgr import ToolSvc 
from TrigFTKSim.TrigFTKSimConf import FTKRoadFinderAlgo, FTKTrackFitterAlgo, FTK_SGHitInput, FTKDetectorTool


print "Add FTK_SGHitInput tool"
FTKSGHit = FTK_SGHitInput()
ToolSvc += FTKSGHit
print "Add FTKDetector tool"
FTKDet = FTKDetectorTool()
FTKDet.doBadModuleMap = True # True to produce or False to read an existing one
FTKDet.BadSSMapPath = "../config/bad_modules/badSSmap.bmap"
FTKDet.FTK_BadModuleMapPath = "../config/bad_modules/badModulemap_FTK.bmap"
FTKDet.ATLAS_BadModuleMapPath = "../config/bad_modules/badModulemap_ATLAS.bmap"
ToolSvc += FTKDet

FTKRoadFinder = FTKRoadFinderAlgo( "FTKRoadFinderAlgo" , OutputLevel=INFO)
FTKRoadFinder.NBanks = 8
FTKRoadFinder.pmap_path = "../config/map_file/raw_7L.pmap"
FTKRoadFinder.pmapunused_path = "../config/map_file/raw_7L_unused.pmap"
FTKRoadFinder.rmap_path = "../config/map_file/raw_7L.rmap"
FTKRoadFinder.ssmap_path = "../config/ss_file/raw_24x20x36.ss"
FTKRoadFinder.ssmapunused_path = "../config/ss_file/raw_24x20x36unused.ss"
FTKRoadFinder.ssmaptsp_path = "../config/ss_file/raw_12x10x36.ss" 
FTKRoadFinder.badmap_path = "badModulemap_FTK.bmap_bk" # if = "" dont use WC 
#FTKRoadFinder.banks = ["0; bank0.patt; -1;","1; bank1.patt; 1000;"]
FTKRoadFinder.UseTSPBank = False
FTKRoadFinder.DBBankLevel = 1
FTKRoadFinder.TSPSimulationLevel = 0
FTKRoadFinder.TSPMinCoverage = 1
FTKRoadFinder.MakeCache = False
FTKRoadFinder.CachePath = "bankcache.root"
FTKRoadFinder.NBanks = 8
if FTKRoadFinder.UseTSPBank :
    #FTKRoadFinder.bankpath = ["/share/ftkdata/volpig/bank24x20x36_current/bankAM48x40x36/patt/patterns_raw_7L_24x20x36_16M_reg0_sub0.patt.db"]
    FTKRoadFinder.bankpath = ["/home/volpig/small_bank/patterns_raw_7L_24x20x36_16M_reg0_sub0.patt.db"]   #Default
else :
    #    FTKRoadFinder.bankpath = ["/home/volpig/small_bank/patterns_raw_7L_24x20x36_16M_reg0_sub0.patt.bz2"]
    FTKRoadFinder.bankpath = ["/afs/cern.ch/user/k/kshohei/public/FTK/data/user.gvolpi.000590._00001.inp0_reg0_sub0_loop0.patterns_raw_7L_24x20x36_16M_reg0_sub0.patt.bz2"]    
    #    FTKRoadFinder.bankpath = ["/share/ftkdata/volpig/bank24x20x36_current/bankAM48x40x36/patt/patterns_raw_7L_24x20x36_16M_reg0_sub0.patt"]
FTKRoadFinder.bankregion = [0]
FTKRoadFinder.bankpatterns = [6250000]
#FTKRoadFinder.bankpatterns = [10]
alg += FTKRoadFinder   # 1 alg, named "MyNewPackage"
FTKTrackFitter = FTKTrackFitterAlgo( "FTKTrackFitterAlgo" , OutputLevel=INFO)
alg += FTKTrackFitter   # 1 alg, named "MyNewPackage"
