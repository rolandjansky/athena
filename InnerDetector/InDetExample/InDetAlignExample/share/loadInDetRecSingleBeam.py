loadInDetRec_Options = {"detectorDescription" : "ATLAS-GEO-03-00-00"
                        ,"doReadBS":True
                        ,"globalTag" : ""
                        ,"doTrkNtuple":True
                        ,"doMonitoring":True
                        ,"siPoolFile":""
                        ,"siAlignmentTag":""
                        ,"trtAlignmentTag":""
                        ,"errorScalingTag":""
                        ,"TRTCalibTextFile":""
                        ,"numberOfEvents":30
                        ,"useTRT": True
                        ,"inputFiles":["/afs/cern.ch/user/g/gencomm/w0/Collisions/MergeBS.run140541.LArAndMBTSInTime._0001.dat" ]
                        }
                        
# If the variables are defined use thier values.
# If not defined the defaults given above are used
for var in loadInDetRec_Options:
  if var in dir():
    loadInDetRec_Options[var] = eval(var)
    
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
# --- read BS - please look at the default input file(s) to know the det descr and cond tag
doReadBS        = True
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
DetDescrVersion =  loadInDetRec_Options["detectorDescription"]

#--------------------------------------------------------------
# load Global Flags and set defaults (import the new jobProperty globalflags)
#--------------------------------------------------------------

from AthenaCommon.GlobalFlags import globalflags
# --- default is atlas geometry
globalflags.DetGeo = 'atlas'
# --- set defaults
# --- BS default input uses this
if loadInDetRec_Options["doReadBS"]:
  globalflags.DataSource    = 'data'
  globalflags.InputFormat   = 'bytestream'
else:
  globalflags.DataSource  = 'data'
  globalflags.InputFormat = 'pool'

if loadInDetRec_Options["globalTag"]:
  globalflags.ConditionsTag = loadInDetRec_Options["globalTag"]
  
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
jobproperties.Beam.beamType           = "singlebeam"

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
DetFlags.dcs.SCT_setOff()
if not loadInDetRec_Options["useTRT"]:
  DetFlags.TRT_setOff()
  DetFlags.detdescr.TRT_setOn()
  
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
rec.doMonitoring  = False
from AthenaCommon.BFieldFlags import jobproperties
jobproperties.BField.solenoidOn = True

# --- setup InDetJobProperties
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doTruth = (globalflags.DataSource == 'geant4' and globalflags.InputFormat == "pool")

# --- Disable DCS usage
#InDetFlags.useDCS=False

# --- uncomment to change the default of one of the following options:
#InDetFlags.doNewTracking = False
#InDetFlags.doCTBTracking = False
#InDetFlags.preProcessing = True
#InDetFlags.doBeamGas     = True
#InDetFlags.doBeamHalo    = True
##InDetFlags.doxKalman    = False
##InDetFlags.doiPatRec    = False
InDetFlags.doRefit = True

# --- possibility to run tracking on subdetectors separately (and independent from each other)
#InDetFlags.doTrackSegmentsPixel = False
#InDetFlags.doTrackSegmentsSCT   = False
#InDetFlags.doTrackSegmentsTRT   = False

# --- possibility to change the trackfitter
#InDetFlags.trackFitterType = 'KalmanFitter'

# --- Turn off track slimming
#InDetFlags.doSlimming = False

# --- possibility to run tracking on subdetectors separately (and independent from each other)
# --- activate ntuple writing for IDRecStat (if it is on)
#InDetFlags.doStatNtuple    = True

# --- deactivate monitorings
InDetFlags.doMonitoringGlobal    = False
InDetFlags.doMonitoringPixel     = False
InDetFlags.doMonitoringSCT       = False
InDetFlags.doMonitoringTRT       = False
InDetFlags.doMonitoringAlignment = False

# --- activate creation of standard plots
#InDetFlags.doStandardPlots  = True

# --- produce various ntuples (all in one root file)
InDetFlags.doTrkNtuple      = loadInDetRec_Options["doTrkNtuple"]
InDetFlags.doPixelTrkNtuple = loadInDetRec_Options["doTrkNtuple"]
InDetFlags.doSctTrkNtuple   = loadInDetRec_Options["doTrkNtuple"]
#InDetFlags.doTrtTrkNtuple   = True
#InDetFlags.doPixelClusterNtuple = True
#InDetFlags.doSctClusterNtuple   = True
#InDetFlags.doTrtDriftCircleNtuple = True
#InDetFlags.doVtxNtuple      = True
#InDetFlags.doConvVtxNtuple  = True
#InDetFlags.doV0VtxNtuple    = True

