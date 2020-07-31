import os,sys,time,glob,fnmatch

def cosmicMC(config,inputfiles,calibconstants):
    
    print "Loading from CosmicMCTemplate.py"
    ostring = """


#--------------------------------------------------------------
# Control
#--------------------------------------------------------------
# --- Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
OutputLevel     = %s""" % config["OutputLevel"]
    ostring+="""
# --- produce an atlantis data file
doJiveXML       = False
# --- run the Virtual Point 1 event visualisation
doVP1           = False
# --- controls what is written out. ESD includes AOD, so it's normally enough
doWriteESD      = True
doWriteAOD      = False
# --- read BS - please look at the default input file(s) to know the det descr and cond tag
doReadBS        = False
# --- do auditors ?
doAuditors      = False

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
# detector description version: both RDO and BS default use this
#--------------------------------------------------------------
#DetDescrVersion = os.environ['DETECTOR_DESCRIPTION']
DetDescrVersion = '%s'""" % config["DetDescVer"]
    ostring+="""

#--------------------------------------------------------------
# load Global Flags and set defaults (import the new jobProperty globalflags)
#--------------------------------------------------------------

from AthenaCommon.GlobalFlags import globalflags
# --- default is atlas geometry
# --- set defaults
# --- BS default input uses this
if 'doReadBS' in dir() and doReadBS:
  globalflags.DataSource = 'data'
  globalflags.InputFormat   = 'bytestream'
  globalflags.ConditionsTag = 'COMCOND-ES1C-000-00'
else:
  globalflags.DataSource = 'data'
  globalflags.InputFormat = 'pool'

globalflags.DataSource 	= 'geant4'
globalflags.DetGeo 	= 'atlas'

# --- set geometry version
globalflags.DetDescrVersion = DetDescrVersion

# --- printout
globalflags.print_JobProperties()

if len(globalflags.ConditionsTag())!=0:
  from IOVDbSvc.CondDB import conddb
  conddb.setGlobalTag(globalflags.ConditionsTag())

#IOVDbSvc = Service("IOVDbSvc")
#IOVDbSvc.GlobalTag="OFLCOND-CSC-00-01-00"

from AthenaCommon.BeamFlags import jobproperties
# --- default is zero luminosity
jobproperties.Beam.numberOfCollisions = 0.0
jobproperties.Beam.beamType="cosmics"


#--------------------------------------------------------------
# Set Detector setup
#--------------------------------------------------------------

from AthenaCommon.DetFlags import DetFlags
# --- switch on InnerDetector
DetFlags.ID_setOn()

#DetFlags.makeRIO.pixel_setOff()
#DetFlags.readRIOPool.pixel_setOff()
#DetFlags.readRIOBS.pixel_setOff()

#DetFlags.makeRIO.SCT_setOff()
#DetFlags.readRIOPool.SCT_setOff()
#DetFlags.readRIOBS.SCT_setOff()

# --- and switch off all the rest
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()

# --- printout
DetFlags.Print()

#--------------------------------------------------------------
# Load InDet configuration
#--------------------------------------------------------------

from RecExConfig.RecFlags import rec
rec.Commissioning = True
rec.doMonitoring = True
from AthenaCommon.BFieldFlags import jobproperties
jobproperties.BField.solenoidOn=%s""" % config["MagnetOn"]
    ostring+="""

# --- setup InDetJobProperties
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doTruth       = (globalflags.DataSource == 'geant4' and globalflags.InputFormat == "pool")

#InDetFlags.doCosmics=True
# --- uncomment to change the default of one of the following options:
#InDetFlags.doNewTracking  = False
#InDetFlags.doCTBTracking = False
#InDetFlags.preProcessing  = True
#InDetFlags.doLowPt        = True
#InDetFlags.doBeamGas      = True
#InDetFlags.doBeamHalo     = True
#InDetFlags.doBackTracking = False
#InDetFlags.doTRTStandalone = False
#InDetFlags.doVertexFinding = False
#InDetFlags.doConversions = False
#InDetFlags.doSingleSpBackTracking = True

# --- Turn off track slimming
#InDetFlags.doSlimming = False

# --- possibility to run tracking on subdetectors separately (and independent from each other)
#InDetFlags.doTrackSegmentsPixel = False
#InDetFlags.doTrackSegmentsSCT   = False
#InDetFlags.doTrackSegmentsTRT   = False

# --- possibility to change the trackfitter
#InDetFlags.trackFitterType = 'KalmanFitter'

# --- deactivate monitorings
#InDetFlags.doMonitoringGlobal    = False
InDetFlags.doMonitoringPixel     = False
InDetFlags.doMonitoringSCT       = False
InDetFlags.doMonitoringTRT       = False
InDetFlags.doMonitoringAlignment = False

# --- Disable DCS usage
#InDetFlags.useDCS=False

# --- activate (memory/cpu) monitoring
#InDetFlags.doPerfMon = True

# --- activate ntuple writing for IDRecStat (if it is on)
#InDetFlags.doStatNtuple    = True


# --- activate creation of standard plots
#InDetFlags.doPhysValMon  = True

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

#--------------------------------------------------------------
# load master joboptions file
#--------------------------------------------------------------

include("InDetRecExample/InDetRec_all.py")

#--------------------------------------------------------------
# Event related parameters and input files
#--------------------------------------------------------------

# Number of events to be processed (default is 10)
theApp.EvtMax = -1
#ServiceMgr.EventSelector.SkipEvents = 20
#ServiceMgr.StoreGateSvc.Dump = True

# hack for ATLANIS (rrashes in M8+ data)
if doJiveXML:
  ToolSvc.TrackRetriever.OtherTrackCollections =["CombinedInDetTracks", "CombinedInDetTracks_CTB"]

if not doReadBS:
  ServiceMgr.PoolSvc.AttemptCatalogPatch=True
  
  # cosmic MC without field
  #ServiceMgr.EventSelector.InputCollections = ["castor:/castor/cern.ch/grid/atlas/caf/atlcal/perm/id/cosmics/ESDs/InDetESD_91900_lb3.root" ]
  ServiceMgr.EventSelector.InputCollections = ["""
    for i in range(len(inputfiles)-1):
        ostring+='"%s", ' % inputfiles[i]
    ostring+='"%s"]' % inputfiles[len(inputfiles)-1]
    ostring+="""

if doReadBS:
  ServiceMgr.EventSelector.Input = [ "some file" ]

#--------------------------------------------------------------
# Calibration stuff
#--------------------------------------------------------------

from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.AppMgr import ServiceMgr

from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_CalDbTool
InDetCalDbTool=TRT_CalDbTool(name = "TRT_CalDbTool")

from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_StrawStatusSummaryTool
InDetStrawSummaryTool=TRT_StrawStatusSummaryTool(name = "TRT_StrawStatusSummaryTool",
                             isGEANT4=(globalflags.DataSource == 'geant4'))

from TRT_CalibTools.TRT_CalibToolsConf import FitTool
TRTCalFitTool = FitTool (name = 'TRTCalFitTool')
ToolSvc += TRTCalFitTool
print      TRTCalFitTool

from AthenaServices.AthenaServicesConf import AthenaOutputStreamTool
TRTCondStream=AthenaOutputStreamTool(name="CondStream1",OutputFile="trtcalibout.pool.root")

ToolSvc += TRTCondStream
print TRTCondStream

from TRT_CalibTools.TRT_CalibToolsConf import FillAlignTrkInfo 
FillAlignTrkInfo = FillAlignTrkInfo ( name = 'FillAlignTrkInfo',
                                      TrackSummaryTool = InDetTrackSummaryTool)
ToolSvc += FillAlignTrkInfo
print      FillAlignTrkInfo

from TRT_CalibTools.TRT_CalibToolsConf import FillAlignTRTHits 
FillAlignTRTHits = FillAlignTRTHits ( name = 'FillAlignTRTHits',
                                      NeighbourSvc='TRT_StrawNeighbourSvc',
                                      TRTCalDbTool = InDetCalDbTool,
                                      TRTStrawSummaryTool = InDetStrawSummaryTool)

ToolSvc += FillAlignTRTHits
print      FillAlignTRTHits


# select good quality tracks
from TRT_AlignAlgs.TRT_AlignAlgsConf import TRTTrackSelectionAlg
SelectTRTAlignTracks = TRTTrackSelectionAlg( name = "SelectTRTAlignTracks",
#####      JOHN SELECTION:
                                                             inputTrackList = InDetKeys.TRTTracks_CTB(),
                                                             outputTrackList = 'TRTCalibTracks' ,
                                                             SummaryTool = InDetTrackSummaryTool,
                                                             MaxChisqPerDof = 1000000,
                                                             MinPixelHits = -1000,
                                                             PtMin = 2000,
                                                             MinSCTHits = -1000,
                                                             MinTRTHits = 45,
                                                             MinEndcapHits = 9999,
                                                       #      D0Max = 1000,
                                                             UseCosmicTrackSelection = False,
                                                             OneTrackPerEvent = True)#,
                                                       #      OutputLevel = INFO)

topSequence += SelectTRTAlignTracks
print          SelectTRTAlignTracks
                                        
from TRT_CalibAlgs.TRT_CalibAlgsConf import TRTCalibrationMgr
CosmicsTRTCalibMgr = TRTCalibrationMgr(name                = 'CosmicsTRTCalibMgr',
                                       StreamTool          = TRTCondStream,
                                       TrkCollections      = [ 'TRTCalibTracks' ],
                                       AlignTrkTools       = [ FillAlignTrkInfo, FillAlignTRTHits ],
                                       #AccumulatorTools    = [ TRTCalAccumulator ],
                                       TrackFitter         = InDetTrackFitter,
                                       FitTools            = [ TRTCalFitTool] )

topSequence += CosmicsTRTCalibMgr
print CosmicsTRTCalibMgr

"""

    if calibconstants=="":
        ostring+="""

#from IOVDbSvc.CondDB import conddb
#conddb.blockFolder("TRT/Calib/RT")
#conddb.blockFolder("TRT/Calib/T0")
#conddb.addOverride('/TRT/Calib/RT', 'TrtCalibRt-DC3-00')
#conddb.addOverride('/TRT/Calib/T0', 'TrtCalibT0-PitBarrelNF-00')
#InDetTRT_DriftFunctionTool.UniversalError=0.17
#InDetTRT_DriftFunctionTool.ForceUniversalErrors=True


"""

    if not calibconstants=="":

	ostring+='conddb.blockFolder("/TRT/Calib/RT" )\n'
	ostring+='conddb.blockFolder("/TRT/Calib/T0" ) \n'
        ostring+='TRTCondWrite.CalibInputFile="%s"\n' % (calibconstants)

    return ostring
