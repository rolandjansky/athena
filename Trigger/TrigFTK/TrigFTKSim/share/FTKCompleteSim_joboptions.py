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
ServiceMgr.EventSelector.InputCollections = ['rdo.pool.root']

#theApp.EvtMax = 5000 # -1 means all events
theApp.EvtMax = 20 # -1 means all events



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
                          ObjectList=[ "AthenaAttributeList#/Indet/Beampos"] )

# Reconstruction setup

DetDescrVersion = "ATLAS-GEO-16-00-00"
#ConditionsTag = "OFLCOND-DR-BS7T-A100-04"
ConditionsTag = "OFLCOND-SDR-BS14T-2E34-00"

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
from TrigFTKSim.TrigFTKSimConf import *

print "Add FTK_SGHitInput tool"
FTKSGHit = FTK_SGHitInput()
ToolSvc += FTKSGHit
print "Add FTK_SGRoadOutput tool"
FTKSGRoadOut = FTK_SGRoadOutput()
ToolSvc += FTKSGRoadOut
print "Add FTK_SGTrackOutput tool"
FTKSGTrackOut = FTK_SGTrackOutput(OutputLevel=DEBUG)
FTKSGTrackOut.StoreIncomplete = True
ToolSvc += FTKSGTrackOut
print "Add FTK_RoadMarketTool"
FTKRoadMarket = FTK_RoadMarketTool()
FTKRoadMarket.SaveRoads = True
ToolSvc += FTKRoadMarket
#FTKSGHit.OutputLevel=DEBUG
print "Add FTKDetector tool"
FTKDet = FTKDetectorTool()
FTKDet.doBadModuleMap = False # True to produce or False to read an existing one
FTKDet.useBadModuleMap = False # may disable the use of dead modules mapping
FTKDet.BadSSMapPath = "badSSmap.bmap"
FTKDet.FTK_BadModuleMapPath = "badModulemap_FTK.bmap"
FTKDet.ATLAS_BadModuleMapPath = "badModulemap_ATLAS.bmap"
ToolSvc += FTKDet
#FTKDet.OutputLevel=DEBUG

FTKRoadFinder = FTKRoadFinderAlgo( "FTKRoadFinderAlgo" , OutputLevel=INFO)
FTKRoadFinder.NBanks = 8
FTKRoadFinder.NSubRegions = 1
FTKRoadFinder.pmap_path = "../config/map_file/raw_7L.pmap"
FTKRoadFinder.pmapunused_path = "../config/map_file/raw_7L_unused.pmap"
FTKRoadFinder.rmap_path = "../config/map_file/raw_7L.rmap"
FTKRoadFinder.ssmap_path = "../config/ss_file/raw_50x64x144.ss"
FTKRoadFinder.ssmapunused_path = "../config/ss_file/raw_24x20x36unused.ss"
#FTKRoadFinder.badmap_path = "badModulemap_FTK.bmap" # if = "" dont use WC 
#FTKRoadFinder.ssmaptsp_path = "../config/ss_file/raw_12x10x36.ss" 
#FTKRoadFinder.banks = ["0; bank0.patt; -1;","1; bank1.patt; 1000;"]
FTKRoadFinder.UseTSPBank = False
FTKRoadFinder.DBBankLevel = 1
FTKRoadFinder.TSPSimulationLevel = 0
FTKRoadFinder.TSPMinCoverage = 1
FTKRoadFinder.MakeCache = False
FTKRoadFinder.CachePath = "bankcache.root"
FTKRoadFinder.DoRoadFile = False
FTKRoadFinder.SaveRoads = False

if FTKRoadFinder.UseTSPBank :
    #FTKRoadFinder.bankpath = ["/share/ftkdata/volpig/bank24x20x36_current/bankAM48x40x36/patt/patterns_raw_7L_24x20x36_16M_reg0_sub0.patt.db"]
    FTKRoadFinder.bankpath = ["/home/volpig/small_bank/patterns_raw_7L_24x20x36_16M_reg"+str(FTKRegion)+"_sub"+str(FTKSubRegion)+".patt.db"]   #Default