# --- activate (memory/cpu) monitoring
#InDetFlags.doPerfMon = True

# activate the print InDetXYZAlgorithm statements
InDetFlags.doPrintConfigurables = True

# IMPORTANT NOTE: initialization of the flags and locking them is done in InDetRec_jobOptions.py!
# This way RecExCommon just needs to import the properties without doing anything else!
# DO NOT SET JOBPROPERTIES AFTER THIS LINE! The change will be ignored!

#--------------------------------------------------------------
# load master joboptions file
#--------------------------------------------------------------

include("InDetRecExample/InDetRec_all.py")
from IOVDbSvc.CondDB import conddb


if loadInDetRec_Options["siPoolFile"]:
  include ("DetDescrCondAthenaPool/DetDescrCondAthenaPool_joboptions.py" )
  from IOVDbSvc.CondDB import conddb
  # block folders that you want to override
  conddb.blockFolder("/Indet/Align")
  conddb.blockFolder("/TRT/Align")
  from EventSelectorAthenaPool.EventSelectorAthenaPoolConf import CondProxyProvider
  from AthenaCommon.AppMgr import ServiceMgr
  ServiceMgr += CondProxyProvider()
  ServiceMgr.ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]
  print 'Loading initial alignment File'
  ServiceMgr.CondProxyProvider.InputCollections = [ loadInDetRec_Options["siPoolFile"] ]
  ServiceMgr.CondProxyProvider.OutputLevel=INFO
  print ServiceMgr.CondProxyProvider
  IOVSvc = Service("IOVSvc")
  IOVSvc.preLoadData = True
else:
  if loadInDetRec_Options["siAlignmentTag"]:
    conddb.addOverride('/Indet/Align',loadInDetRec_Options["siAlignmentTag"])
  if loadInDetRec_Options["trtAlignmentTag"]:
    conddb.addOverride('/TRT/Align',loadInDetRec_Options["trtAlignmentTag"])



if loadInDetRec_Options["errorScalingTag"]:
  if ".db" in loadInDetRec_Options["errorScalingTag"]:
    conddb.blockFolder("/Indet/TrkErrorScaling")
    conddb.addFolder("","<dbConnection>sqlite://;schema="+loadInDetRec_Options["errorScalingTag"]+";dbname=OFLP200</dbConnection> /Indet/TrkErrorScaling" + "<tag>IndetTrkErrorScaling_cscasbuilt_00</tag>", force=True)
  else:
    conddb.addOverride('/Indet/TrkErrorScaling',loadInDetRec_Options["errorScalingTag"])
  

if loadInDetRec_Options["TRTCalibTextFile"]:
  from AthenaCommon.AppMgr import ToolSvc
  from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_CalDbSvc
  TRTCalibDBSvc=TRT_CalDbSvc()
  ServiceMgr+=TRTCalibDBSvc
  
  conddb.blockFolder("/TRT/Calib/RT" )
  conddb.blockFolder("/TRT/Calib/T0" )
  TRTCalibDBSvc.calibTextFile = loadInDetRec_Options["TRTCalibTextFile"]
#--------------------------------------------------------------
# Event related parameters and input files
#--------------------------------------------------------------

# Number of events to be processed (default is 10)
#ServiceMgr.EventSelector.SkipEvents = 20
#ServiceMgr.StoreGateSvc.Dump = True

# hack for ATLANIS (rrashes in M8+ data)
# Number of events to be processed (default is 10)
theApp.EvtMax = int(loadInDetRec_Options["numberOfEvents"])
#ServiceMgr.EventSelector.SkipEvents = 48
#ServiceMgr.StoreGateSvc.Dump = True

if not  loadInDetRec_Options["doReadBS"]:
  ServiceMgr.PoolSvc.AttemptCatalogPatch=True

  # --- Minbias (DetDescrVersion = "ATLAS-GEO-08-00-01")
  ServiceMgr.EventSelector.InputCollections =  loadInDetRec_Options["inputFiles"]
else:
  ServiceMgr.ByteStreamInputSvc.FullFileName = loadInDetRec_Options["inputFiles"]
