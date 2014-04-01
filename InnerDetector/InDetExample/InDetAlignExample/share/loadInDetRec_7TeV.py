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
# --- in Tier0 reco these flags are set from outside
#Let number of collisions default to 0.0 since input file doesn't include pileup 
#jobproperties.Beam.numberOfCollisions = 10.0 # arbitrary higher number for 7+ TeV, for 900 GeV it is 2.
jobproperties.Beam.energy             = 7.*Units.TeV
jobproperties.Beam.beamType           = 'collisions'
if 'doReadBS' in dir() and doReadBS:
#  jobproperties.Beam.numberOfCollisions = 6.9
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
InDetFlags.doTruth       = (globalflags.DataSource == 'geant4' and globalflags.InputFormat() == 'pool')

# --- uncomment to change the default of one of the following options:
#InDetFlags.doNewTracking          = False
#InDetFlags.doLowPt                = True
#InDetFlags.doBeamGas              = True
#InDetFlags.doBeamHalo             = True
InDetFlags.doxKalman              = True
InDetFlags.doiPatRec              = True
#InDetFlags.doBackTracking         = False
#InDetFlags.doSingleSpBackTracking = True
#InDetFlags.doTRTStandalone        = False
#InDetFlags.doLowBetaFinder        = False

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
#InDetFlags.doPerfMon        = True
# --- activate creation of standard plots
#InDetFlags.doStandardPlots  = True
# --- active storegate delection
#InDetFlags.doSGDeletion  = True

# --- produce various ntuples (all in one root file)
InDetFlags.doTrkNtuple      = loadInDetRec_Options["doTrkNtuple"]
InDetFlags.doPixelTrkNtuple = loadInDetRec_Options["doTrkNtuple"]
InDetFlags.doSctTrkNtuple   = loadInDetRec_Options["doTrkNtuple"]
InDetFlags.doTrtTrkNtuple   = loadInDetRec_Options["doTrkNtuple"]
#InDetFlags.doPixelClusterNtuple = True
#InDetFlags.doSctClusterNtuple   = True
#InDetFlags.doTrtDriftCircleNtuple = True
#InDetFlags.doVtxNtuple      = True
#InDetFlags.doConvVtxNtuple  = True
#InDetFlags.doV0VtxNtuple    = True

# --- produce various ntuples (all in one root file)
#InDetFlags.doTrkD3PD = True
#InDetFlags.doPixelTrkD3PD = True
#InDetFlags.doSctTrkD3PD = True
#InDetFlags.doTrtTrkD3PD = True
#InDetFlags.doVtxD3PD = True
#InDetFlags.doConvVtxD3PD = True  # NOT implemented yet
#InDetFlags.doV0VtxD3PD = True    # NOT implemented yet

#InDetFlags.doMinBias   = True

# activate the print InDetXYZAlgorithm statements
InDetFlags.doPrintConfigurables = True

# IMPORTANT NOTE: initialization of the flags and locking them is done in InDetRec_jobOptions.py!
# This way RecExCommon just needs to import the properties without doing anything else!
# DO NOT SET JOBPROPERTIES AFTER THIS LINE! The change will be ignored!

#--------------------------------------------------------------
# load master joboptions file
#--------------------------------------------------------------

include("InDetRecExample/InDetRec_all.py")

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
  


if loadInDetRec_Options["beamSpotTag"]:
  conddb.addOverride('/Indet/Beampos',loadInDetRec_Options["beamSpotTag"])
  
from AthenaCommon.AppMgr import ToolSvc
from TrkRIO_OnTrackCreator.TrkRIO_OnTrackCreatorConf import Trk__RIO_OnTrackErrorScalingTool
InDetRotErrorScalingTool = Trk__RIO_OnTrackErrorScalingTool( name        = 'RIO_OnTrackErrorScalingTool',
                                                             OutputLevel = INFO )
ToolSvc += InDetRotErrorScalingTool
#print InDetRotErrorScalingTool

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
#ServiceMgr.StoreGateSvc.Dump = True

if not doReadBS:
  ServiceMgr.PoolSvc.AttemptCatalogPatch=True

  # --- t tbar channel 5200 (DetDescrVersion = "ATLAS-DC3-07")
  # ServiceMgr.EventSelector.InputCollections = ["/afs/cern.ch/atlas/maxidisk/d38/ID_ValidationData/testIdeal_07.005200.T1_McAtNlo_Jimmy.digit.RDO.v12000201_tid002760._00002.pool.root.1"]
  # --- TP split version (local) - (DetDescrVersion = "ATLAS-DC3-07")
  #ServiceMgr.EventSelector.InputCollections = ["/afs/cern.ch/atlas/maxidisk/d89/InDetRecRDO.root"]
  # --- Minbias (DetDescrVersion = "ATLAS-GEO-08-00-01")
  #ServiceMgr.EventSelector.InputCollections = ["root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/perf-idtracking/InDetRecExample/mc09_900GeV.105001.pythia_minbias.digit.RDO.e466_s604_s582_d224_tid097817/RDO.097817._000001.pool.root.1"]
  # --- t tbar channel 5200 (DetDescrVersion = "ATLAS-CSC-02-00-00")
  ServiceMgr.EventSelector.InputCollections = loadInDetRec_Options["inputFiles"]
  #ServiceMgr.EventSelector.InputCollections = ["/afs/cern.ch/atlas/maxidisk/d89/InDetRecRDO.root"]
  # --- gamma 20 (ATLAS-CSC-01-00-00)
  #ServiceMgr.EventSelector.InputCollections = ["castor:/castor/cern.ch/grid/atlas/datafiles/egamma/DC3.007040.singlepart_gamma_Et20/digit/120031/ideal0_mc12.007040.singlepart_gamma_Et20.digit.RDO.v12003108_tid005022._00001.pool.root"]
  #ServiceMgr.EventSelector.InputCollections += ["castor:/castor/cern.ch/grid/atlas/datafiles/egamma/DC3.007040.singlepart_gamma_Et20/digit/120031/ideal0_mc12.007040.singlepart_gamma_Et20.digit.RDO.v12003108_tid005022._00002.pool.root"]
  # --- black hole event
  #ServiceMgr.EventSelector.InputCollections = ["castor:/castor/cern.ch/user/j/jgoncalo/valid12003108/misal1_mc12.006640.CharybdisJimmy.digit.RDO.v12003108_tid006066._00038.pool.root.4"]
  # --- event with low lumi pile up (4.6 min bias, 2*10^33), should be ATLAS-CSC-01-02-00
  #ServiceMgr.EventSelector.InputCollections = ["castor:/castor/cern.ch/user/k/kgrybel/pileup/CSC/005850.pyt_WH120_munubb/digits/misal1_csc11.005850.WH120bb_pythia.digit.RDOS.v12003103_lumi02._00001.pool.root"]

if doReadBS:
  # input used for FDR2b (run 8): low lumi (2.3 min bias, 10^33)/pile-up/shifted vertex run 8
  # this file needs (as set by default): DetDescrVersion = "ATLAS-CSC-02-00-00" globalflags.ConditionsTag="OFLCOND-FDR-02-01-00", jobproperties.Beam.numberOfCollisions = 2.3
  ServiceMgr.ByteStreamInputSvc.FullFileName = loadInDetRec_Options["inputFiles"]
  # if you create the input BS file yourself with InDetWriteBS jobO the output will be this file
  # and you have to set the detdescr and cond tag to what you used ...
  #ServiceMgr.ByteStreamInputSvc.FullFileName = [ "daq.csc13.0000000.Single.Stream.LB0000.Athena._0001.data" ]
