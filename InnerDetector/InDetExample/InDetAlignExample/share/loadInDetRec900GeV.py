# The Following options can be specified seperately in a file that include this one
#  The defaults are for MC collision
loadInDetRec_Options = {"detectorDescription" : "ATLAS-GEO-08-00-02"
                        ,"doReadBS":True
                        ,"globalTag" : ""
                        ,"doTrkNtuple":True
                        ,"doMonitoring":True
                        ,"siPoolFile":""
                        ,"siAlignmentTag":""
                        ,"trtAlignmentTag":""
                        ,"beamSpotTag":""
                        ,"errorScalingTag":""
                        ,"TRTCalibTextFile":""
                        ,"numberOfEvents":30
                        ,"useTRT": True
                        ,"inputFiles":["/afs/cern.ch/user/g/gencomm/w0/Collisions/MergeBS.run140541.LArAndMBTSInTime._0001.dat" ]
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
doReadBS        = loadInDetRec_Options["doReadBS"]
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
import AthenaCommon.SystemOfUnits as Units
from AthenaCommon.GlobalFlags import globalflags
# --- default is atlas geometry
globalflags.DetGeo = 'atlas'
# --- set defaults
globalflags.DataSource = 'data'
# --- BS default input uses this
if loadInDetRec_Options["doReadBS"]:
  globalflags.InputFormat   = 'bytestream'
else:
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
jobproperties.Beam.numberOfCollisions = 2.0 # default in RecExCommon for 900 GeV
jobproperties.Beam.energy             = 900.*Units.GeV
jobproperties.Beam.beamType           = 'collisions'
from AthenaCommon.BFieldFlags import jobproperties
jobproperties.BField.solenoidOn=True
#jobproperties.BField.solenoidOn=False
#--------------------------------------------------------------
# Set Detector setup
#--------------------------------------------------------------

from RecExConfig.RecFlags import rec
rec.Commissioning=True
rec.doMonitoring=False

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
#if globalflags.DataSource == 'data':
#DetFlags.dcs.SCT_setOff()
#DetFlags.dcs.TRT_setOff()

# --- printout
DetFlags.Print()

#--------------------------------------------------------------
# Load InDet configuration
#--------------------------------------------------------------

# --- setup InDetJobProperties
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doTruth       = (globalflags.InputFormat() == 'pool' and globalflags.DataSource == 'geant4')

# --- uncomment to change the default of one of the following options:
#InDetFlags.doNewTracking  = False
InDetFlags.doLowPt        = False
#InDetFlags.doBeamGas      = True
#InDetFlags.doBeamHalo     = True
#InDetFlags.doxKalman      = False
#InDetFlags.doiPatRec      = False
#InDetFlags.doBackTracking = False
#InDetFlags.doTRTStandalone = False
#InDetFlags.doSingleSpBackTracking = True
#InDetFlags.doRefit = True
# --- Turn off track slimming
#InDetFlags.doSlimming = False

# --- possibility to run tracking on subdetectors separately (and independent from each other)
#InDetFlags.doTrackSegmentsPixel = False
#InDetFlags.doTrackSegmentsSCT   = False
#InDetFlags.doTrackSegmentsTRT   = False

# --- possibility to change the trackfitter
#InDetFlags.trackFitterType = 'KalmanFitter'

# --- deactivate monitorings
InDetFlags.doMonitoringGlobal    = False
InDetFlags.doMonitoringPixel     = False
InDetFlags.doMonitoringSCT       = False
InDetFlags.doMonitoringTRT       = False
InDetFlags.doMonitoringAlignment = False

# --- activate (memory/cpu) monitoring
#InDetFlags.doPerfMon = True

# --- activate creation of standard plots
#InDetFlags.doStandardPlots  = True

# --- produce various ntuples (all in one root file)
InDetFlags.doTrkNtuple      = loadInDetRec_Options["doTrkNtuple"]
InDetFlags.doPixelTrkNtuple = loadInDetRec_Options["doTrkNtuple"]
InDetFlags.doSctTrkNtuple   = loadInDetRec_Options["doTrkNtuple"]
#InDetFlags.doTrtTrkNtuple   = True
InDetFlags.doPixelClusterNtuple = False
InDetFlags.doSctClusterNtuple   = False
#InDetFlags.doTrtDriftCircleNtuple = True
#InDetFlags.doVtxNtuple      = True
#InDetFlags.doConvVtxNtuple  = True
#InDetFlags.doV0VtxNtuple    = True
InDetFlags.primaryVertexCutSetup = "StartUp"

# activate the print InDetXYZAlgorithm statements
InDetFlags.doPrintConfigurables = True
#InDetFlags.useHVForSctDCS=True


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
    conddb.addFolder("","<dbConnection>sqlite://;schema="+loadInDetRec_Options["errorScalingTag"]+";dbname=OFLP200</dbConnection> /Indet/TrkErrorScaling" + "<tag>IndetTrkErrorScaling_nominal</tag>", force=True)
  else:
    conddb.addOverride('/Indet/TrkErrorScaling',loadInDetRec_Options["errorScalingTag"])
  
else:
  conddb.addOverride('/Indet/TrkErrorScaling','IndetTrkErrorScaling_nominal')

if loadInDetRec_Options["beamSpotTag"]:
  conddb.addOverride('/Indet/Beampos',loadInDetRec_Options["beamSpotTag"])
  
from AthenaCommon.AppMgr import ToolSvc
from TrkRIO_OnTrackCreator.TrkRIO_OnTrackCreatorConf import Trk__RIO_OnTrackErrorScalingTool
InDetRotErrorScalingTool = Trk__RIO_OnTrackErrorScalingTool( name        = 'RIO_OnTrackErrorScalingTool',
                                                             OutputLevel = DEBUG )
ToolSvc += InDetRotErrorScalingTool
print InDetRotErrorScalingTool

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
#ServiceMgr.EventSelector.SkipEvents = 48
#ServiceMgr.StoreGateSvc.Dump = True

if not  loadInDetRec_Options["doReadBS"]:
  # --- Minbias (DetDescrVersion = "ATLAS-GEO-08-00-01")
  ServiceMgr.PoolSvc.AttemptCatalogPatch=True
  ServiceMgr.EventSelector.InputCollections =  loadInDetRec_Options["inputFiles"]

else:
  ServiceMgr.ByteStreamInputSvc.FullFileName = loadInDetRec_Options["inputFiles"]
#InDetTrackFitter.OutputLevel=2
#ToolSvc.InDetAmbiguityProcessor.OutputLevel=1
#ToolSvc.InDetAmbiTrackSelectionTool.OutputLevel=1
#ToolSvc.InDetAmbiScoringTool.OutputLevel=1
#ToolSvc.InDetCosmicScoringTool.OutputLevel=1
#ServiceMgr.InDetSCT_DCSConditionsSvc.OutputLevel=1

