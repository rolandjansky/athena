## The Following options can be specified seperately in a file that include this one
loadInDetRec_Options = {"detectorDescription" : ""
                        ,"doReadBS":False
                        ,"realData":False
                        ,"preIBLgeometry":False
                        ,"globalTag" : ""
                        ,"doTrkNtuple":True
                        ,"doMonitoring":True
                        ,"siPoolFile":""
                        ,"siAlignmentTag":""
                        ,"trtAlignmentTag":""
                        ,"trtAlignmentTagL3":""
                        ,"lorentzAngleTag":""
                        ,"beamSpotTag":""
                        ,"errorScalingTag":""
                        ,"TRTCalibTextFile":""
                        ,"TRTCalibT0TagCos":""
                        ,"TRTCalibRtTagCos":""
                        ,"numberOfEvents":30
                        ,"useTRT": True
                        ,"GoodRunList":""
                        # 7 TeV Simulation Data
                        ,"inputFiles":["root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/perf-idtracking/InDetRecExample/mc09_7TeV.105200.T1_McAtNlo_Jimmy.digit.RDO.e510_s624_s633_d287_tid112426_00/RDO.112426._000007.pool.root.1"]
                        ,"Cosmics":False
                        ,"BField":True
                        ,"SkipEvents":0
                        }


# If the variables are defined use thier values.
# If not defined the defaults given above are used
for var in loadInDetRec_Options:
  if var in dir():
    loadInDetRec_Options[var] = eval(var)

#printfunc ('HERE test ',loadInDetRec_Options["Cosmics"])
#Just make sure for now it is set. Might needs further changes in the future
if loadInDetRec_Options["preIBLgeometry"] == False:
  from IOVDbSvc.CondDB import conddb
  conddb.addOverride('/GLOBAL/TrackingGeo/LayerMaterial','AtlasLayerMat_v15_ATLAS-IBL3D25-04-00-01')


from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc.IgnoreTagDifference=True


# Good run list
if loadInDetRec_Options["GoodRunList"] != "":
  from GoodRunsLists.GoodRunsListsConf import *
  GoodRunsTool = GoodRunsListSelectorTool("GoodRunsTool") 
  GoodRunsTool.GoodRunsListVec   = [ loadInDetRec_Options["GoodRunList"] ]
  GoodRunsTool.EventSelectorMode = True
  GoodRunsTool.OutputLevel = DEBUG
  printfunc (GoodRunsTool)
  AthenaEventLoopMgr=Service("AthenaEventLoopMgr")
  AthenaEventLoopMgr.PreSelectTools+=[GoodRunsTool]
  AthenaEventLoopMgr.OutputLevel = DEBUG
  printfunc (AthenaEventLoopMgr)


# --- read BS - please look at the default input file(s) to know the det descr and cond tag
doReadBS        = loadInDetRec_Options["doReadBS"]

#--------------------------------------------------------------
# detector description version: both RDO and BS default use this
#--------------------------------------------------------------
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

athenaCommonFlags.FilesInput = loadInDetRec_Options["inputFiles"]

if not loadInDetRec_Options["Cosmics"]:
  import AthenaPython.ConfigLib as apcl
  cfg = apcl.AutoCfg(name = 'InDetRecExampleAutoConfig',
                     input_files = athenaCommonFlags.FilesInput())
  cfg.configure_job()
   
theApp.EvtMax = int(loadInDetRec_Options["numberOfEvents"])
#ServiceMgr.EventSelector.SkipEvents = int(loadInDetRec_Options["SkipEvents"])

if doReadBS:
  from AthenaCommon.GlobalFlags import globalflags
  if len(globalflags.ConditionsTag())!=0:
    from IOVDbSvc.CondDB import conddb
    conddb.setGlobalTag(globalflags.ConditionsTag())
    
    if loadInDetRec_Options["Cosmics"]:
      from AthenaCommon.AppMgr import ServiceMgr
        ##ServiceMgr Is included if SiPool is defined. If it is not defined it is not included. If doReadBS and UseCosmics are both true probably the script will crash
      ServiceMgr.PoolSvc.AttemptCatalogPatch=True
      ServiceMgr.ByteStreamInputSvc.FullFileName = loadInDetRec_Options["inputFiles"]
        
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

#-------------------------------------------------------
#Load globalFlags and set Defaults
#-------------------------------------------------------
from AthenaCommon.GlobalFlags import globalflags
##From the cosmics script this flag was set
if loadInDetRec_Options["Cosmics"]:
  globalflags.DetGeo = 'atlas'
  
if doReadBS:
  globalflags.InputFormat   = 'bytestream'
else:
  globalflags.InputFormat = 'pool'
if loadInDetRec_Options["realData"]:
  globalflags.DataSource = 'data'
else:
  globalflags.DataSource = 'geant4'

