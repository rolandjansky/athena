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
doWriteESD      = False
doWriteAOD      = False
# --- read BS - please look at the default input file(s) to know the det descand cond tag
doReadBS        =  Myoption_ReadBStream
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

#--------------------------------------------------------------
# detector description version: both RDO and BS default use this
#--------------------------------------------------------------
DetDescrVersion = My_DetectorDescription
	
#--------------------------------------------------------------
# load Global Flags and set defaults (import the new jobProperty globalflags)
#--------------------------------------------------------------
from AthenaCommon.GlobalFlags import globalflags
# --- default is atlas geometry
globalflags.DetGeo = 'atlas'
# --- set defaults
# --- BS default input uses this

if 'doReadBS' in dir() and doReadBS:
  globalflags.DataSource = 'data'
  globalflags.InputFormat   = 'bytestream'
  globalflags.ConditionsTag = My_ConditionsTag #'COMCOND-ES1C-000-00'
else:
  globalflags.DataSource = 'data'
  #globalflags.DataSource = 'geant4'
  globalflags.InputFormat = 'pool'
	
# --- set geometry version
globalflags.DetDescrVersion = DetDescrVersion
# --- printout
globalflags.print_JobProperties()
	
if len(globalflags.ConditionsTag())!=0:
  from IOVDbSvc.CondDB import conddb
  conddb.setGlobalTag(globalflags.ConditionsTag())
	   
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
#DetFlags.readRDOBS.pixel_setOff()

#DetFlags.makeRIO.SCT_setOff()
#DetFlags.readRIOPool.SCT_setOff()
#DetFlags.readRIOBS.SCT_setOff()
#DetFlags.readRDOBS.SCT_setOff()
	
#DetFlags.TRT_setOff()
#DetFlags.detdescr.TRT_setOn()

# --- and switch off all the rest
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()

if not doTRT:
  DetFlags.TRT_setOff()
  DetFlags.detdescr.TRT_setOn()

# --- switch off DCS
#DetFlags.dcs.pixel_setOff()
#DetFlags.dcs.SCT_setOff()
#DetFlags.dcs.TRT_setOff()
# --- printout
DetFlags.Print()

#--------------------------------------------------------------
# Load InDet configuration
#--------------------------------------------------------------
	
from RecExConfig.RecFlags import rec
rec.Commissioning = True
rec.doMonitoring = True
from AthenaCommon.BFieldFlags import jobproperties
jobproperties.BField.solenoidOn= Magnet
	
# --- setup InDetJobProperties
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doTruth       = (globalflags.DataSource == 'geant4' and globalflags.InputFormat == "pool")
	
#InDetFlags.doCosmics=True
# --- uncomment to change the default of one of the following options:
#InDetFlags.doNewTracking  = False
InDetFlags.doCTBTracking = True
##InDetFlags.preProcessing  = True
#InDetFlags.doLowPt        = True
#InDetFlags.doBeamGas      = True
#InDetFlags.doBeamHalo     = True
##InDetFlags.doxKalman      = False
##InDetFlags.doiPatRec      = False
#InDetFlags.doBackTracking = True

if doTRTConstraint:
  InDetFlags.doBackTracking       = False
  InDetFlags.doTRTStandalone      = False
  InDetFlags.doTrackSegmentsTRT   = False

#InDetFlags.doTRTStandalone = True
##InDetFlags.doVertexFinding=False
##InDetFlags.doConversions=False
#InDetFlags.doSingleSpBackTracking = True

# --- Turn off track slimming
#InDetFlags.doSlimming = False
	
# --- possibility to run tracking on subdetectors separately (and independent from each other)
#InDetFlags.doCTBTrackSegmentsPixel = False
#InDetFlags.doCTBTrackSegmentsSCT   = False
#InDetFlags.doCTBTrackSegmentsTRT   = False
#InDetFlags.doCTBTrackSegmentsPixel = True
#InDetFlags.doCTBTrackSegmentsSCT   = True
#InDetFlags.doCTBTrackSegmentsTRT   = True

# --- possibility to change the trackfitter
#InDetFlags.trackFitterType = 'KalmanFitter'

