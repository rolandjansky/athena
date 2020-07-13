#--------------------------------------------------------------
# Control
#--------------------------------------------------------------

# --- Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
OutputLevel     = INFO
# --- produce an atlantis data file
doJiveXML       = False
# --- run the Virtual Point 1 event visualisation
doVP1           = False
# --- controls what is written out. ESD includes AOD, so it's normally enough
doWriteESD      = True
doWriteAOD      = False
#
#readESD = True
# --- do auditors ?
doAuditors      = True

import os
if os.environ['CMTCONFIG'].endswith('-dbg'):
  # --- do EDM monitor (debug mode only)
  doEdmMonitor    = True 
  # --- write out a short message upon entering or leaving each algorithm
  doNameAuditor   = True
else:
  doEdmMonitor    = False
  doNameAuditor   = False

# --- enable rec.doHeavyIon
from RecExConfig.RecFlags import rec
rec.doHeavyIon=True
#rec.Commissioning=False

#--------------------------------------------------------------
# load Global Flags and set defaults (import the new jobProperty globalflags)
#--------------------------------------------------------------
import AthenaCommon.SystemOfUnits as Units
from AthenaCommon.GlobalFlags import globalflags
# --- default is atlas geometry
globalflags.DetGeo = 'atlas'
# --- set defaults
globalflags.DataSource = 'geant4'
# --- BS default input uses this
if 'doReadBS' in dir() and doReadBS:
  globalflags.InputFormat   = 'bytestream'
  globalflags.ConditionsTag = 'OFLCOND-FDR-02-01-00'
else:
  globalflags.InputFormat = 'pool'

#--------------------------------------------------------------
# detector description version: both RDO and BS default use this
#--------------------------------------------------------------
# --- set geometry version
if not ('DetDescrVersion' in dir()):
  DetDescrVersion = "ATLAS-GEO-10-00-00"

globalflags.DetDescrVersion = DetDescrVersion
  
# --- printout
globalflags.print_JobProperties()

if len(globalflags.ConditionsTag())!=0:
   from IOVDbSvc.CondDB import conddb
   conddb.setGlobalTag(globalflags.ConditionsTag()) 
    
from AthenaCommon.BeamFlags import jobproperties
# --- in Tier0 reco these flags are set from outside
jobproperties.Beam.numberOfCollisions = 10.0 # arbitrary higher number for 7+ TeV, for 900 GeV it is 2.
jobproperties.Beam.energy             = 7.*Units.TeV
jobproperties.Beam.beamType           = 'collisions'
if 'doReadBS' in dir() and doReadBS:
  jobproperties.Beam.numberOfCollisions = 6.9
  jobproperties.Beam.bunchSpacing       = 75

ServiceMgr += Service('BeamCondSvc')
beamCondSvc = ServiceMgr.BeamCondSvc
beamCondSvc.useDB = False
beamCondSvc.posX = 0.0 #0.0
beamCondSvc.posY = 0.0025 #0.004
beamCondSvc.posZ = 0.0
beamCondSvc.sigmaX = 0.013 #0.012
beamCondSvc.sigmaY = 0.013 #0.012
beamCondSvc.sigmaZ = 55.0 #55.0

#--------------------------------------------------------------
# Set Detector setup
#--------------------------------------------------------------

from RecExConfig.RecFlags import rec
rec.Commissioning=False

from AthenaCommon.DetFlags import DetFlags 
# --- switch on InnerDetector
DetFlags.ID_setOn()
#DetFlags.makeRIO.pixel_setOff()
#DetFlags.makeRIO.SCT_setOff()
#DetFlags.makeRIO.TRT_setOff()

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
InDetFlags.doTruth       = (globalflags.DataSource == 'geant4' and globalflags.InputFormat() == 'pool')

# --- uncomment to change the default of one of the following options:
#InDetFlags.doHeavyIon = True
#InDetFlags.doNewTracking  = False#####################################
#InDetFlags.doLowPt        = True
#InDetFlags.doBeamGas      = True
#InDetFlags.doBeamHalo     = True
#InDetFlags.doxKalman      = True
#InDetFlags.doiPatRec      = False
#InDetFlags.doBackTracking = False
#InDetFlags.doTRTStandalone = False
#InDetFlags.doSingleSpBackTracking = True
#InDetFlags.doV0Finder         = False
#InDetFlags.doConversions      = False
#InDetFlags.doStatistics       = False
#InDetFlags.preProcessing      = False
#InDetFlags.doPRDFormation        = False
#InDetFlags.doSpacePointFormation = False
#InDetFlags.doVertexFinding    = True
#InDetFlags.doParticleCreation = True
#InDetFlags.primaryVertexSetup = 'DefaultFastFinding'
#InDetFlags.primaryVertexCutSetup = "StartUp"
#InDetFlags.useBeamConstraint = True ###
#InDetFlags.doSingleCollisionVertexReco=True
#InDetFlags.useZvertexTool     = True # with False I don't reconstruct tracks at all, but ME suggested turning off