if len(loadInDetRec_Options["globalTag"])!=0:
  #from AthenaCommon.GlobalFlags import globalflags
  globalflags.ConditionsTag.set_Value_and_Lock(loadInDetRec_Options["globalTag"])

if len(loadInDetRec_Options["detectorDescription"])!=0:
  #from AthenaCommon.GlobalFlags import globalflags
  globalflags.DetDescrVersion.set_Value_and_Lock(loadInDetRec_Options["detectorDescription"])

if len(globalflags.ConditionsTag())!=0:
  from IOVDbSvc.CondDB import conddb
  conddb.setGlobalTag(globalflags.ConditionsTag())
  
globalflags.print_JobProperties()

#---------------------------------------------------------
#Setup jobProperties
#---------------------------------------------------------
from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon.BFieldFlags import jobproperties
if loadInDetRec_Options["Cosmics"]:
  jobproperties.Beam.numberOfCollisions = 0.0  
  jobproperties.Beam.beamType="cosmics"
  jobproperties.BField.solenoidOn=loadInDetRec_Options["BField"]

#------------------------------------------------------
#Set Detector Setup
#------------------------------------------------------
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

from RecExConfig.RecFlags import rec
if loadInDetRec_Options["Cosmics"]:
  rec.Commissioning = True
  rec.doMonitoring = True
else:
  rec.Commissioning = False
  ##In the original loadDetInRec_new.py for collisions the rec.doMonitoring was not set
  #rec.doMonitoring = True
    
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doTruth       = (globalflags.DataSource == 'geant4' and globalflags.InputFormat() == 'pool')
#InDetFlags.doTruth = False
if globalflags.InputFormat() == 'pool' and not globalflags.DataSource == 'geant4':
  # ---- run over ESD files
  #  InDetFlags.preProcessing=True
  #This is not originally defined in the Cosmics jobOption, but i add it here
  InDetFlags.doPRDFormation = False
else:
  InDetFlags.doPRDFormation = True

# --- uncomment to change the default of one of the following options:
#InDetFlags.doNewTracking          = False
#InDetFlags.doLowPt                = True
#InDetFlags.doBeamGas              = True
#InDetFlags.doBeamHalo             = True
#InDetFlags.doBackTracking         = False
#InDetFlags.doSingleSpBackTracking = True
#InDetFlags.doTRTStandalone        = False
#InDetFlags.doLowBetaFinder        = False

# --- Turn off track slimming
#InDetFlags.doSlimming = False

# --- possibility to run tracking on subdetectors separately (and independent from each other)
#if not Cosmics:
#    InDetFlags.doTrackSegmentsPixel = True
#    InDetFlags.doTrackSegmentsSCT   = True
 #   InDetFlags.doTrackSegmentsTRT   = True

# --- possibility to change the trackfitter
#InDetFlags.trackFitterType = 'KalmanFitter'
# --- activate monitorings
InDetFlags.doMonitoringGlobal    = False
InDetFlags.doMonitoringPixel     = False
InDetFlags.doMonitoringSCT       = False
InDetFlags.doMonitoringTRT       = False
#InDetFlags.doMonitoringAlignment = loadInDetRec_Options["doMonitoring"]
InDetFlags.doMonitoringAlignment = False
#InDetFlags.doMonitoringAlignment.set_Value_and_Lock(True)

if loadInDetRec_Options["Cosmics"]:
  InDetFlags.doCosmics.set_Value_and_Lock(True)
  #InDetFlags.doMonitoringAlignment = loadInDetRec_Options["doMonitoring"]
  #InDetFlags.doMonitoringAlignment.set_Value_and_Lock(True)
  InDetFlags.doCTBTracking.set_Value_and_Lock(False)# = False
  InDetFlags.doCTBTrackSegmentsPixel.set_Value_and_Lock(False)
  InDetFlags.doCTBTrackSegmentsSCT.set_Value_and_Lock(False)
  InDetFlags.doCTBTrackSegmentsTRT.set_Value_and_Lock(False)

# --- activate (memory/cpu) monitoring
#InDetFlags.doPerfMon        = True
# --- activate creation of standard plots
#InDetFlags.doPhysValMon  = True
# --- active storegate delection
#InDetFlags.doSGDeletion  = True

# --- produce various ntuples (all in one root file)
#InDetFlags.doTrkNtuple      = loadInDetRec_Options["doTrkNtuple"]
InDetFlags.doTrkNtuple.set_Value_and_Lock( False )
InDetFlags.doPixelTrkNtuple = loadInDetRec_Options["doTrkNtuple"]
InDetFlags.doSctTrkNtuple   = loadInDetRec_Options["doTrkNtuple"]
InDetFlags.doTrtTrkNtuple   = loadInDetRec_Options["doTrkNtuple"]
#InDetFlags.doSctClusterNtuple   = True
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