# --- deactivate monitorings
InDetFlags.doMonitoringGlobal    = False
InDetFlags.doMonitoringPixel     = False
InDetFlags.doMonitoringSCT       = False
InDetFlags.doMonitoringTRT       = False
InDetFlags.doMonitoringAlignment = False
	

# --- Disable DCS usage
#InDetFlags.useDCS=False
	
# --- activate (memory/cpu) monitoring
#InDetFlags.doPerfMon = True
	
# --- activate creation of standard plots
#InDetFlags.doStandardPlots  = True
	
# --- produce various ntuples (all in one root file)
InDetFlags.doTrkNtuple      = False
InDetFlags.doPixelTrkNtuple = False
InDetFlags.doSctTrkNtuple   = False
InDetFlags.doTrtTrkNtuple   = False
InDetFlags.doPixelClusterNtuple = False
InDetFlags.doSctClusterNtuple   = False
InDetFlags.doTrtDriftCircleNtuple = False
InDetFlags.doVtxNtuple      = False
InDetFlags.doConvVtxNtuple  = False
InDetFlags.doV0VtxNtuple    = False
	
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
theApp.EvtMax = EvtMax
ServiceMgr.EventSelector.SkipEvents = SkipEvents
#ServiceMgr.StoreGateSvc.Dump = True
	
# hack for ATLANIS (rrashes in M8+ data)
if doJiveXML:
  ToolSvc.TrackRetriever.OtherTrackCollections =["CombinedInDetTracks", "CombinedInDetTracks_CTB"]
	
if not doReadBS:
  ServiceMgr.PoolSvc.AttemptCatalogPatch=True
  ServiceMgr.EventSelector.InputCollections = InDetAlignExampleFlags.Files
  # cosmic MC without field
  #ServiceMgr.EventSelector.InputCollections = ["castor:/castor/cern.ch/grid/atlas/caf/atlcal/perm/id/cosmics/ESDs/InDetESD_91900_lb3.root" ]
  #ServiceMgr.EventSelector.InputCollections = ["castor:/castor/cern.ch/grid/atlas/tzero/prod1/perm/data08_cosmag/physics_IDCosmic/0091890/data08_cosmag.00091890.physics_IDCosmic.recon.ESD.o4_f73/data08_cosmag.00091890.physics_IDCosmic.recon.ESD.o4_f73._lb0001._sfo01._0001.1"]
  #ServiceMgr.EventSelector.InputCollections = ["/afs/cern.ch/atlas/maxidisk/d17/dig.trtbarrel_ma_B100.root"]
  #ServiceMgr.EventSelector.InputCollections = ["castor:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-simcos/rel14/ATLAS-GEO-03-00-00/dig/dig.pixelbarrel_B.400000.root"]
  
if doReadBS:
  ServiceMgr.ByteStreamInputSvc.FullFileName = InDetAlignExampleFlags.Files
  # Input file: ConditionsTag = 'COMCOND-ES1C-000-00'
  #ServiceMgr.ByteStreamInputSvc.FullFileName = [ "/castor/cern.ch/grid/atlas/DAQ/2008/90272/physics_IDCosmic/daq.ATLAS.0090272.physics.IDCosmic.LB0001.SFO-1._0001.data" ]
  # Input file: real data from M6, ConditionsTag = 'COMCOND-ES1C-000-00'
  #ServiceMgr.ByteStreamInputSvc.FullFileName = [ "/castor/cern.ch/grid/atlas/DAQ/M6/daq.NoTag.0043705.physics.HLT_Cosmic_AllTeIDSelected.LB0000.SFO-1._0001.data" ]
  # Real data from M8 plus (nearly complete TRT) 
  #ServiceMgr.ByteStreamInputSvc.FullFileName = [ "/castor/cern.ch/grid/atlas/DAQ/2008/80725/physics_HLT_Cosmics_NIM4/daq.ATLAS.0080725.physics.HLT_Cosmics_NIM4.LB0000.SFO-5._0011.data" ]
  #ServiceMgr.ByteStreamInputSvc.FullFileName = [ "/castor/cern.ch/grid/atlas/DAQ/2008/91890/physics_IDCosmic/daq.ATLAS.0091890.physics.IDCosmic.LB0002.SFO-1._0001.data"]