# --- Turn off track slimming
InDetFlags.doSlimming = False

# --- possibility to run tracking on subdetectors separately (and independent from each other)
InDetFlags.doTrackSegmentsPixel = False
#InDetFlags.doTrackSegmentsSCT   = False
#InDetFlags.doTrackSegmentsTRT   = True

# --- possibility to change the trackfitter
#InDetFlags.trackFitterType = 'KalmanFitter'

# --- activate monitorings
InDetFlags.doMonitoringGlobal    = False
InDetFlags.doMonitoringPixel     = False
InDetFlags.doMonitoringSCT       = False
InDetFlags.doMonitoringTRT       = False
InDetFlags.doMonitoringAlignment = False

# --- activate (memory/cpu) monitoring
InDetFlags.doPerfMon = True

# --- activate creation of standard plots
#InDetFlags.doPhysValMon  = True

# --- produce various ntuples (all in one root file)
#InDetFlags.doTrkNtuple      = True
#InDetFlags.doPixelTrkNtuple = True
#InDetFlags.doSctTrkNtuple   = True
#InDetFlags.doTrtTrkNtuple   = True
#InDetFlags.doSctClusterNtuple   = True
#InDetFlags.doVtxNtuple      = True
#InDetFlags.doConvVtxNtuple  = True
#InDetFlags.doV0VtxNtuple    = True

# --- produce various ntuples (all in one root file)
#InDetFlags.doTrkD3PD = True
#InDetFlags.doPixelTrkD3PD = False
#InDetFlags.doSctTrkD3PD = False
#InDetFlags.doTrtTrkD3PD = False
#InDetFlags.doVtxD3PD = True
#InDetFlags.doConvVtxD3PD = True  # NOT implemented yet
#InDetFlags.doV0VtxD3PD = True    # NOT implemented yet

# activate the print InDetXYZAlgorithm statements
InDetFlags.doPrintConfigurables = True

# IMPORTANT NOTE: initialization of the flags and locking them is done in InDetRec_jobOptions.py!
# This way RecExCommon just needs to import the properties without doing anything else!
# DO NOT SET JOBPROPERTIES AFTER THIS LINE! The change will be ignored!

#-------------------------------------------------
# change Configured parameters
#------------------------------------------------

#from InDetRecExample.ConfiguredNewTrackingCuts import ConfiguredNewTrackingCuts
#InDetNewTrackingCuts = ConfiguredNewTrackingCuts("HeavyIon")
#InDetNewTrackingCuts._ConfiguredNewTrackingCuts__minPT           = 500    #1000.0
#InDetNewTrackingCuts._ConfiguredNewTrackingCuts__Xi2max          = 15.

#--------------------------------------------------------------
# load master joboptions file
#--------------------------------------------------------------
#from InDetPriVxFinderTool.InDetPriVxFinderToolConf import InDet__InDetPriVxFinderTool
#InDetPriVxFinderTool = InDet__InDetPriVxFinderTool(name              = "InDetPriVxFinderTool")
# import TrackD3PD stuff 
include('HIInDetValidation/HIInDetTrkD3PD_jobOptions.py')

include("InDetRecExample/InDetRec_all.py")

#ToolSvc.InDetPriVxFinderTool.chi2CutMethod = 1  #2#
#ToolSvc.InDetPriVxFinderTool.maxChi2PerTrack = 15.0 ###were commented
#ToolSvc.InDetPriVxFinderTool.enableMultipleVertices = 0	###were commented

#print 'igb 1 - InDetPriVxFinderTool setup changed'
#if 'InDetPriVxFinderTool' in dir():
#  print InDetPriVxFinderTool
  
#--------------------------------------------------------------
# Event related parameters and input files
#--------------------------------------------------------------

# Number of events to be processed
if not ('EMax' in dir()):
	EMax=20
	
theApp.EvtMax = EMax


#ServiceMgr.EventSelector.SkipEvents = 2
#ServiceMgr.StoreGateSvc.Dump = True