if (hasattr(InDetFlags,"doPixelClusterSplitting")) :
  InDetFlags.doPixelClusterSplitting.set_Value_and_Lock(False)

from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
TrkDetFlags.MaterialDatabaseLocal =False

# For Rel17 we need pre and post include
#--------------------------------------------------------------
# configure IBL reco
#--------------------------------------------------------------

if loadInDetRec_Options["preIBLgeometry"] == False:
  include("InDetIBL_Example/InDetIBLRecoPreInclude.py")
 
#--------------------------------------------------------------
# load master joboptions file
#--------------------------------------------------------------
 
include("InDetRecExample/InDetRec_all.py")
 
#--------------------------------------------------------------
# configure IBL reco
#--------------------------------------------------------------

if loadInDetRec_Options["preIBLgeometry"] == False:
  include("InDetIBL_Example/InDetIBLRecoPostInclude.py")


if not loadInDetRec_Options["BField"] and loadInDetRec_Options["Cosmics"]:
  InDetTrackFitter.GetMaterialFromTrack = False
  InDetTrackFitter.Momentum = 100000. *MeV


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
  printfunc ('Loading initial alignment File')
  ServiceMgr.CondProxyProvider.InputCollections = [ loadInDetRec_Options["siPoolFile"] ]
  ServiceMgr.CondProxyProvider.OutputLevel=INFO
  printfunc (ServiceMgr.CondProxyProvider)
  IOVSvc = Service("IOVSvc")
  IOVSvc.preLoadData = True

else:
  if loadInDetRec_Options["siAlignmentTag"]:
    conddb.addOverride('/Indet/Align',loadInDetRec_Options["siAlignmentTag"])
  if loadInDetRec_Options["trtAlignmentTag"]:
    conddb.addOverride('/TRT/Align',loadInDetRec_Options["trtAlignmentTag"])
  if loadInDetRec_Options["trtAlignmentTagL3"] and not loadInDetRec_Options["Cosmics"]:
    conddb.addOverride('/TRT/Calib/DX',loadInDetRec_Options["trtAlignmentTagL3"])


if loadInDetRec_Options["errorScalingTag"]:
  if ".db" in loadInDetRec_Options["errorScalingTag"]:
    conddb.blockFolder("/Indet/TrkErrorScaling")
    conddb.addFolder("","<dbConnection>sqlite://;schema="+loadInDetRec_Options["errorScalingTag"]+";dbname=OFLP200</dbConnection> /Indet/TrkErrorScaling" + "<tag>IndetTrkErrorScaling_nominal</tag>", force=True)
  else:
    conddb.addOverride('/Indet/TrkErrorScaling',loadInDetRec_Options["errorScalingTag"])
##Added if you put an empty ErrorScalingTag
else:
  conddb.addOverride('/Indet/TrkErrorScaling','IndetTrkErrorScaling_nominal')


if loadInDetRec_Options["beamSpotTag"] and not loadInDetRec_Options["Cosmics"]:
  conddb.addOverride('/Indet/Beampos',loadInDetRec_Options["beamSpotTag"])

if loadInDetRec_Options["lorentzAngleTag"] and not loadInDetRec_Options["Cosmics"]:
  conddb.addOverride('/PIXEL/LorentzAngleScale',loadInDetRec_Options["lorentzAngleTag"])
  
from AthenaCommon.AppMgr import ToolSvc
from TrkRIO_OnTrackCreator.TrkRIO_OnTrackCreatorConf import Trk__RIO_OnTrackErrorScalingTool
InDetRotErrorScalingTool = Trk__RIO_OnTrackErrorScalingTool( name        = 'RIO_OnTrackErrorScalingTool',
                                                             OutputLevel = INFO )
ToolSvc += InDetRotErrorScalingTool
printfunc (InDetRotErrorScalingTool)

  
# Correct TRT calibration for cosmics
if loadInDetRec_Options["TRTCalibTextFile"] and loadInDetRec_Options["Cosmics"]:
  from AthenaCommon.AppMgr import ToolSvc
  from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_CalDbTool
  TRTCalibDBTool=TRT_CalDbTool(name="TRT_CalDbTool")
  
  conddb.blockFolder("/TRT/Calib/RT" )
  conddb.blockFolder("/TRT/Calib/T0" )
  TRTCalibDBSvc.calibTextFile = loadInDetRec_Options["TRTCalibTextFile"]

if loadInDetRec_Options["TRTCalibT0TagCos"]:
  conddb.addOverride('/TRT/Calib/T0',loadInDetRec_Options["TRTCalibT0TagCos"])
if loadInDetRec_Options["TRTCalibRtTagCos"]:
  conddb.addOverride('/TRT/Calib/RT',loadInDetRec_Options["TRTCalibRtTagCos"])

if doJiveXML:
  ToolSvc.TrackRetriever.OtherTrackCollections =["CombinedInDetTracks", "CombinedInDetTracks_CTB"]

  
