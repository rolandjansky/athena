## The Following options can be specified seperately in a file that include this one
printfunc (" <loadInDetRec> Starting loadInDet_Rec")

if loadInDetRec_Options["realData"]:    
  rec.projectName="Cosm14"

#Just make sure for now it is set. Might needs further changes in the future


printfunc ("conddb loaded")
  
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


printfunc ("Before Autoconfig")


if not loadInDetRec_Options["Cosmics"]:
  import AthenaPython.ConfigLib as apcl
  cfg = apcl.AutoCfg(name = 'InDetRecExampleAutoConfig',
                     input_files = athenaCommonFlags.FilesInput())
  cfg.configure_job()
 

   
theApp.EvtMax = int(loadInDetRec_Options["numberOfEvents"])
#ServiceMgr.EventSelector.SkipEvents = int(loadInDetRec_Options["SkipEvents"])



#-------------------------------------------------------
#Load globalFlags and set Defaults
#-------------------------------------------------------
from AthenaCommon.GlobalFlags import globalflags
##From the cosmics script this flag was set
if loadInDetRec_Options["Cosmics"]:
  #globalflags.DetGeo = 'atlas'
  globalflags.DetGeo='commis' #PF 2014/25/10
  
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


printfunc ("INFO:: JobProperties Printing ")
globalflags.print_JobProperties()





if loadInDetRec_Options["preIBLgeometry"] == False:
  from IOVDbSvc.CondDB import conddb
  #conddb.addOverride('/GLOBAL/TrackingGeo/LayerMaterial','AtlasLayerMat_v15_ATLAS-IBL3D25-04-00-01')
  conddb.addOverride('/GLOBAL/TrackingGeo/LayerMaterialV2','AtlasLayerMat_v18_ATLAS-R2-2015-01')

if doReadBS:
  #ByteStreamInputSvc this is needed for 2014 cosmic ray data
  from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamEventStorageInputSvc
  ByteStreamInputSvc = ByteStreamEventStorageInputSvc("ByteStreamInputSvc",
                                                        FullFileName = ["RawEvent.re"])
  ServiceMgr            += ByteStreamInputSvc
  printfunc               (ByteStreamInputSvc)
  
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
printfunc ("INFO:: Printing DetFlags")
DetFlags.Print()

# --- output level
#OutputLevel          = DEBUG

#--------------------------------------------------------------
# Load InDet configuration
#--------------------------------------------------------------

from RecExConfig.RecFlags import rec
if loadInDetRec_Options["Cosmics"]:
  rec.Commissioning = True 
  #rec.doMonitoring = True
else:
  rec.Commissioning = False
  ##In the original loadDetInRec_new.py for collisions the rec.doMonitoring was not set
  #rec.doMonitoring = True


##Implementing x277 requirements  PF 26/10/2014
#rec.doJetMissingETTag=False  -> Not needed for this Jo since it properly set the Calo off
rec.doTau=False
from TrigHLTMonitoring.HLTMonFlags import HLTMonFlags
HLTMonFlags.doBphys=False
from AthenaMonitoring import DQMonFlags
DQMonFlags.doJetMon=False
DQMonFlags.doMissingEtMon=False
DQMonFlags.enableLumiAccess=False
DQMonFlags.doStreamAwareMon=False
from JetRec.JetRecFlags import jetFlags
jetFlags.useTracks=False


    
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
#InDetFlags.doMonitoringAlignment = loadInDetRec_Options["doMonitoring"]
InDetFlags.doMonitoringAlignment = False
#InDetFlags.doMonitoringAlignment.set_Value_and_Lock(True)

if loadInDetRec_Options["Cosmics"]:
  InDetFlags.doCosmics.set_Value_and_Lock(True)
  #InDetFlags.doMonitoringAlignment = loadInDetRec_Options["doMonitoring"]
  #InDetFlags.doMonitoringAlignment.set_Value_and_Lock(True)
  
#CTBTracking not anymore in release 19
 
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
InDetFlags.doPrintConfigurables = False

#Commissioning Flags
InDetFlags.useBroadClusterErrors = False
InDetFlags.doTrackSegmentsPixel = False 
InDetFlags.doTrackSegmentsSCT   = False 
InDetFlags.doTrackSegmentsTRT   = False

#if (hasattr(InDetFlags,"doPixelClusterSplitting")) :
#  InDetFlags.doPixelClusterSplitting=False
#conddb.addOverride("/PIXEL/PixelClustering/PixelClusNNCalib","PixClusNNCalib-IBL3D25-00-00-02")
InDetFlags.doPixelClusterSplitting.set_Value_and_Lock(False);


# IMPORTANT NOTE: initialization of the flags and locking them is done in InDetRec_jobOptions.py!
# This way RecExCommon just needs to import the properties without doing anything else!
# DO NOT SET JOBPROPERTIES AFTER THIS LINE! The change will be ignored!

#--------------------------------------------------------------
# load master joboptions file
#--------------------------------------------------------------


from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
TrkDetFlags.MaterialDatabaseLocal =False



# no clue about that one at moment!!!
import MagFieldServices.SetupField

#another hack!!
#from PixelGeoModel.PixelGeoModelConf import PixelDetectorTool
#pixelTool           = PixelDetectorTool()
#pixelTool.Alignable = True


#--------------------------------------------------------------
# load master joboptions file
#--------------------------------------------------------------

printfunc ('before Rec all')
 
include("InDetRecExample/InDetRec_all.py")
 
printfunc ('after Rec all')
