# The Following options can be specified seperately in a file that include this one
loadInDetRec_Options = {"detectorDescription" : ""
                        ,"doReadBS":True
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
                        # Real ESD Cosmics
                        #,"inputFiles":["root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/perf-idtracking/InDetRecExample/data10_cos.00151040.physics_IDCosmic.recon.ESD.f220/data10_cos.00151040.physics_IDCosmic.recon.ESD.f220._lb0002._0001.1"]
                        }
#!! Just a test
#from IOVDbSvc.CondDB import conddb
#conddb.addOverride('/GLOBAL/TrackingGeo/LayerMaterial','AtlasLayerMat_v15_ATLAS-IBL3D25-04-00-01')

# If the variables are defined use thier values.
# If not defined the defaults given above are used
for var in loadInDetRec_Options:
  if var in dir():
    loadInDetRec_Options[var] = eval(var)

if len(loadInDetRec_Options["globalTag"])!=0:
	from AthenaCommon.GlobalFlags import globalflags
	globalflags.ConditionsTag.set_Value_and_Lock(loadInDetRec_Options["globalTag"])

if len(loadInDetRec_Options["detectorDescription"])!=0:
	from AthenaCommon.GlobalFlags import globalflags
	globalflags.DetDescrVersion.set_Value_and_Lock(loadInDetRec_Options["detectorDescription"])


# OUT
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc.IgnoreTagDifference=True



# Good run list
if loadInDetRec_Options["GoodRunList"] != "":
  from GoodRunsLists.GoodRunsListsConf import *
  GoodRunsTool = GoodRunsListSelectorTool("GoodRunsTool") 
  GoodRunsTool.GoodRunsListVec   = [ loadInDetRec_Options["CollisionGRL"] ]
  GoodRunsTool.EventSelectorMode = True
  GoodRunsTool.OutputLevel = DEBUG
  printfunc (GoodRunsTool)

  AthenaEventLoopMgr=Service("AthenaEventLoopMgr")
  AthenaEventLoopMgr.PreSelectTools+=[GoodRunsTool]
  AthenaEventLoopMgr.OutputLevel = DEBUG
  printfunc (AthenaEventLoopMgr)



# --- read BS - please look at the default input file(s) to know the det descr and cond tag
doReadBS        = loadInDetRec_Options["doReadBS"]
#if not doReadBS:
#	PoolSvc=Service("PoolSvc")
#	ServiceMgr += PoolSvc()
#	ServiceMgr.PoolSvc.AttemptCatalogPatch=True

#--------------------------------------------------------------
# detector description version: both RDO and BS default use this
#--------------------------------------------------------------

if "IDCosmic" in loadInDetRec_Options["inputFiles"][0]:
	printfunc ("Switched manually to cosmics")
	from AthenaCommon.BeamFlags import jobproperties
	jobproperties.Beam.beamType.set_Value_and_Lock('cosmics')
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

athenaCommonFlags.FilesInput = loadInDetRec_Options["inputFiles"]


import AthenaPython.ConfigLib as apcl
cfg = apcl.AutoCfg(name = 'InDetRecExampleAutoConfig',
                   input_files = athenaCommonFlags.FilesInput())
cfg.configure_job()
   

theApp.EvtMax = loadInDetRec_Options["numberOfEvents"]


if doReadBS:
	from AthenaCommon.GlobalFlags import globalflags
	if len(globalflags.ConditionsTag())!=0:
		from IOVDbSvc.CondDB import conddb
		conddb.setGlobalTag(globalflags.ConditionsTag())  
from AthenaCommon.BeamFlags import jobproperties
if jobproperties.Beam.beamType == 'cosmics':
	Cosmics = True
else:
	Cosmics = False

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



from RecExConfig.RecFlags import rec
if Cosmics:
    rec.Commissioning = True
    rec.doMonitoring = True
else:
    rec.Commissioning = False

 
from AthenaCommon.DetFlags import DetFlags 
# --- switch on InnerDetector
DetFlags.ID_setOn()
#DetFlags.makeRIO.pixel_setOff()
DetFlags.makeRIO.SCT_setOff()
DetFlags.makeRIO.TRT_setOff()

# --- and switch off all the rest
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()

# ---- switch parts of ID off/on as follows (always use both lines)
#DetFlags.pixel_setOff()
#DetFlags.detdescr.pixel_setOn()
DetFlags.SCT_setOff()
DetFlags.detdescr.SCT_setOn()
#if not loadInDetRec_Options["useTRT"]:
DetFlags.TRT_setOff()
DetFlags.detdescr.TRT_setOn()

# --- switch off DCS
#DetFlags.dcs.pixel_setOff()
DetFlags.dcs.SCT_setOff()
DetFlags.dcs.TRT_setOff()

# --- printout
DetFlags.Print()

# --- output level
OutputLevel          = DEBUG

  
#--------------------------------------------------------------
# Load InDet configuration
#--------------------------------------------------------------

# --- setup InDetJobProperties
from AthenaCommon.GlobalFlags import globalflags
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doTruth       = (globalflags.DataSource == 'geant4' and globalflags.InputFormat() == 'pool')
#InDetFlags.doTruth = False
#if globalflags.InputFormat() == 'pool' and not globalflags.DataSource == 'geant4':
  # ---- run over ESD files
