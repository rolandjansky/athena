# The Following options can be specified seperately in a file that include this one
#  The defaults are for M8Plus cosmics
loadInDetRec_cosmics_Options = {"detectorDescription":"ATLAS-GEO-03-00-00"
								,"doReadBS":True
                                ,"realData" : True
                                ,"doReadBS":  False
                                ,"realData" :False
                                ,"globalTag" : ""
                                ,"BField":True
                                ,"doTrkNtuple":False
                                ,"doMonitoring":True
                                ,"siPoolFile":""
                                ,"siAlignmentTag":"InDet_Cosmic_2008_6"
                                ,"trtAlignmentTag":""
                                ,"errorScalingTag":""
                                ,"TRTCalibTextFile":""
                                ,"TRTCalibT0TagCos":""
                                ,"TRTCalibRtTagCos":""
                                ,"numberOfEvents":30
                                ,"SkipEvents":0
                                ,"useTRT":True
                                ,"inputFiles":['/afs/cern.ch/atlas/maxidisk/d158/alignment/InDetESD_91361_1.root'] 
                                ,"CosmicsGRL":""
                                }

# If the variables are defined use thier values.
# If not defined the defaults given above are used
for var in loadInDetRec_cosmics_Options:
  if var in dir():
    loadInDetRec_cosmics_Options[var] = eval(var)

# Good run list
if loadInDetRec_cosmics_Options["CosmicsGRL"] != "":
  from GoodRunsLists.GoodRunsListsConf import *
  GoodRunsTool = GoodRunsListSelectorTool("GoodRunsTool") 
  GoodRunsTool.GoodRunsListVec   = [ loadInDetRec_cosmics_Options["CollisionGRL"] ]
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
doReadBS        = loadInDetRec_cosmics_Options["doReadBS"] 
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
DetDescrVersion = loadInDetRec_cosmics_Options["detectorDescription"]

#--------------------------------------------------------------
# load Global Flags and set defaults (import the new jobProperty globalflags)
#--------------------------------------------------------------

from AthenaCommon.GlobalFlags import globalflags
# --- default is atlas geometry
globalflags.DetGeo = 'atlas'
# --- set defaults
# --- BS default input uses this
if loadInDetRec_cosmics_Options["doReadBS"]:
  globalflags.InputFormat   = 'bytestream'
else:
  #globalflags.DataSource = 'geant4'
  globalflags.InputFormat = 'pool'
if loadInDetRec_Options["realData"]:
  globalflags.DataSource = 'data'
else:
  globalflags.DataSource = 'geant4'

globalflags.ConditionsTag = loadInDetRec_cosmics_Options["globalTag"]
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

if not loadInDetRec_cosmics_Options["useTRT"]:
  DetFlags.TRT_setOff()
  DetFlags.detdescr.TRT_setOn()

# --- and switch off all the rest
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()

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
jobproperties.BField.solenoidOn=loadInDetRec_cosmics_Options["BField"]

# --- setup InDetJobProperties
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doTruth       = (globalflags.DataSource == 'geant4' and globalflags.InputFormat == "pool")

#InDetFlags.doCosmics=True
# --- uncomment to change the default of one of the following options:
#InDetFlags.doNewTracking  = False
#InDetFlags.doCTBTracking = False
##InDetFlags.preProcessing  = True
#InDetFlags.doLowPt        = True
#InDetFlags.doBeamGas      = True
#InDetFlags.doBeamHalo     = True
##InDetFlags.doxKalman      = False
##InDetFlags.doiPatRec      = False
#InDetFlags.doBackTracking = True
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
InDetFlags.doTrkNtuple      = loadInDetRec_cosmics_Options["doTrkNtuple"]
InDetFlags.doPixelTrkNtuple = loadInDetRec_cosmics_Options["doTrkNtuple"]
InDetFlags.doSctTrkNtuple   = loadInDetRec_cosmics_Options["doTrkNtuple"]
InDetFlags.doTrtTrkNtuple   = loadInDetRec_cosmics_Options["doTrkNtuple"]
#InDetFlags.doPixelClusterNtuple = True
#InDetFlags.doSctClusterNtuple   = True
#InDetFlags.doTrtDriftCircleNtuple = True
#InDetFlags.doVtxNtuple      = True
#InDetFlags.doConvVtxNtuple  = True
#InDetFlags.doV0VtxNtuple    = True

# activate the print InDetXYZAlgorithm statements
InDetFlags.doPrintConfigurables = True

# IMPORTANT NOTE: initialization of the flags and locking them is done in InDetRec_jobOptions.py!
# This way RecExCommon just needs to import the properties without doing anything else!
# DO NOT SET JOBPROPERTIES AFTER THIS LINE! The change will be ignored!

#--------------------------------------------------------------
# load master joboptions file
#--------------------------------------------------------------

include("InDetRecExample/InDetRec_all.py")

if not loadInDetRec_cosmics_Options["BField"]:
  InDetTrackFitter.GetMaterialFromTrack = False
  InDetTrackFitter.Momentum = 100000. *MeV

from IOVDbSvc.CondDB import conddb

if loadInDetRec_cosmics_Options["siPoolFile"]:
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
  #filename = '%s%s%s' % ( AlignConstsPath, ReadPrefixName, 'AlignmentConstants.root')
  ServiceMgr.CondProxyProvider.InputCollections = [ loadInDetRec_cosmics_Options["siPoolFile"] ]
  ServiceMgr.CondProxyProvider.OutputLevel=INFO
  print ServiceMgr.CondProxyProvider
  IOVSvc = Service("IOVSvc")
  IOVSvc.preLoadData = True

  # With Global Chi2
  #conddb.blockFolder("/Indet/Align")
  #conddb.addFolder("","<dbConnection>sqlite://X;schema=mycool.db;dbname=OFLP200</dbConnection> /Indet/Align" + "<tag>cog_tag</tag>", force=True)