else :    #    FTKRoadFinder.bankpath = ["/home/volpig/small_bank/patterns_raw_7L_24x20x36_16M_reg0_sub0.patt.bz2"]
    #    FTKRoadFinder.bankpath = ["/afs/cern.ch/user/c/chiba/data/user.gvolpi.000590._00001.inp0_reg0_sub0_loop0.patterns_raw_7L_24x20x36_16M_reg0_sub0.patt.bz2"]
    FTKRoadFinder.bankpath = [];
    for reg in xrange(0,8):
       FTKRoadFinder.bankpath.append("baseDS_v16/patt/pattgen_raw_7L_50x64x144_1M_reg"+str(reg)+"_sub0.patt.bz2")

FTKRoadFinder.bankregion = range(0,8)
FTKRoadFinder.bankpatterns = []
for reg in xrange(0,8):
   FTKRoadFinder.bankpatterns.append(2000000)
   
alg += FTKRoadFinder   # 1 alg, named "MyNewPackage"


#-------------------   Track Fitter   -------------------#
FTKTrackFitter = FTKTrackFitterAlgo( "FTKTrackFitterAlgo" , OutputLevel=INFO)
FTKTrackFitter.NBanks = 8
FTKTrackFitter.NSubRegions = 1
FTKTrackFitter.pmap_path = "../config/map_file/raw_7L.pmap"

#set bank path 
FTKTrackFitter.bankpath = []
FTKTrackFitter.bankregion = []
FTKTrackFitter.banksubregion = []
for reg in xrange(0,8) :
   FTKTrackFitter.bankpath.append("baseDS_v16/gcon/corrgen_raw_7L_1M_reg"+str(reg)+"_sub0.gcon.bz2")
   FTKTrackFitter.bankregion.append(reg)
   FTKTrackFitter.banksubregion.append(0)

## set parameter ##
FTKTrackFitter.Chi2Cut         = 17 #Default 17
FTKTrackFitter.Chi2Cut_Maj     = 14 #Default 0
FTKTrackFitter.Chi2Cut_VetoMaj = 0 #Default 0
FTKTrackFitter.Chi2DofCutAux   = 4 #Default 0
FTKTrackFitter.Chi2DofCutSSB   = 4 #Default 0

FTKTrackFitter.NCoords        = 10 # 10 for 7L, 11 for 8L, 14 for 11L
FTKTrackFitter.HitWarrior     = 2  #Default 2
FTKTrackFitter.KeepRejected   = 0 # >0 enable HW debug feature
FTKTrackFitter.FitRemoved     = 0 # >0 enable debug feature
FTKTrackFitter.HWNDiff        = 4 # number of coordinates to define a not match (HW uses inverse logic)
FTKTrackFitter.MaxNcomb       = 1000 # maximum number of combinations to fit in  1 road
FTKTrackFitter.MaxTrkout      = 2000000 # maximum number of output tracks in a sub-region
FTKTrackFitter.loadHWConf_path = "../config/hwsdev_file/raw_10.hw" # HW thresholds
FTKTrackFitter.doTrackFile = False
FTKTrackFitter.OutDir= "./" #default ./

alg += FTKTrackFitter   # 1 alg, named "MyNewPackage"

#-------------------   Merger   -------------------#
FTKMerger = FTKMergerAlgo( "FTKMergerAlgo" , OutputLevel=DEBUG)
FTKMerger.force_merge     = True # true only for tests or well known reason
FTKMerger.useStandalone   = False
FTKMerger.SingleProces    = True
FTKMerger.doMerging       = True
FTKMerger.pmap_path       = "../config/map_file/raw_7L.pmap"
FTKMerger.NRegions        = 8
FTKMerger.NSubRegions     = 1
#Set Hit Warrior parameter
FTKMerger.NCoords         = 10 # 10 for 7L, 11 for 8L, 14 for 11L
FTKMerger.HitWarrior      = 2  # Default 2
FTKMerger.KeepRejected    = 0  # >0 enable HW debug feature
FTKMerger.HWSDEV_FILE     = "../config/hwsdev_file/raw_10.hw" # HW thresholds

alg += FTKMerger
 
# Stream's output file
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream( "Stream1" )
Stream1.OutputFile =   "SimplePoolFile_Complete.root"
# Optionally, one can explicitly set the list of Items to write
# Format:  ["<clid>#<key>]
if not FTKRoadFinder.DoRoadFile and FTKRoadFinder.SaveRoads :
   Stream1.ItemList   += ["FTKTestData#*"]
if not FTKTrackFitter.doTrackFile :
   Stream1.ItemList += ["FTKAthTrackContainer#*","FTKTrackFitterStats#*","TrigInDetTrackCollection#*"]
