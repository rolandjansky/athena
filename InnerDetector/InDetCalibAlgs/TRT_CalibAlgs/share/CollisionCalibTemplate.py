import os,sys,time,glob,fnmatch

def collisioncalib(config,inputfiles,calibconstants,calpart,calsettings):
#    print "Loading from CalibCollisionTemplate.py"
#    print "Based on Thjis jO: "
    ostring="""
# Collision JOs
#
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
doReadBS        = False
###doReadBS        = True
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
InDetFlags.doTrkNtuple      = True
InDetFlags.doPixelTrkNtuple = True
InDetFlags.doSctTrkNtuple   = True
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

# Number of events to be processed (default is 10)
theApp.EvtMax = 1
#ServiceMgr.EventSelector.SkipEvents = 2
#ServiceMgr.StoreGateSvc.Dump = True

if not doReadBS:
  ServiceMgr.PoolSvc.AttemptCatalogPatch=True
  ServiceMgr.EventSelector.InputCollections = ["""
    for i in range(len(inputfiles)-1):
        ostring+='"%s", ' % inputfiles[i]
    ostring+='"%s"]' % inputfiles[len(inputfiles)-1]
    ostring+="""

#--------------------------------------------------------------
# Calibration stuff
#--------------------------------------------------------------

from AthenaCommon.AppMgr import ToolSvc

from AthenaCommon.AppMgr import ServiceMgr


from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_StrawNeighbourSvc
TRTStrawNeighbourSvc=TRT_StrawNeighbourSvc()
ServiceMgr += TRTStrawNeighbourSvc

from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_CalDbTool
InDetCalDbTool=TRT_CalDbTool(name = "TRT_CalDbTool")

from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_StrawStatusSummaryTool
InDetStrawSummaryTool=TRT_StrawStatusSummaryTool(name = "TRT_StrawStatusSummaryTool",
                             isGEANT4=(globalflags.DataSource == 'geant4'))

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
                                      TRTCalDbTool = InDetCalDbTool,
                                      TRTStrawSummaryTool = InDetStrawSummaryTool)
ToolSvc += FillAlignTRTHits
print      FillAlignTRTHits

from TRT_CalibTools.TRT_CalibToolsConf import TRTCalibrator 
TRTCalibrator = TRTCalibrator ( name = 'TRTCalibrator',
                                MinRt               = %s,""" % config["MinRT"]
    ostring+="""
                                MinT0               = %s,""" % config["MinT0"]
    ostring+="""
                                Nevents             = -1,\n"""
    selstring=calsettings
    ostring+="%32sSelstring =  '%s'," % (" ",selstring['_'+calpart]['sel'])
    for setting in selstring['_'+calpart]['user']:
        if len(selstring['_'+calpart]['user'][setting])>0: ostring+= '\n%32s%s =  %s,'%(" ",setting,selstring['_'+calpart]['user'][setting])

    ostring+="""
                                Hittuple            = 'merged.root',
                                RtRel               = '%s',""" % config["RtRelation"]
    ostring+="""
                                RtBinning           = '%s',""" % config["RtBinning"]
    ostring+="""
                                UseP0               = %s,""" % "True"#config["UsePol0"]
    ostring+="""
                                FloatP3             = %s,""" % config["FloatPol3"]
    ostring+="""
                                T0Offset             = %s,""" % config["T0Offset"]
    ostring+="""
                                TrtManagerLocation  = InDetKeys.TRT_Manager(),
				DoShortStrawCorrection = False,
                                NeighbourSvc=TRTStrawNeighbourSvc,
                                TRTCalDbTool = InDetCalDbTool,
                                TRTStrawSummaryTool = InDetStrawSummaryTool)

ToolSvc += TRTCalibrator
print      TRTCalibrator

# select good quality tracks
#from TRT_AlignAlgs.TRT_AlignAlgsConf import TRTTrackSelectionAlg
#SelectTRTAlignTracks = TRTTrackSelectionAlg( name = "SelectTRTAlignTracks",
#                                             DoPtCut = False,
#                                             D0Max = 10000,
#                                             D0Min = -10000,
#                                            # PtMin = 2000,
#                                            # PtMax = 'inf',
#                                             inputTrackList = "StandaloneTRTTracks",
#                                             UseCosmicTrackSelection = True,
#                                             MinEventPhase = -1000,
#                                             MaxEventPhase = 1000,
#                                             outputTrackList = "TRTCalibTracks",
#                                             SummaryTool = InDetTrackSummaryTool,
#                                             MaxChisqPerDof = 3, #50.,
#                                             MinPixelHits = -1000,
#                                             MinSCTHits = -1000,
#                                             MinTRTHits = 45,
#                                             EtaMin    = -1000,
#                                             PhiMin    = -1000,
#                                             EtaMax   = 1000,
#                                             PhiMax   = 1000)

#topSequence += SelectTRTAlignTracks
#print          SelectTRTAlignTracks



from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetDetailedTrackSelectorTool
TRTTrackSelectorTool = InDet__InDetDetailedTrackSelectorTool(name = "InDetDetailedTrackSelectorTool",
                                        pTMin              =    1000    ,
                                        fitChi2OnNdfMax         = 50.           ,
                                        z0Max           = 30            ,
                                        nHitBLayer      = 0,
                                        nHitPix         = 0,
                                        nHitBLayerPlusPix =     0,
                                        nHitSct         =       0,
                                        nHitSi          = 4,
                                        nHitTrt                         = 20,
                                        nHitTrtPlusOutliers             =0,
                                        nHitTrtHighE                    =0,
                                        nHitTrtPlusOutliersHighE        =0,
                                        nHitTrtHighEFractionMax         =0,
                                        nHitTrtHighEFractionWithOutliersMax=0
                                        )

ToolSvc += TRTTrackSelectorTool
if (InDetFlags.doPrintConfigurables()):
        print TRTTrackSelectorTool








conddb.addFolderWithTag('TRT_OFL','/TRT/Calib/errors','TrtCalibErrors-ErrorVal-00-00')
                                        
from TRT_CalibAlgs.TRT_CalibAlgsConf import TRTCalibrationMgr
CosmicsTRTCalibMgr = TRTCalibrationMgr(name                = 'CosmicsTRTCalibMgr',
                                       StreamTool          = TRTCondStream,
				       Max_ntrk		   = 10000,
                                       TrackSelectorTool   = TRTTrackSelectorTool,
                                       TrkCollections      = [ 'TRTCalibTracks' ],
                                       AlignTrkTools       = [ FillAlignTrkInfo, FillAlignTRTHits ],
                                       DoCalibrate         = True,
                                       TrackFitter         = InDetTrackFitter,
                                       FitTools            = [ TRTCalFitTool] )

topSequence += CosmicsTRTCalibMgr
print CosmicsTRTCalibMgr

from TRT_ConditionsAlgs.TRT_ConditionsAlgsConf import TRTCondWrite
TRTCondWrite = TRTCondWrite( name = "TRTCondWrite")
topSequence+=TRTCondWrite 

# DCS Data Folders
if (globalflags.InputFormat() == 'bytestream' and globalflags.DataSource() == 'data'):
    if InDetFlags.useTrtDCS():
        conddb.addFolder('DCS_OFL',"/TRT/DCS/HV/BARREL <cache>600</cache>",classname='CondAttrListCollection')
        conddb.addFolder('DCS_OFL',"/TRT/DCS/HV/ENDCAPA <cache>600</cache>",classname='CondAttrListCollection')
        conddb.addFolder('DCS_OFL',"/TRT/DCS/HV/ENDCAPC <cache>600</cache>",classname='CondAttrListCollection')

#############################################conddb.addFolderWithTag('TRT_OFL','/TRT/Calib/errors','TrtCalibErrors-ErrorVal-00-00')
"""

    if not calibconstants=="":
        ostring+="""
conddb.blockFolder("/TRT/Calib/RT" )   
conddb.blockFolder("/TRT/Calib/T0" )  
conddb.addFolderWithTag('TRT_OFL','/TRT/Calib/errors','TrtCalibErrors-00') 
"""

    if not calibconstants=="":
        ostring+='TRTCondWrite.CalibInputFile="%s"\n' % (calibconstants)

    return ostring

