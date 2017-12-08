#--------------------------------------------------------------
# Control
#--------------------------------------------------------------

# --- Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
OutputLevel     = INFO
# --- produce tracking ntuple
doTrkNtuple     = False
# --- produce pixel ntuple
doPixelTrkNtuple = False
# --- produce an atlantis data file
doJiveXML       = False
# --- run the Virtual Point 1 event visualisation
doVP1           = False
# --- controls what is written out. ESD includes AOD, so it's normally enough
doWriteESD      = False
doWriteAOD      = False
# --- read BS
doReadBS        = False
# --- do auditors ?
doAuditors      = True
# --- Tobi chaned beacause of crash otherwise
doVtxNtuple = False

import os
if os.environ['CMTCONFIG'].endswith('-dbg'):
  # --- do EDM monitor (debug mode only)
  doEdmMonitor    = True 
  # --- write out a short message upon entering or leaving each algorithm
  doNameAuditor   = True
else:
  doEdmMonitor    = False
  doNameAuditor   = False

#--------------------------------------------------------------
# detector description version
#--------------------------------------------------------------
DetDescrVersion = "ATLAS-GEO-18-01-01"

#--------------------------------------------------------------
# load Global Flags and set defaults (import the new jobProperty globalflags)
#--------------------------------------------------------------

from AthenaCommon.GlobalFlags import globalflags
# --- default is atlas geometry
globalflags.DetGeo = 'atlas'
# --- set defaults
globalflags.DataSource = 'geant4'
if 'doReadBS' in dir() and doReadBS:
  globalflags.InputFormat = 'bytestream'
else:
  globalflags.InputFormat = 'pool'
# --- set geometry version
globalflags.DetDescrVersion = DetDescrVersion
# --- printout
#globalflags.print_JobProperties()

from AthenaCommon.BeamFlags import jobproperties
# --- default is zero luminosity
jobproperties.Beam.numberOfCollisions = 0.0  

print "Setting conditions"
from AthenaCommon.GlobalFlags import jobproperties
if "AlignmentGlobalTag" in dir():
  jobproperties.Global.ConditionsTag = AlignmentGlobalTag
else:
  jobproperties.Global.ConditionsTag = "OFLCOND-SDR-BS7T-05-14"

globalflags.print_JobProperties()

#--------------------------------------------------------------
# Set Detector setup
#--------------------------------------------------------------

from AthenaCommon.DetFlags import DetFlags 
# --- switch on InnerDetector
DetFlags.ID_setOn()
# --- and switch off all the rest
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
# ---- switch parts of ID off/on as follows
#DetFlags.pixel_setOff()
#DetFlags.SCT_setOff()
#DetFlags.TRT_setOff()
# --- printout
DetFlags.Print()

#--------------------------------------------------------------
# Load InDet configuration
#--------------------------------------------------------------

# --- setup InDetJobProperties
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doTruth       = (globalflags.InputFormat() == 'pool')

# --- uncomment to change the default of one of the following options:
#InDetFlags.doNewTracking  = False
#InDetFlags.doLowPt        = True
#InDetFlags.doxKalman      = False
#InDetFlags.doiPatRec      = False
#InDetFlags.doBackTracking = False

# --- possibility to run tracking on subdetectors separately (and independent from each other)
#InDetFlags.doTrackSegmentsPixel = True
#InDetFlags.doTrackSegmentsSCT   = True
#InDetFlags.doTrackSegmentsTRT   = True

# --- possibility to change the trackfitter
#InDetFlags.trackFitterType = 'KalmanFitter'

# --- activate non default algorithms
InDetFlags.doV0Finder = True

# activate the print InDetXYZAlgorithm statements
InDetFlags.doPrintConfigurables = True

# turn off BeamConstraint for Juerg's PV histograms
InDetFlags.useBeamConstraint = False

# IMPORTANT NOTE: initialization of the flags and locking them is done in InDetRec_jobOptions.py!
# This way RecExCommon just needs to import the properties without doing anything else!
# DO NOT SET JOBPROPERTIES AFTER THIS LINE! The change will be ignored!

#--------------------------------------------------------------
# load master joboptions file
#--------------------------------------------------------------

InDetFlags.doMonitoringAlignment = True


IOVDbSvc = Service("IOVDbSvc")
#so for actually trying to recover the CSC misalignments,
#I DON'T want to use the truth info, so I go with 00
#on the other hand if I want to attempt to recover a handmade
#misalignment, then I don't want to be screwed by the CSC
#misalignment, so I DO want to use the truth database, i.e 01
#
#IOVDbSvc.GlobalTag = "OFLCOND-CSC-00-00-00"
#IOVDbSvc.GlobalTag = "OFLCOND-CSC-00-01-00"
#IOVDbSvc.GlobalTag = "OFLCOND-CSC-00-01-04" # DetectorPaper (Pawel)
if "AlignmentGlobalTag" in dir():
  IOVDbSvc.GlobalTag = AlignmentGlobalTag
else:
  IOVDbSvc.GlobalTag = "OFLCOND-SDR-BS7T-05-14"


include("InDetRecExample/InDetRec_all.py")

#--------------------------------------------------------------
# Event related parameters and input files
#--------------------------------------------------------------

# Number of events to be processed (default is 10)
if "EventMax" in dir():
  theApp.EvtMax = EventMax  
else:
  theApp.EvtMax = 20
  
if not doReadBS:
  ServiceMgr.PoolSvc.AttemptCatalogPatch=True
  ServiceMgr.EventSelector.InputCollections  = ["root://eosatlas//eos/atlas/atlascerngroupdisk/det-muon/valid/mc11_Zmumu/RDO.533482._000001.pool.root.1" ]
    
#--------------------------------------------------------------

# include this if you want to use the geometry from the database
#this is used to explicitely specify what geometry database to use for reconstruction
# Don NOT include these 2, don't know why...
#GeoModelSvc=Service("GeoModelSvc")
#GeoModelSvc.IgnoreTagDifference = True


#--------------------------------------------------------------