InDetFlags.preProcessing=True
InDetFlags.doPRDFormation = True
#else:
#  InDetFlags.doPRDFormation = True

# --- uncomment to change the default of one of the following options:
InDetFlags.doNewTracking          = True
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
InDetFlags.doMonitoringAlignment = False
#InDetFlags.doMonitoringAlignment.set_Value_and_Lock(True)

# --- activate (memory/cpu) monitoring
#InDetFlags.doPerfMon        = True
# --- activate creation of standard plots
#InDetFlags.doPhysValMon  = True
# --- active storegate delection
#InDetFlags.doSGDeletion  = True

# --- produce various ntuples (all in one root file)
#InDetFlags.doTrkNtuple      = loadInDetRec_Options["doTrkNtuple"]
#InDetFlags.doTrkNtuple.set_Value_and_Lock( False )
#InDetFlags.doPixelTrkNtuple = loadInDetRec_Options["doTrkNtuple"]
#InDetFlags.doSctTrkNtuple   = loadInDetRec_Options["doTrkNtuple"]
#InDetFlags.doTrtTrkNtuple   = loadInDetRec_Options["doTrkNtuple"]
#InDetFlags.doSctClusterNtuple   = True
#InDetFlags.doVtxNtuple      = True
#InDetFlags.doConvVtxNtuple  = True
#InDetFlags.doV0VtxNtuple    = True

# activate the print InDetXYZAlgorithm statements
InDetFlags.doPrintConfigurables = True

# CHECK HERE.....
#if InDetFlags.doLowPt() or InDetFlags.doVeryLowPt():
#  InDetTrackFitter=InDetTrackFitterLowPt

# IMPORTANT NOTE: initialization of the flags and locking them is done in InDetRec_jobOptions.py!
# This way RecExCommon just needs to import the properties without doing anything else!
# DO NOT SET JOBPROPERTIES AFTER THIS LINE! The change will be ignored!

#--------------------------------------------------------------
# load master joboptions file
#--------------------------------------------------------------

InDetFlags.doPixelClusterSplitting=False
#if (hasattr(InDetFlags,"doPixelClusterSplitting")) :
InDetFlags.doPixelClusterSplitting.set_Value_and_Lock( False )

from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
TrkDetFlags.MaterialDatabaseLocal =False

include("InDetRecExample/InDetRec_all.py")

# My change
#--------------------------------------------------------------
# configure IBL reco
#--------------------------------------------------------------
printfunc ('MATTHIAS testing' )
include("InDetIBL_Example/InDetIBLRecoPreInclude.py")

#--------------------------------------------------------------
# load master joboptions file
#--------------------------------------------------------------
printfunc ('MATTHIAS testing 2' )
include("InDetRecExample/InDetRec_all.py")

 
#--------------------------------------------------------------
# configure IBL reco
#--------------------------------------------------------------
printfunc ('MATTHIAS testing 3')
include("InDetIBL_Example/InDetIBLRecoPostInclude.py")



#if (hasattr(InDetFlags,"doPixelClusterSplitting")) :
 # InDetFlags.doPixelClusterSplitting.set_Value_and_Lock( False )

#include("InDetIBL_Example/jobOptions_tracking_IBL_trf.py")


printfunc ('MATTHIAS testing 4')

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
    if loadInDetRec_Options["trtAlignmentTagL3"]:
        conddb.addOverride('/TRT/Calib/DX',loadInDetRec_Options["trtAlignmentTagL3"])


if loadInDetRec_Options["errorScalingTag"]:
    if ".db" in loadInDetRec_Options["errorScalingTag"]:
        conddb.blockFolder("/Indet/TrkErrorScaling")
        conddb.addFolder("","<dbConnection>sqlite://;schema="+loadInDetRec_Options["errorScalingTag"]+";dbname=OFLP200</dbConnection> /Indet/TrkErrorScaling" + "<tag>IndetTrkErrorScaling_nominal</tag>", force=True)
    else:
        conddb.addOverride('/Indet/TrkErrorScaling',loadInDetRec_Options["errorScalingTag"])
  
if loadInDetRec_Options["beamSpotTag"] and not Cosmics:
    conddb.addOverride('/Indet/Beampos',loadInDetRec_Options["beamSpotTag"])

if loadInDetRec_Options["lorentzAngleTag"]:
    conddb.addOverride('/PIXEL/LorentzAngleScale',loadInDetRec_Options["lorentzAngleTag"])
  
from AthenaCommon.AppMgr import ToolSvc
from TrkRIO_OnTrackCreator.TrkRIO_OnTrackCreatorConf import Trk__RIO_OnTrackErrorScalingTool
InDetRotErrorScalingTool = Trk__RIO_OnTrackErrorScalingTool( name        = 'RIO_OnTrackErrorScalingTool',
                                                             OutputLevel = INFO )
ToolSvc += InDetRotErrorScalingTool
#printfunc (InDetRotErrorScalingTool)

  
# Correct TRT calibration for cosmics
if loadInDetRec_Options["TRTCalibTextFile"] and Cosmics:
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

