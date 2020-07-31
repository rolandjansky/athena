import os,sys,time,glob,fnmatch

def singlebeam(config,inputfiles,calibconstants):
    print "Loading from SingleBeamTemplate.py"
    ostring="""
#--------------------------------------------------------------
# Control
#--------------------------------------------------------------

# --- Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
OutputLevel     = INFO
# --- produce an atlantis data file
doJiveXML       = False
# --- run the Virtual Point 1 event visualisation
doVP1           = False
# --- controls what is written out. ESD includes AOD, so its normally enough
doWriteESD      = True
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
#DetDescrVersion = "ATLAS-CSC-02-00-00"
DetDescrVersion = "ATLAS-CommNF-09-00-00"
#--------------------------------------------------------------
# load Global Flags and set defaults (import the new jobProperty globalflags)
#--------------------------------------------------------------

from AthenaCommon.GlobalFlags import globalflags
# --- default is atlas geometry
globalflags.DetGeo = 'atlas'
# --- set defaults
globalflags.DataSource = 'data'
# --- BS default input uses this
#globalflags.ConditionsTag = 'OFLCOND-CSC-00-00-00'
globalflags.InputFormat = 'pool'

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
jobproperties.Beam.beamType="singlebeam"
#--------------------------------------------------------------
# Set Detector setup
#--------------------------------------------------------------

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
# --- printout
DetFlags.Print()


#--------------------------------------------------------------
# Load InDet configuration
#--------------------------------------------------------------

# --- setup InDetJobProperties
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doTruth=False
#InDetFlags.solenoidOn=%s""" % config["MagnetOn"]
    ostring+="""

# --- uncomment to change the default of one of the following options:
InDetFlags.doNewTracking  = False
#InDetFlags.doLowPt        = True
InDetFlags.doBeamGas      = True
InDetFlags.doBeamHalo     = True
InDetFlags.doBackTracking = False
InDetFlags.doTRTStandalone = True
#InDetFlags.doSingleSpBackTracking = True

# --- Turn off track slimming
#InDetFlags.doSlimming = False

# --- possibility to run tracking on subdetectors separately (and independent from each other)
#InDetFlags.doTrackSegmentsPixel = True
#InDetFlags.doTrackSegmentsSCT   = True
InDetFlags.doTrackSegmentsTRT   = True

# --- possibility to change the trackfitter
#InDetFlags.trackFitterType = 'KalmanFitter'

# --- activate monitorings
InDetFlags.doMonitoringGlobal    = True
InDetFlags.doMonitoringPixel     = True
InDetFlags.doMonitoringSCT       = True
InDetFlags.doMonitoringTRT       = True
InDetFlags.doMonitoringAlignment = True

# --- activate (memory/cpu) monitoring
#InDetFlags.doPerfMon = True

# --- activate creation of standard plots
#InDetFlags.doPhysValMon  = True

# --- produce various ntuples (all in one root file)
#InDetFlags.doTrkNtuple      = True
#InDetFlags.doPixelTrkNtuple = True
#InDetFlags.doVtxNtuple      = True
#InDetFlags.doConvVtxNtuple  = True

# activate the print InDetXYZAlgorithm statements
InDetFlags.doPrintConfigurables = True
# IMPORTANT NOTE: initialization of the flags and locking them is done in InDetRec_jobOptions.py!
# This way RecExCommon just needs to import the properties without doing anything else!
# DO NOT SET JOBPROPERTIES AFTER THIS LINE! The change will be ignored!

#--------------------------------------------------------------
# load master joboptions file
#--------------------------------------------------------------

include("InDetRecExample/InDetRec_all.py")
include( "CBNT_Athena/CBNT_AthenaAware_jobOptions.py" )
include( "CBNT_Athena/CBNT_EventInfo_jobOptions.py" )
include( "InDetCosmicRecExample/InDetCosmicFlags_jobOptions.py" )
include("InDetCosmicRecExample/InDetCosmicNtuple_jobOptions.py")
#if not hasattr(ServiceMgr, 'THistSvc'):
#    from GaudiSvc.GaudiSvcConf import THistSvc
#    ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output += ["AANT DATAFILE='ntuple.root' OPT='RECREATE'"]
theApp.TopAlg += [ "AANTupleStream" ]
AANTupleStream = Algorithm( "AANTupleStream" )
#AANTupleStream.ExtraRefNames = [ "StreamESD","StreamRDO" ]
AANTupleStream.OutputName = 'ntuple.root'
AANTupleStream.ExistDataHeader=False

#--------------------------------------------------------------
# Event related parameters and input files
#--------------------------------------------------------------

# Number of events to be processed (default is 10)
theApp.EvtMax = -1
#ServiceMgr.EventSelector.SkipEvents = 2
#ServiceMgr.StoreGateSvc.Dump = True

if not doReadBS:
  ServiceMgr.PoolSvc.AttemptCatalogPatch=True
  ServiceMgr.EventSelector.InputCollections = ["""
    for i in range(len(inputfiles)-1):
        ostring+='"%s", ' % inputfiles[i]
    ostring+='"%s"]' % inputfiles[len(inputfiles)-1]
    ostring+="""

from AthenaCommon.AppMgr import ToolSvc
from TRT_ConditionsTools.TRT_ConditionsToolsConf import TRTCalDbTool
TRTCalibDBTool=TRTCalDbTool()
ToolSvc+=TRTCalibDBTool


#InDetTRTExtensionTool.UseDriftRadius=False
#InDetTRT_DriftFunctionTool.DummyMode=True
#InDetTRT_DriftFunctionTool.UniversalError=1.15
InDetTRTExtensionTool.maxImpactParameter=500
#InDetTRTExtensionTool.Maxslope=.0005
#InDetTRTExtensionTool.ZVertexHalfWidth=1500
#InDetTRT_TrackSegmentsMakerTRT.pTmin=500*MeV


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
                                      TRTCalDbTool=TRTCalibDBTool)
ToolSvc += FillAlignTRTHits
print      FillAlignTRTHits

#from TRT_CalibTools.TRT_CalibToolsConf import TRTCalAccumulator
#TRTCalAccumulator = TRTCalAccumulator ( name = "TRTCalAccumulator",
#                                        TRTCalDBTool=TRTCalibDBTool,
#                                        storeAllWires = True,
#                                        calibrateRt = False,
#                                        createStrawTuple = True,
#                                        rtParameterizationType = 1,
#                                        rtPolyOrder = 0,
#                                        minEntriesPerStraw = 0, #2'
#                                        maxTrackChisquarePerDof = 10.,
#                                        minTimebinsOverThreshold = 2)

#ToolSvc += TRTCalAccumulator
#print      TRTCalAccumulator

# select good quality tracks
from TRT_AlignAlgs.TRT_AlignAlgsConf import TRTTrackSelectionAlg
SelectTRTAlignTracks = TRTTrackSelectionAlg( name = "SelectTRTAlignTracks",
#                                             inputTrackList = "InDetCosmic_Tracks",
#                                             inputTrackList = "TRT_Cosmic_Tracks",
#                                             inputTrackList = "Tracks",
                                             inputTrackList = InDetKeys.UnslimmedTracks(),
                                             outputTrackList = "TRTCalibTracks",
                                             SummaryTool = InDetTrackSummaryTool,
                                             MaxChisqPerDof = 50.,
                                             MinPixelHits = -1000,
                                             MinSCTHits = -1000,
                                             MinTRTHits = 15)

topSequence += SelectTRTAlignTracks
print          SelectTRTAlignTracks
                                        
from TRT_CalibAlgs.TRT_CalibAlgsConf import TRTCalibrationMgr
CosmicsTRTCalibMgr = TRTCalibrationMgr(name                = 'CosmicsTRTCalibMgr',
                                       TrkCollections      = [ 'TRTCalibTracks' ],
                                       AlignTrkTools       = [ FillAlignTrkInfo, FillAlignTRTHits ],
                                       #AccumulatorTools    = [ TRTCalAccumulator ],
                                       TrackFitter         = InDetTrackFitter,
                                       FitTools            = [ TRTCalFitTool] )

topSequence += CosmicsTRTCalibMgr
print CosmicsTRTCalibMgr

"""

    if not calibconstants=="":
        ostring+="""
conddb.blockFolder("/TRT/Calib/RT" )   
conddb.blockFolder("/TRT/Calib/T0" )   
"""

    if not calibconstants=="":
        ostring+='TRTCalibDBTool.calibTextFile="%s"\n' % (calibconstants)

    ostring+='TRTCalibDBTool.StreamTool=TRTCondStream'
    return ostring

