# The Following options can be specified seperately in a file that include this one
#  The defaults are for MC collision
loadInDetRec_Options = {"detectorDescription" : ""
                        ,"doReadBS":False
                        ,"realData" :False
                        ,"globalTag" : ""
                        ,"doTrkNtuple":False
                        ,"doMonitoring":False
                        ,"siPoolFile":""
                        ,"siAlignmentTag":""
                        ,"trtAlignmentTag":""
                        ,"beamSpotTag":""
                        ,"errorScalingTag":""
                        ,"TRTCalibTextFile":""
                        ,"numberOfEvents":30
                        ,"useTRT": True
                        ,"inputFiles":["/afs/cern.ch/atlas/maxidisk/d158/CSC.005200.T1_McAtNlo_Jimmy.RDO.pool.root" ]
                        ,"CollisionGRL":""
                        }

# If the variables are defined use thier values.
# If not defined the defaults given above are used
for var in loadInDetRec_Options:
  if var in dir():
    loadInDetRec_Options[var] = eval(var)

# Good run list
if loadInDetRec_Options["CollisionGRL"] != "":
  from GoodRunsLists.GoodRunsListsConf import *
  GoodRunsTool = GoodRunsListSelectorTool("GoodRunsTool") 
  GoodRunsTool.GoodRunsListVec   = [ loadInDetRec_Options["CollisionGRL"] ]
  GoodRunsTool.EventSelectorMode = True
  GoodRunsTool.OutputLevel = DEBUG
  print GoodRunsTool
  AthenaEventLoopMgr=Service("AthenaEventLoopMgr")
  AthenaEventLoopMgr.PreSelectTools+=[GoodRunsTool]
  AthenaEventLoopMgr.OutputLevel = DEBUG
  print AthenaEventLoopMgr
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
DetDescrVersion = loadInDetRec_Options["detectorDescription"]

#--------------------------------------------------------------
# load Global Flags and set defaults (import the new jobProperty globalflags)
#--------------------------------------------------------------

from AthenaCommon.GlobalFlags import globalflags
# --- default is atlas geometry
globalflags.DetGeo = 'atlas'
# --- set defaults
globalflags.DataSource = 'geant4'

if loadInDetRec_Options["realData"]:
  globalflags.DataSource = 'data'

# --- BS default input uses this
if loadInDetRec_Options["doReadBS"]:
  globalflags.InputFormat   = 'bytestream'
  globalflags.ConditionsTag = 'OFLCOND-CSC-00-00-00'
else:
  globalflags.InputFormat = 'pool'

# --- set geometry version
globalflags.DetDescrVersion = DetDescrVersion
# --- printout
globalflags.print_JobProperties()

if len(globalflags.ConditionsTag())!=0:
  from IOVDbSvc.CondDB import conddb
  conddb.setGlobalTag(globalflags.ConditionsTag()) 

if loadInDetRec_Options["globalTag"]:
  from IOVDbSvc.CondDB import conddb
  conddb.setGlobalTag(loadInDetRec_Options["globalTag"]) 
    
from AthenaCommon.BeamFlags import jobproperties
# --- default is zero luminosity
jobproperties.Beam.numberOfCollisions = 0.0  
if loadInDetRec_Options["doReadBS"]:
  jobproperties.Beam.numberOfCollisions = 6.9
  jobproperties.Beam.bunchSpacing       = 75

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
if not loadInDetRec_Options["useTRT"]:
  DetFlags.TRT_setOff()
  DetFlags.detdescr.TRT_setOn()

# --- switch off DCS
#DetFlags.dcs.pixel_setOff()
#DetFlags.dcs.SCT_setOff()
#DetFlags.dcs.TRT_setOff()

# --- printout
DetFlags.Print()

# --- output level
#OutputLevel          = DEBUG

#--------------------------------------------------------------
# Load InDet configuration
#--------------------------------------------------------------

# --- setup InDetJobProperties
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doTruth       = (globalflags.InputFormat() == 'pool')
InDetFlags.doTruth = False
if globalflags.InputFormat() == 'pool':
  # ---- run over ESD files
  #  InDetFlags.preProcessing=True
  InDetFlags.doPRDFormation = False
  #  InDetFlags.doSpacePointFormation = True
# --- uncomment to change the default of one of the following options:
#InDetFlags.doNewTracking          = False
InDetFlags.doLowPt                = False
#InDetFlags.doBeamGas              = True
#InDetFlags.doBeamHalo             = True
InDetFlags.doxKalman              = False
InDetFlags.doiPatRec              = False
#InDetFlags.doBackTracking         = False
#InDetFlags.doSingleSpBackTracking = True
#InDetFlags.doTRTStandalone        = False

# --- Turn off track slimming
InDetFlags.doSlimming = False

# --- possibility to run tracking on subdetectors separately (and independent from each other)
#InDetFlags.doTrackSegmentsPixel = True
#InDetFlags.doTrackSegmentsSCT   = True
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
#InDetFlags.doPerfMon        = True
# --- activate creation of standard plots
#InDetFlags.doStandardPlots  = True
# --- active storegate delection
#InDetFlags.doSGDeletion  = True

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
InDetFlags.doRefit = True
# activate the print InDetXYZAlgorithm statements
InDetFlags.doPrintConfigurables = True

ServiceMgr.StoreGateSvc.Dump = True

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

if loadInDetRec_Options["beamSpotTag"]:
  conddb.addOverride('/Indet/Beampos',loadInDetRec_Options["beamSpotTag"])  

if loadInDetRec_Options["TRTCalibTextFile"]:
  from AthenaCommon.AppMgr import ToolSvc
  from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_CalDbSvc
  TRTCalibDBSvc=TRT_CalDbSvc()
  ServiceMgr+=TRTCalibDBSvc
  
  conddb.blockFolder("/TRT/Calib/RT" )
  conddb.blockFolder("/TRT/Calib/T0" )
  TRTCalibDBSvc.calibTextFile = loadInDetRec_Options["TRTCalibTextFile"]

# --- dump MC truth into logfile
#from TruthExamples.TruthExamplesConf import DumpMC
#topSequence += DumpMC(McEventKey = "TruthEvent")

#--------------------------------------------------------------
# Event related parameters and input files
#--------------------------------------------------------------

# Number of events to be processed (default is 10)
theApp.EvtMax = int(loadInDetRec_Options["numberOfEvents"])
#ServiceMgr.EventSelector.SkipEvents = 2

if not loadInDetRec_Options["doReadBS"]:
  svcMgr.PoolSvc.AttemptCatalogPatch = True
  ServiceMgr.EventSelector.InputCollections = loadInDetRec_Options["inputFiles"]

if loadInDetRec_Options["doReadBS"]:
  ServiceMgr.ByteStreamInputSvc.FullFileName = loadInDetRec_Options["inputFiles"]
  # if you create the input BS file yourself with InDetWriteBS jobO the output will be this file
  # and you have to set the detdescr and cond tag to what you used ...
  #ServiceMgr.ByteStreamInputSvc.FullFileName = [ "daq.csc13.0000000.Single.Stream.LB0000.Athena._0001.data" ]
