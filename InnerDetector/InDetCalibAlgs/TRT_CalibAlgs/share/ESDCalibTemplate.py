import os,sys,time,glob,fnmatch

def ESDCalib(config,inputfiles,calibconstants,calpart):
    print "Reading from ESDCalibTemplate.py"
    ostring="""
#--------------------------------------------------------------
# control input/output   MIERDA
#--------------------------------------------------------------
readESD = True
readAOD = not readESD

# --- controls what is written out. ESD includes AOD, so its normally enough
doWriteESD = False and readESD
doWriteAOD = False

#--------------------------------------------------------------
# control algorithms to be rerun
#--------------------------------------------------------------
# --- run InDetRecStatistics (only possible if readESD = True)
doInDetRecStatistics = True and readESD
# --- refit the tracks in ESD (only possible if readESD = True)
doRefitTracks = False and readESD
# --- redo primary vertexing
reDoPrimaryVertexing = False
# --- redo particle creation (recommended after revertexing on ESD, otherwise trackparticles are inconsistent)
reDoParticleCreation = False and readESD and reDoPrimaryVertexing
# --- redo conversion finding
reDoConversions = False

#--------------------------------------------------------------
# Control - standard options (as in jobOptions.py)
#--------------------------------------------------------------
# --- Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
OutputLevel     = %s""" % config["OutputLevel"]
    ostring+="""
# --- produce an atlantis data file
doJiveXML       = False
# --- run the Virtual Point 1 event visualisation
doVP1           = False
# --- do auditors ?
doAuditors      = True

doEdmMonitor    = False
doNameAuditor   = False
 
# safety section ... redoing vertexing is a tricky business to stay consistent ...
if not (readESD or readAOD):
  print "You have to turn on reading of ESD or AOD! Thats the purpose of this jobO!"
if readESD and readAOD:
  print "I can either read ESD or AOD but not both at the same time! Turn on or the other off!"
if readESD and reDoPrimaryVertexing and not reDoParticleCreation:
  print "INFO! You are running on ESD, redoing the vertexing but not recreating the TrackParticles!"
  print "INFO! To avoid inconsistencies do not use the old track particles in conjunction with the new vertex!"
  if doWriteESD or doWriteAOD:
    print "INFO! To avoid inconsistencies the old track particle (truth) container will not be in the new ESD/AOD!"
if readAOD and reDoPrimaryVertexing:
  print "INFO! You are running on AOD, and redoing the vertexing. At the moment new track particles cannot be made from old ones."
  print "INFO! To avoid inconsistencies do not use the old track particles in conjunction with the new vertex!"
  if doWriteAOD:
    print "INFO! To avoid inconsistencies the old track particle (truth) container will not be in the new AOD!"
if doRefitTracks and (reDoPrimaryVertexing or reDoParticleCreation):
  print "INFO! You are refitting tracks and also revertex and/or recreate track particles"
  print "INFO! The input for that will be the refitted tracks!"

#--------------------------------------------------------------
# detector description version
#--------------------------------------------------------------
#DetDescrVersion = os.environ['DETECTOR_DESCRIPTION']
DetDescrVersion = 'ATLAS-CSC-02-00-00'

#--------------------------------------------------------------
# load Global Flags and set defaults (import the new jobProperty globalflags)
#--------------------------------------------------------------

from AthenaCommon.GlobalFlags import globalflags
# --- default is atlas geometry
globalflags.DetGeo = 'atlas'
# --- set defaults
globalflags.DataSource  = 'geant4'
globalflags.InputFormat = 'pool'
# --- set geometry version
globalflags.DetDescrVersion = DetDescrVersion
# --- printout
globalflags.print_JobProperties()

from AthenaCommon.BeamFlags import jobproperties
# --- default is zero luminosity
jobproperties.Beam.numberOfCollisions = 0.0

globalflags.ConditionsTag = 'OFLCOND-FDR-02-05-00'

if len(globalflags.ConditionsTag())!=0:
    from IOVDbSvc.CondDB import conddb
    conddb.setGlobalTag(globalflags.ConditionsTag())




#--------------------------------------------------------------
# Set Detector setup
#--------------------------------------------------------------

from AthenaCommon.DetFlags import DetFlags 
# --- switch on InnerDetector
DetFlags.ID_setOn()
# --- and switch off all the rest
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
# ---- switch parts of ID off/on as follows
#DetFlags.pixel_setOff()
#DetFlags.SCT_setOff()
#DetFlags.TRT_setOff()
# --- printout
DetFlags.makeRIO.pixel_setOff()
DetFlags.readRIOPool.pixel_setOff()
DetFlags.readRIOBS.pixel_setOff()

DetFlags.makeRIO.SCT_setOff()
DetFlags.readRIOPool.SCT_setOff()
DetFlags.readRIOBS.SCT_setOff()

DetFlags.Print()


#--------------------------------------------------------------
# Load Reconstruction configuration for tools only
#--------------------------------------------------------------
#--------------------------------------------------------------
# Load InDet configuration
#--------------------------------------------------------------

# --- setup InDetJobProperties
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.preProcessing      = False
InDetFlags.doNewTracking      = False
InDetFlags.doLowPt            = False
InDetFlags.doBackTracking     = False
InDetFlags.doTRTStandalone    = False
InDetFlags.doTrtSegments      = False
InDetFlags.postProcessing     = reDoPrimaryVertexing or reDoParticleCreation or reDoConversions or doInDetRecStatistics
InDetFlags.doTruth            = False
InDetFlags.doSlimming         = False
InDetFlags.loadTools          = True
InDetFlags.doVertexFinding    = reDoPrimaryVertexing
InDetFlags.doParticleCreation = reDoParticleCreation
InDetFlags.doConversions      = reDoConversions
InDetFlags.doSecVertexFinder  = reDoConversions
InDetFlags.doV0Finder         = False
InDetFlags.doTrkNtuple        = False
InDetFlags.doPixelTrkNtuple   = False
InDetFlags.doVtxNtuple        = False and readAOD # for now only possible on AOD
InDetFlags.doConvVtxNtuple    = False
InDetFlags.doRefit            = doRefitTracks
InDetFlags.doPrintConfigurables = True


# --- activate (memory/cpu) monitoring
#InDetFlags.doPerfMon = True

# IMPORTANT NOTE: initialization of the flags and locking them is done in InDetRec_jobOptions.py!
# This way RecExCommon just needs to import the properties without doing anything else!
# DO NOT SET JOBPROPERTIES AFTER THIS LINE! The change will be ignored!

# next section assures that keys are ok and nothing inconsistent is written out ...
print "InDetRec_jobOptions: InDetKeys not set - setting to defaults"
from InDetRecExample.InDetKeys import InDetKeys
if InDetFlags.doVertexFinding():
  print "reDoPrimaryVertexing enabled: adjusting some StoreGate keys:"
  
  InDetKeys.PrimaryVertices = "New"+InDetKeys.PrimaryVertices()
  print "InDetKeys.PrimaryVertices = "+InDetKeys.PrimaryVertices()
  
  if readAOD: # this one is mutally exclusive with useRefittedTrack.. and the doParticleCreation flag
    InDetKeys.Tracks = InDetKeys.TrackParticles()
    print "InDetKeys.Tracks = "+InDetKeys.Tracks()
  
  if InDetFlags.doParticleCreation():
    InDetKeys.TrackParticles      = "New"+InDetKeys.TrackParticles()
    InDetKeys.TrackParticlesTruth = "New"+InDetKeys.TrackParticlesTruth()
    print "InDetKeys.TrackParticles      = "+InDetKeys.TrackParticles()
    print "InDetKeys.TrackParticlesTruth = "+InDetKeys.TrackParticlesTruth()
  else:
    InDetKeys.TrackParticles      = "Dummy"
    InDetKeys.TrackParticlesTruth = "Dummy"

if InDetFlags.doConversions():
  InDetKeys.Conversions = "New"+InDetKeys.Conversions()
  InDetKeys.SecVertices = "New"+InDetKeys.SecVertices()
  
print "Printing InDetKeys. Be aware that some might be adjusted lateron!"
InDetKeys.print_JobProperties()

#--------------------------------------------------------------
# enable statistics for reading ESD testing
#--------------------------------------------------------------

InDetFlags.doStatistics   = doInDetRecStatistics
#InDetFlags.StatNtupleName = "InDetRecStatistics.root"
TrackCollectionKeys        = [InDetKeys.Tracks()]
TrackCollectionTruthKeys   = [InDetKeys.TracksTruth()]

#--------------------------------------------------------------
# load master joboptions file
#--------------------------------------------------------------
  
include("InDetRecExample/InDetRec_all.py")

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------

# --- Number of events to be processed (default is 10)
theApp.EvtMax  = 1

ServiceMgr.EventSelector.InputCollections = [ """
    for i in range(len(inputfiles)-1):
        ostring+='"%s", ' % inputfiles[i]
    ostring+='"%s"]\n' % inputfiles[len(inputfiles)-1]

    ostring+="""
    
#--------------------------------------------------------------
#  TRT Calibration Stuff
#--------------------------------------------------------------
from AthenaServices.AthenaServicesConf import AthenaOutputStreamTool
TRTCondStream=AthenaOutputStreamTool(name="CondStream1",OutputFile="trtcalibout.pool.root")
ToolSvc += TRTCondStream
print TRTCondStream


from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_StrawNeighbourSvc
TRTStrawNeighbourSvc=TRT_StrawNeighbourSvc()
ServiceMgr += TRTStrawNeighbourSvc

from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_CalDbTool
InDetCalDbTool=TRT_CalDbTool(name = "TRT_CalDbTool")

from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_StrawStatusSummaryTool
InDetStrawSummaryTool=TRT_StrawStatusSummaryTool(name = "TRT_StrawStatusSummaryTool",
                             isGEANT4=(globalflags.DataSource == 'geant4'))



from TRT_DriftFunctionTool.TRT_DriftFunctionToolConf import TRT_DriftFunctionTool
InDetTRT_DriftFunctionTool = TRT_DriftFunctionTool(name = "InDetTRT_DriftFunctionTool",
                                                   AllowDataMCOverride = True,
                                                   TRTCalDbTool=InDetCalDbTool,
                                                   ForceData = True,
                                                   IsMC=(globalflags.DataSource == 'geant4'))

ToolSvc += InDetTRT_DriftFunctionTool
print InDetTRT_DriftFunctionTool


from TRT_CalibTools.TRT_CalibToolsConf import FitTool
TRTCalFitTool = FitTool (name = 'TRTCalFitTool')
ToolSvc += TRTCalFitTool
print      TRTCalFitTool

from TRT_CalibTools.TRT_CalibToolsConf import FillAlignTrkInfo 
FillAlignTrkInfo = FillAlignTrkInfo ( name = 'FillAlignTrkInfo',
                                      TrackSummaryTool = InDetTrackSummaryTool)
ToolSvc += FillAlignTrkInfo
print      FillAlignTrkInfo

from TRT_CalibTools.TRT_CalibToolsConf import FillAlignTRTHits 
FillAlignTRTHits = FillAlignTRTHits ( name = 'FillAlignTRTHits', 
				      TRTCalDbSvc  = TRTCalSvc,
                                      minTimebinsOverThreshold=0,
                                      TRTCalDbTool = InDetCalDbTool,
                                      TRTStrawSummaryTool = InDetStrawSummaryTool)

ToolSvc += FillAlignTRTHits
print      FillAlignTRTHits

from TRT_DriftCircleOnTrackTool.TRT_DriftCircleOnTrackToolConf import InDet__TRT_DriftCircleOnTrackRecalibrateTool
TRT_RecalibrateRotCreator = InDet__TRT_DriftCircleOnTrackRecalibrateTool(name                = 'TRT_RecalibrateRotCreator',
                        DriftFunctionTool=InDetTRT_DriftFunctionTool,
                        ScaleHitUncertainty = 2.5) # fix from Thijs

ToolSvc += TRT_RecalibrateRotCreator
print      TRT_RecalibrateRotCreator

InDetRefitRotCreator.ToolTRT_DriftCircle = TRT_RecalibrateRotCreator

from TrkGlobalChi2Fitter.TrkGlobalChi2FitterConf import Trk__GlobalChi2Fitter
RecalibrationFitter = Trk__GlobalChi2Fitter(name = 'RecalibrationFitter',
                                                 ExtrapolationTool     = InDetExtrapolator,
                                                 NavigatorTool         = InDetNavigator,
                                                 PropagatorTool        = InDetPropagator,
                                                 RotCreatorTool        = InDetRefitRotCreator,
                                                 MeasurementUpdateTool = InDetUpdator,
                                                 StraightLine          = False,
#                                                 ExternalMat           = True,
#                                                 ExternalMatTools      = True,
                                                 OutlierCut            = 3.0,
#                                                 LayerCreatorTool      = InDetDynamicLayerCreator,
                                                 NumericalDerivs       = False,
                                                 SignedDriftRadius     = True,
#                                                 CopyCircleRIOs        = False,
                                                 ReintegrateOutliers   = True,
                                                 TrackChi2PerNDFCut    = 10.)

ToolSvc+=RecalibrationFitter
print    RecalibrationFitter

from TRT_CalibTools.TRT_CalibToolsConf import TRTCalibrator
TRTCalibrator = TRTCalibrator ( name = 'TRTCalibrator',
				DoShortStrawCorrection = False, 
                                MinRt               = %s,""" % config["MinRT"]
    ostring+="""
                                MinT0               = %s,""" % config["MinT0"]
    ostring+="""
                                Nevents             = -1,\n"""
    selstring=config["CalibArgs"]
    ostring+="%32sSelstring =  '%s'," % (" ",selstring['_'+calpart]['sel'])
    for setting in selstring['_'+calpart]['user']:
        if len(selstring['_'+calpart]['user'][setting])>0: ostring+= '\n%32s%s =  %s,'%(" ",setting,selstring['_'+calpart]['user'][setting])

    ostring+="""
                                Hittuple            = 'merged.root',
                                RtRel               = '%s',""" % config["RtRelation"]
    ostring+="""
                                UseP0               = %s,""" % config["UsePol0"]
    ostring+="""
                                FloatP3             = %s,""" % config["FloatPol3"]
    ostring+="""
                                TrtManagerLocation  = InDetKeys.TRT_Manager(),
                                NeighbourSvc=TRTStrawNeighbourSvc,
                                TRTCalDbTool = InDetCalDbTool,
                                TRTStrawSummaryTool = InDetStrawSummaryTool)

ToolSvc += TRTCalibrator
print      TRTCalibrator





from TRT_AlignAlgs.TRT_AlignAlgsConf import TRTTrackSelectionAlg
SelectTRTAlignTracks = TRTTrackSelectionAlg( name = "SelectTRTAlignTracks",
#                                             inputTrackList = "InDetCosmic_Tracks",
                                             inputTrackList = "Tracks",
                                             TrackFitter         = RecalibrationFitter,
                                             outputTrackList = "TRTCalibTracks",
                                             SummaryTool = InDetTrackSummaryTool,
                                             MinPixelHits = 0, #2, #0,
                                             MinSCTHits = 0, #6, #5,
                                             MinTRTHits = 21) #15)

topSequence += SelectTRTAlignTracks
print          SelectTRTAlignTracks

                                     
from TRT_CalibAlgs.TRT_CalibAlgsConf import TRTCalibrationMgr
CosmicsTRTCalibMgr = TRTCalibrationMgr(name                = 'CosmicsTRTCalibMgr',
                                       StreamTool          = TRTCondStream,
                                       TrkCollections      = [ 'TRTCalibTracks' ],
                                       AlignTrkTools       = [ FillAlignTrkInfo, FillAlignTRTHits ],
                                       DoCalibrate         = True,
                                       FitTools            = [ TRTCalFitTool],
                                       TrackFitter         = RecalibrationFitter,
                                       TRTCalDBTool        = TRTCalTool )

topSequence += CosmicsTRTCalibMgr
print CosmicsTRTCalibMgr

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
"""

    if not calibconstants=="":
        ostring+='TRTCondWrite.CalibInputFile="%s"\n' % (calibconstants)

    return ostring
