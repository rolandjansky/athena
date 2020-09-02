import os,sys,time,glob,fnmatch

def collision(config,inputfiles,calibconstants,nevents):
    print "Reading from ESD. Track Refitting!!!"
    ostring="""
# Collision JOs; To read ESD and do track refitting. Questions to Anum.
#
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
doWriteESD      = False
doWriteAOD      = False
# --- read BS - please look at the default input file(s) to know the det descr and cond tag
doReadBS        = False
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
DetDescrVersion =  '%s'""" % config["DetDescVer"]
    ostring+="""
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
if 'doReadBS' in dir() and doReadBS:
  globalflags.InputFormat   = 'bytestream'
  globalflags.ConditionsTag = '%s'""" % config['GLtag']
    ostring+="""
else:
  globalflags.DataSource = 'data'
  globalflags.InputFormat = 'pool'

globalflags.DetDescrVersion = DetDescrVersion
globalflags.ConditionsTag =  '%s'""" % config["GLtag"]
    ostring+="""

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



# --- Autosetup of magnetic field
from AthenaCommon.BFieldFlags import jobproperties
jobproperties.BField.barrelToroidOn=  %s""" % config["barrelToroid"]
    ostring+="""
jobproperties.BField.endcapToroidOn=  %s""" % config["endcapToroid"]
    ostring+="""
jobproperties.BField.solenoidOn=  %s""" % config["Solenoid"]
    ostring+="""

#--------------------------------------------------------------
# Set Detector setup
#--------------------------------------------------------------
from RecExConfig.RecFlags import rec
rec.Commissioning=False
rec.doMonitoring=False

from AthenaCommon.DetFlags import DetFlags
# --- switch on InnerDetector
DetFlags.ID_setOn()
DetFlags.makeRIO.pixel_setOff()
DetFlags.makeRIO.SCT_setOff()
DetFlags.makeRIO.TRT_setOff()

# --- and switch off all the rest
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()

# ---- switch parts of ID off/on as follows (always use both lines)
#DetFlags.pixel_setOff()
#DetFlags.detdescr.pixel_setOn()
#DetFlags.SCT_setOff()
#DetFlags.detdescr.SCT_setOn()
#DetFlags.TRT_setOff()
#DetFlags.detdescr.TRT_setOn()

# --- switch off DCS
#DetFlags.dcs.pixel_setOff()
#if globalflags.DataSource == 'data':
#  DetFlags.dcs.SCT_setOff()
#DetFlags.dcs.TRT_setOff()

# --- printout
DetFlags.Print()

#--------------------------------------------------------------
# Load InDet configuration
#--------------------------------------------------------------

from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
TrkDetFlags.TRT_BuildStrawLayers = True

# --- setup InDetJobProperties
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doTruth       = (globalflags.InputFormat() == 'pool' and globalflags.DataSource == 'geant4')

# --- uncomment to change the default of one of the following options:
#InDetFlags.doNewTracking  = False
#InDetFlags.doLowPt        = False
#InDetFlags.doBeamGas      = True
#InDetFlags.doBeamHalo     = True
#InDetFlags.doBackTracking = False
#InDetFlags.doTRTStandalone = False
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
InDetFlags.doMonitoringGlobal    = False
InDetFlags.doMonitoringPixel     = False
InDetFlags.doMonitoringSCT       = False
InDetFlags.doMonitoringTRT       = False
InDetFlags.doMonitoringAlignment = False

# --- activate (memory/cpu) monitoring
#InDetFlags.doPerfMon = True

# --- activate creation of standard plots
#InDetFlags.doPhysValMon  = True

# --- produce various ntuples (all in one root file)
InDetFlags.doTrkNtuple      = False
InDetFlags.doPixelTrkNtuple = False
InDetFlags.doSctTrkNtuple   = False
#InDetFlags.doTrtTrkNtuple   = True
#InDetFlags.doSctClusterNtuple   = True
#InDetFlags.doVtxNtuple      = True
#InDetFlags.doConvVtxNtuple  = True
#InDetFlags.doV0VtxNtuple    = True
#InDetFlags.primaryVertexCutSetup = "StartUp"

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

#InDetTrackFitter.OutputLevel=2
#ToolSvc.InDetAmbiguityProcessor.OutputLevel=1
#ToolSvc.InDetAmbiTrackSelectionTool.OutputLevel=1
#ToolSvc.InDetAmbiScoringTool.OutputLevel=1
#ToolSvc.InDetCosmicScoringTool.OutputLevel=1
#ServiceMgr.InDetSCT_DCSConditionsSvc.OutputLevel=1

#--------------------------------------------------------------
# Event related parameters and input files
#--------------------------------------------------------------


theApp.EvtMax = %i""" % nevents
    ostring+="""

ServiceMgr.PoolSvc.AttemptCatalogPatch=True
ServiceMgr.EventSelector.InputCollections = [ """
    for i in range(len(inputfiles)-1):
        ostring+='"%s", ' % inputfiles[i]
    ostring+='"%s"]\n' % inputfiles[len(inputfiles)-1]

    ostring+="""



from AthenaCommon.AppMgr import ToolSvc

from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_CalDbTool
InDetCalDbTool=TRT_CalDbTool(name = "TRT_CalDbTool")

from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_StrawStatusSummaryTool
InDetStrawSummaryTool=TRT_StrawStatusSummaryTool(name = "TRT_StrawStatusSummaryTool",
                             isGEANT4=(globalflags.DataSource == 'geant4'))


from TRT_DriftFunctionTool.TRT_DriftFunctionToolConf import TRT_DriftFunctionTool
InDetTRT_DriftFunctionTool = TRT_DriftFunctionTool(name = "InDetTRT_DriftFunctionTool",
                                                   TRTCalDbTool=InDetCalDbTool,
                                                   IsMC=(globalflags.DataSource == 'geant4'))

ToolSvc += InDetTRT_DriftFunctionTool
print InDetTRT_DriftFunctionTool

from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_StrawNeighbourSvc
TRTStrawNeighbourSvc=TRT_StrawNeighbourSvc()
ServiceMgr += TRTStrawNeighbourSvc

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
                                      minTimebinsOverThreshold=0, 
                                      NeighbourSvc=TRTStrawNeighbourSvc,
                                      TRTDriftFunctionTool            = "InDetTRT_DriftFunctionTool",
                                      TRTCalDbTool = InDetCalDbTool,
                                      TRTStrawSummaryTool = InDetStrawSummaryTool)

ToolSvc += FillAlignTRTHits
print      FillAlignTRTHits

from TRT_DriftCircleTool.TRT_DriftCircleToolConf import InDet__TRT_DriftCircleTool

MinTrailingEdge = 16.0*ns
MaxDriftTime = 65.0*ns
LowGate = 23.0*ns
HighGate = 43.0*ns
if globalflags.DataSource == 'data':
   MinTrailingEdge = 11.0*ns
   MaxDriftTime = 60.0*ns
   LowGate = 18.0*ns
   HighGate = 38.0*ns

InDetTRT_DriftCircleTool = InDet__TRT_DriftCircleTool(name                            = "InDetTRT_DriftCircleTool",
                                    TRTDriftFunctionTool            = InDetTRT_DriftFunctionTool,
                                    TrtDescrManageLocation          = InDetKeys.TRT_Manager(),
                                    #ConditionsSummaryTool           = InDetTRTConditionsSummaryService,
                                    ConditionsSummaryTool           = InDetTRTStrawSummaryTool,
				    #HighGateArgon                   = 75.0,
				    #LowGateArgon                    = 0,
				    #MaxDriftTimeArgon               = 99.0,
				    #MinTrailingEdgeArgon            = 0,
				    #RejectIfFirstBitArgon           = False,
				    #ValidityGateSuppressionArgon    = False,
				    #UseConditionsHTStatus           = True,
                                    UseConditionsStatus             = True,
                                    SimpleOutOfTimePileupSupression = True,
                                    RejectIfFirstBit                = False, # fixes 50 nsec issue 
                                    MinTrailingEdge                 = MinTrailingEdge,
                                    MaxDriftTime                    = MaxDriftTime,
                                    ValidityGateSuppression         = False,
                                    LowGate                         = LowGate,
                                    HighGate                        = HighGate,
                                    useDriftTimeHTCorrection        = True,
                                    useDriftTimeToTCorrection       = True
) # reenable ToT

ToolSvc += InDetTRT_DriftCircleTool
print ToolSvc.InDetTRT_DriftCircleTool



from TRT_DriftCircleOnTrackTool.TRT_DriftCircleOnTrackToolConf import InDet__TRT_DriftCircleOnTrackRecalibrateTool
TRT_RecalibrateRotCreator = InDet__TRT_DriftCircleOnTrackRecalibrateTool(
                        name                    = 'TRT_RecalibrateRotCreator',
                        DriftFunctionTool      = InDetTRT_DriftFunctionTool,
#                        RIOonTrackToolTube      = ToolSvc.TRT_DriftCircleOnTrackNoDriftTimeTool,
                        ScaleHitUncertainty     = 2.5, # fix from Thijs
                        useDriftTimeToTCorrection = False)#,
ToolSvc += TRT_RecalibrateRotCreator
print      TRT_RecalibrateRotCreator

InDetRefitRotCreator.ToolTRT_DriftCircle = TRT_RecalibrateRotCreator
print InDetRefitRotCreator


RecalibrationFitter = Trk__GlobalChi2Fitter(name = 'RecalibrationFitter',
                                                 Acceleration          = True,
                                                 MaxIterations        =  40,
                                                 ExtrapolationTool     = InDetExtrapolator,
                                                 NavigatorTool         = InDetNavigator,
                                                 PropagatorTool        = InDetPropagator,
                                                 RotCreatorTool        = InDetRefitRotCreator,
                                                 BroadRotCreatorTool   = BroadInDetRotCreator,
                                                 MeasurementUpdateTool = InDetUpdator,
                                                 StraightLine          = not InDetFlags.solenoidOn(),
                                                 RecalibrateSilicon    = True,
                                                 RecalibrateTRT        = True,
                                                 OutlierCut            = 4.0,
                                                 ReintegrateOutliers   = True, # as Thijs proposed
                                                 SignedDriftRadius     = True,
                                                 TrackChi2PerNDFCut    = 7)

ToolSvc+=RecalibrationFitter
print    RecalibrationFitter


from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetDetailedTrackSelectorTool
TRTTrackSelectorTool = InDet__InDetDetailedTrackSelectorTool(name = "InDetDetailedTrackSelectorTool",
                                        pTMin                   =    1000    ,
                                        fitChi2OnNdfMax         = 50. ,
                                        z0Max                   = 300  ,
                                        nHitBLayer              = 0 ,
                                        nHitPix                 = 0 ,
                                        nHitBLayerPlusPix       = 0 ,
                                        nHitSct                 = 0 ,
                                        nHitSi                  = 4 ,
                                        nHitTrt                         = 20,
                                        nHitTrtPlusOutliers             =0,
                                        nHitTrtHighE                    =0,
                                        nHitTrtPlusOutliersHighE        =0,
                                        nHitTrtHighEFractionMax         =1,
                                        nHitTrtHighEFractionWithOutliersMax=1
                                        )

ToolSvc += TRTTrackSelectorTool
if (InDetFlags.doPrintConfigurables()):
        print TRTTrackSelectorTool


from TRT_CalibAlgs.TRT_CalibAlgsConf import TRTCalibrationMgr
CosmicsTRTCalibMgr = TRTCalibrationMgr(name                = 'CosmicsTRTCalibMgr',
                                       StreamTool          = TRTCondStream,
                                       TrackSelectorTool   = TRTTrackSelectorTool,
                                       TrkCollections      = [ 'CombinedInDetTracks' ],
                                       AlignTrkTools       = [ FillAlignTrkInfo, FillAlignTRTHits ],
                                       TrackFitter         = RecalibrationFitter,
                                       FitTools            = [ TRTCalFitTool] )

topSequence += CosmicsTRTCalibMgr
print CosmicsTRTCalibMgr



from TRT_ConditionsAlgs.TRT_ConditionsAlgsConf import TRTCondWrite
TRTCondWrite = TRTCondWrite( name = \"TRTCondWrite\")
topSequence+=TRTCondWrite 

from IOVDbSvc.CondDB import conddb
conddb.addFolder(\"PIXEL_OFL\",\"/PIXEL/PixelClustering/PixelClusNNCalib\")

# DCS Data Folders
if (globalflags.InputFormat() == 'bytestream' and globalflags.DataSource() == 'data'):
    if InDetFlags.useTrtDCS():
        conddb.addFolder('DCS_OFL',"/TRT/DCS/HV/BARREL <cache>600</cache>",classname='CondAttrListCollection')
        conddb.addFolder('DCS_OFL',"/TRT/DCS/HV/ENDCAPA <cache>600</cache>",classname='CondAttrListCollection')
        conddb.addFolder('DCS_OFL',"/TRT/DCS/HV/ENDCAPC <cache>600</cache>",classname='CondAttrListCollection')


"""

    if not calibconstants=="":
        ostring+="""
conddb.blockFolder("/TRT/Calib/RT" )
conddb.blockFolder("/TRT/Calib/T0" )
#conddb.addFolderWithTag('TRT_OFL','/TRT/Calib/errors','TrtCalibErrors-ErrorVal-00-00')
"""

    if not calibconstants=="":
        ostring+='TRTCondWrite.CalibInputFile="%s"\n' % (calibconstants)

    return ostring