ServiceMgr.PoolSvc.AttemptCatalogPatch=True

#ServiceMgr.EventSelector.InputCollections = ["root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/proj-sit/rtt/rtt/archive/devval/2/nightly/devval/build/i686-slc5-gcc43-opt/offline/HIInDetValidation/HIInDetValidation_Min_Bias/InDetRecESD.root/InDetRecESD.root"]
#ServiceMgr.EventSelector.InputCollections = ["root://castoratlas//castor/cern.ch/grid/atlas/atlasgroupdisk/phys-hi/dq2/mc09_7TeV/RDO/e511_s780_s767_d313/mc09_7TeV.208801.Hijing_PbPb_2p75TeV_MinBias.digit.RDO.e511_s780_s767_d313_tid144736_00/RDO.144736._000001.pool.root.1","root://castoratlas//castor/cern.ch/grid/atlas/atlasgroupdisk/phys-hi/dq2/mc09_7TeV/RDO/e511_s780_s767_d313/mc09_7TeV.208801.Hijing_PbPb_2p75TeV_MinBias.digit.RDO.e511_s780_s767_d313_tid144736_00/RDO.144736._000002.pool.root.1", "root://castoratlas//castor/cern.ch/grid/atlas/atlasgroupdisk/phys-hi/dq2/mc09_7TeV/RDO/e511_s780_s767_d313/mc09_7TeV.208801.Hijing_PbPb_2p75TeV_MinBias.digit.RDO.e511_s780_s767_d313_tid144736_00/RDO.144736._000003.pool.root.1", "root://castoratlas//castor/cern.ch/grid/atlas/atlasgroupdisk/phys-hi/dq2/mc09_7TeV/RDO/e511_s780_s767_d313/mc09_7TeV.208801.Hijing_PbPb_2p75TeV_MinBias.digit.RDO.e511_s780_s767_d313_tid144736_00/RDO.144736._000004.pool.root.1", "root://castoratlas//castor/cern.ch/grid/atlas/atlasgroupdisk/phys-hi/dq2/mc09_7TeV/RDO/e511_s780_s767_d313/mc09_7TeV.208801.Hijing_PbPb_2p75TeV_MinBias.digit.RDO.e511_s780_s767_d313_tid144736_00/RDO.144736._000005.pool.root.1", "root://castoratlas//castor/cern.ch/grid/atlas/atlasgroupdisk/phys-hi/dq2/mc09_7TeV/RDO/e511_s780_s767_d313/mc09_7TeV.208801.Hijing_PbPb_2p75TeV_MinBias.digit.RDO.e511_s780_s767_d313_tid144736_00/RDO.144736._000006.pool.root.1", "root://castoratlas//castor/cern.ch/grid/atlas/atlasgroupdisk/phys-hi/dq2/mc09_7TeV/RDO/e511_s780_s767_d313/mc09_7TeV.208801.Hijing_PbPb_2p75TeV_MinBias.digit.RDO.e511_s780_s767_d313_tid144736_00/RDO.144736._000007.pool.root.1", "root://castoratlas//castor/cern.ch/grid/atlas/atlasgroupdisk/phys-hi/dq2/mc09_7TeV/RDO/e511_s780_s767_d313/mc09_7TeV.208801.Hijing_PbPb_2p75TeV_MinBias.digit.RDO.e511_s780_s767_d313_tid144736_00/RDO.144736._000008.pool.root.1", "root://castoratlas//castor/cern.ch/grid/atlas/atlasgroupdisk/phys-hi/dq2/mc09_7TeV/RDO/e511_s780_s767_d313/mc09_7TeV.208801.Hijing_PbPb_2p75TeV_MinBias.digit.RDO.e511_s780_s767_d313_tid144736_00/RDO.144736._000009.pool.root.1", "root://castoratlas//castor/cern.ch/grid/atlas/atlasgroupdisk/phys-hi/dq2/mc09_7TeV/RDO/e511_s780_s767_d313/mc09_7TeV.208801.Hijing_PbPb_2p75TeV_MinBias.digit.RDO.e511_s780_s767_d313_tid144736_00/RDO.144736._000010.pool.root.1", "root://castoratlas//castor/cern.ch/grid/atlas/atlasgroupdisk/phys-hi/dq2/mc09_7TeV/RDO/e511_s780_s767_d313/mc09_7TeV.208801.Hijing_PbPb_2p75TeV_MinBias.digit.RDO.e511_s780_s767_d313_tid144736_00/RDO.144736._000011.pool.root.1"]