else:
  if loadInDetRec_cosmics_Options["siAlignmentTag"]:
    conddb.addOverride('/Indet/Align',loadInDetRec_cosmics_Options["siAlignmentTag"])
  if loadInDetRec_cosmics_Options["trtAlignmentTag"]:
    conddb.addOverride('/TRT/Align',loadInDetRec_cosmics_Options["trtAlignmentTag"])  

 
if loadInDetRec_cosmics_Options["errorScalingTag"]:
  if ".db" in loadInDetRec_cosmics_Options["errorScalingTag"]:
    conddb.blockFolder("/Indet/TrkErrorScaling")
    conddb.addFolder("","<dbConnection>sqlite://;schema="+loadInDetRec_cosmics_Options["errorScalingTag"]+";dbname=OFLP200</dbConnection> /Indet/TrkErrorScaling" + "<tag>IndetTrkErrorScaling_nominal</tag>", force=True)
  else:
    conddb.addOverride('/Indet/TrkErrorScaling',loadInDetRec_cosmics_Options["errorScalingTag"])
else:
  conddb.addOverride('/Indet/TrkErrorScaling','IndetTrkErrorScaling_nominal')

from AthenaCommon.AppMgr import ToolSvc
from TrkRIO_OnTrackCreator.TrkRIO_OnTrackCreatorConf import Trk__RIO_OnTrackErrorScalingTool
InDetRotErrorScalingTool = Trk__RIO_OnTrackErrorScalingTool( name        = 'RIO_OnTrackErrorScalingTool',
                                                             OutputLevel = DEBUG )
ToolSvc += InDetRotErrorScalingTool
print InDetRotErrorScalingTool
if loadInDetRec_cosmics_Options["TRTCalibTextFile"]:
  from AthenaCommon.AppMgr import ToolSvc
  from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_CalDbSvc
  TRTCalibDBSvc=TRT_CalDbSvc()
  ServiceMgr += TRTCalibDBSvc
  
  conddb.blockFolder("/TRT/Calib/RT" )
  conddb.blockFolder("/TRT/Calib/T0" )
  TRTCalibDBSvc.calibTextFile = loadInDetRec_cosmics_Options["TRTCalibTextFile"]
  
if loadInDetRec_cosmics_Options["TRTCalibT0TagCos"]:
  conddb.addOverride('/TRT/Calib/T0',loadInDetRec_cosmics_Options["TRTCalibT0TagCos"])
if loadInDetRec_cosmics_Options["TRTCalibRtTagCos"]:
  conddb.addOverride('/TRT/Calib/RT',loadInDetRec_cosmics_Options["TRTCalibRtTagCos"])
#--------------------------------------------------------------
# Event related parameters and input files
#--------------------------------------------------------------

# Number of events to be processed (default is 10)
theApp.EvtMax = int(loadInDetRec_cosmics_Options["numberOfEvents"])
ServiceMgr.EventSelector.SkipEvents = int(loadInDetRec_cosmics_Options["SkipEvents"])
#ServiceMgr.StoreGateSvc.Dump = True

# hack for ATLANIS (rrashes in M8+ data)
if doJiveXML:
  ToolSvc.TrackRetriever.OtherTrackCollections =["CombinedInDetTracks", "CombinedInDetTracks_CTB"]

if not  loadInDetRec_cosmics_Options["doReadBS"]:
  ServiceMgr.PoolSvc.AttemptCatalogPatch=True
  
  # cosmic MC without field
  #ServiceMgr.EventSelector.InputCollections = ["castor:/castor/cern.ch/grid/atlas/caf/atlcal/perm/id/cosmics/ESDs/InDetESD_91900_lb3.root" ]
  ServiceMgr.EventSelector.InputCollections =loadInDetRec_cosmics_Options["inputFiles"]
  #ServiceMgr.EventSelector.InputCollections = ["/afs/cern.ch/atlas/maxidisk/d17/dig.trtbarrel_ma_B100.root"]
  #ServiceMgr.EventSelector.InputCollections = ["castor:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-simcos/rel14/ATLAS-GEO-03-00-00/dig/dig.pixelbarrel_B.400000.root"]

if  loadInDetRec_cosmics_Options["doReadBS"]:
  # Input file: ConditionsTag = 'COMCOND-ES1C-000-00'
  ServiceMgr.ByteStreamInputSvc.FullFileName = loadInDetRec_cosmics_Options["inputFiles"]
  # Input file: real data from M6, ConditionsTag = 'COMCOND-ES1C-000-00'
  #ServiceMgr.ByteStreamInputSvc.FullFileName = [ "/castor/cern.ch/grid/atlas/DAQ/M6/daq.NoTag.0043705.physics.HLT_Cosmic_AllTeIDSelected.LB0000.SFO-1._0001.data" ]
  # Real data from M8 plus (nearly complete TRT)  
  #ServiceMgr.ByteStreamInputSvc.FullFileName = [ "/castor/cern.ch/grid/atlas/DAQ/2008/80725/physics_HLT_Cosmics_NIM4/daq.ATLAS.0080725.physics.HLT_Cosmics_NIM4.LB0000.SFO-5._0011.data" ]
  #ServiceMgr.ByteStreamInputSvc.FullFileName = [ "/castor/cern.ch/grid/atlas/DAQ/2008/91890/physics_IDCosmic/daq.ATLAS.0091890.physics.IDCosmic.LB0002.SFO-1._0001.data"]

