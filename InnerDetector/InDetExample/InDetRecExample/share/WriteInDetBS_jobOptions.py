#--------------------------------------------------------------
# Control
#--------------------------------------------------------------

# --- Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
OutputLevel     = INFO
# --- produce trackingg ntuple
doVP1           = False
# --- produce an atlantis data file
doJiveXML       = False
# --- IO setup
doWriteESD      = False
doWriteAOD      = False
# --- write BS
doWriteBS       = True
# --- do auditors ?
doAuditors      = False
doEdmMonitor    = False
doNameAuditor   = False

#--------------------------------------------------------------
# detector description version
#--------------------------------------------------------------
DetDescrVersion = "ATLAS-CSC-02-00-00"

#--------------------------------------------------------------
# load Global Flags and set defaults (import the new jobProperty globalflags)
#--------------------------------------------------------------

from AthenaCommon.GlobalFlags import globalflags
# --- default is atlas geometry
globalflags.DetGeo = 'atlas'
# --- set defaults
globalflags.DataSource = 'geant4'
globalflags.InputFormat = 'pool'
# --- set geometry version
globalflags.DetDescrVersion = DetDescrVersion
# --- printout
globalflags.print_JobProperties()

from AthenaCommon.BeamFlags import jobproperties
# --- default is zero luminosity
jobproperties.Beam.numberOfCollisions = 0.0

#--------------------------------------------------------------
# Set Detector setup
#--------------------------------------------------------------

from AthenaCommon.DetFlags import DetFlags 
# --- switch on InnerDetector
DetFlags.ID_setOn()
# --- and switch off all the rest
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
# ---- switch parts of ID off/on as follows (always use both lines)
#DetFlags.pixel_setOff()
#DetFlags.detdescr.pixel_setOn()
#DetFlags.SCT_setOff()
#DetFlags.detdescr.SCT_setOn()
#DetFlags.TRT_setOff()
#DetFlags.detdescr.TRT_setOn()
# --- printout
DetFlags.Print()

#--------------------------------------------------------------
# Load Reconstruction configuration for tools only
#--------------------------------------------------------------
# --- setup InDetJobProperties
from InDetRecExample.InDetJobProperties import InDetFlags

# --- to make sure explicitely turn everything off
InDetFlags.preProcessing        = False
InDetFlags.doNewTracking        = False
InDetFlags.doLowPt              = False
InDetFlags.doBeamGas            = False
InDetFlags.doBackTracking       = False
InDetFlags.doTRTStandalone      = False
InDetFlags.postProcessing       = False
InDetFlags.doParticleCreation   = False
InDetFlags.doTrackSegmentsPixel = False
InDetFlags.doTrackSegmentsSCT   = False
InDetFlags.doTrackSegmentsTRT   = False
InDetFlags.doTruth              = False
InDetFlags.loadTools            = False

# IMPORTANT NOTE: initialization of the flags and locking them cannot be postponed to
# InDetRec_jobOptions.py because doWriteBS=True uses another jobO.
# --- initialize the flags (and lock them)
InDetFlags.init()

# --- flags have been setup: print and lock the job properties (flags are print out later)
InDetFlags.printInfo()
InDetFlags.print_JobProperties()
#InDetFlags.lock_JobProperties() # this is already done in InDetFlags.init()
# DO NOT SET JOBPROPERTIES AFTER THIS LINE! The change will be ignored!

#--------------------------------------------------------------
# load master joboptions file
#--------------------------------------------------------------
  
include("InDetRecExample/InDetRec_all.py")

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------

# --- Number of events to be processed (default is 10)
theApp.EvtMax  = 10
# --- RDO file
ServiceMgr.EventSelector.InputCollections = ["/afs/cern.ch/atlas/maxidisk/d158/CSC.005200.T1_McAtNlo_Jimmy.RDO.pool.root"]

