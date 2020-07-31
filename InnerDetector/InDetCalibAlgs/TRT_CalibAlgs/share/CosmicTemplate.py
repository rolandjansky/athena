import os,sys,time,glob,fnmatch

def cosmic(config,inputfiles,calibconstants, nevents):
    
    print "Loading from CosmicTemplate.py"
    ostring = """
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
# --- controls what is written out. ESD includes AOD, so its normally enough
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
jobproperties.Beam.numberOfCollisions = 0.0  
jobproperties.Beam.beamType="cosmics"

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

from AthenaCommon.DetFlags import DetFlags 
# --- switch on InnerDetector
DetFlags.ID_setOn()

#DetFlags.makeRIO.pixel_setOff()
#DetFlags.readRIOPool.pixel_setOff()
#DetFlags.readRIOBS.pixel_setOff() 

#DetFlags.makeRIO.SCT_setOff()
#DetFlags.readRIOPool.SCT_setOff()
#DetFlags.readRIOBS.SCT_setOff() 

# --- and switch off all the rest
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()

# --- printout
DetFlags.Print()

#--------------------------------------------------------------
# Load InDet configuration
#--------------------------------------------------------------

#from RecExConfig.RecFlags import rec
rec.Commissioning = True
rec.doMonitoring = True

# --- setup InDetJobProperties
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doTruth       = (globalflags.DataSource == 'geant4' and globalflags.InputFormat == "pool")

#InDetFlags.doCosmics=True
# --- uncomment to change the default of one of the following options:
#InDetFlags.doNewTracking  = False
#InDetFlags.doCTBTracking = False
#InDetFlags.preProcessing  = True
#InDetFlags.doLowPt        = True
#InDetFlags.doBeamGas      = True
#InDetFlags.doBeamHalo     = True
#InDetFlags.doxKalman      = False
#InDetFlags.doiPatRec      = False
#InDetFlags.doBackTracking = False
#InDetFlags.doTRTStandalone = False
#InDetFlags.doVertexFinding = False
#InDetFlags.doConversions = False
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

# --- Disable DCS usage
#InDetFlags.useDCS=False

# --- activate (memory/cpu) monitoring
#InDetFlags.doPerfMon = True

# --- activate ntuple writing for IDRecStat (if it is on)
#InDetFlags.doStatNtuple    = True


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


# This setup works:
#ToolSvc.InDetCosmicsEventPhaseTool.UseNewEP=False
#topSequence.InDetCosmicsEventPhase.EventPhaseTool=ToolSvc.InDetCosmicsEventPhaseTool

# Try new setup proposed by Sasa::
ToolSvc.InDetCosmicsEventPhaseTool.UseNewEP		= True
ToolSvc.InDetCosmicsEventPhaseTool.GlobalOffset		= 8
topSequence.InDetCosmicsEventPhase.EventPhaseTool	= ToolSvc.InDetCosmicsEventPhaseTool

#--------------------------------------------------------------
# Event related parameters and input files
#--------------------------------------------------------------

# Number of events to be processed (default is 10)
theApp.EvtMax = %i """ %nevents
    ostring+="""
#ServiceMgr.EventSelector.SkipEvents = 20
#ServiceMgr.StoreGateSvc.Dump = True

# hack for ATLANIS (rrashes in M8+ data)
if doJiveXML:
  ToolSvc.TrackRetriever.OtherTrackCollections =["CombinedInDetTracks", "CombinedInDetTracks_CTB"]

if not doReadBS:
  ServiceMgr.PoolSvc.AttemptCatalogPatch=True
  
  # cosmic MC without field
  #ServiceMgr.EventSelector.InputCollections = ["castor:/castor/cern.ch/grid/atlas/caf/atlcal/perm/id/cosmics/ESDs/InDetESD_91900_lb3.root" ]
  ServiceMgr.EventSelector.InputCollections = ["""
    for i in range(len(inputfiles)-1):
        ostring+='"%s", ' % inputfiles[i]
    ostring+='"%s"]' % inputfiles[len(inputfiles)-1]
    ostring+="""

if doReadBS:
  ServiceMgr.EventSelector.Input = [ "some file" ]

#--------------------------------------------------------------
# Calibration stuff
#--------------------------------------------------------------

from AthenaCommon.AppMgr import ToolSvc

from AthenaCommon.AppMgr import ServiceMgr

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
                                      TRTCalDbSvc=TRTCalibDBSvc)
ToolSvc += FillAlignTRTHits
print      FillAlignTRTHits

from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetCosmicTrackSelectorTool

TRTTrackSelectorTool = InDet__InDetCosmicTrackSelectorTool(name = "InDetCosmicTrackSelectorTool",
                                        TrackSummaryTool        = InDetTrackSummaryTool,
					maxZ0			= 10000,
					maxD0			= 10000,
					minPt			= -1,
					numberOfPixelHits	= 0,
					numberOfSCTHits		= 0,
					numberOfTRTHits		= 20,
					numberOfSiliconHits	= 0,
					numberOfSiliconHitsTop	= -1,
					numberOfSiliconHitsBottom	=-1,
                                        )

ToolSvc += TRTTrackSelectorTool
if (InDetFlags.doPrintConfigurables()):
        print TRTTrackSelectorTool


from TRT_CalibAlgs.TRT_CalibAlgsConf import TRTCalibrationMgr
CosmicsTRTCalibMgr = TRTCalibrationMgr(name                = 'CosmicsTRTCalibMgr',
                                       StreamTool          = TRTCondStream,
                                       TrackSelectorTool   = TRTTrackSelectorTool,
                                       #TrkCollections      = [ 'TRTCalibTracks' ],
                                       #TrkCollections      = [ 'CombinedInDetTracks' ],
                                       TrkCollections      = [ 'CombinedInDetTracks_CTB' ],
                                       AlignTrkTools       = [ FillAlignTrkInfo, FillAlignTRTHits ],
                                       #AccumulatorTools    = [ TRTCalAccumulator ],
				       IsCosmic 	   = True,
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

 
#==============================
#====  Straw Status stuff  ====
#==============================

#from TRT_CalibAlgs.TRT_CalibAlgsConf import InDet__TRT_StrawStatus
#TRT_StrawStatus = InDet__TRT_StrawStatus(       name                    = "TRT_StrawStatus",
#                                                outputFileName          = "TRT_StrawStatusOutput",
#                                                tracksCollection        = "TRTStandaloneTRTTracks_CTB"                                          
#                                                )
#topSequence += TRT_StrawStatus
#print TRT_StrawStatus


#===========================================
#===   OVERRIDE THE ALIGMENT CONSTANTS	===
#===========================================

"""

    if not calibconstants=="":
        ostring+="""
conddb.blockFolder("/TRT/Calib/RT" )   
conddb.blockFolder("/TRT/Calib/T0" )   
"""

    if not calibconstants=="":
        ostring+='TRTCondWrite.CalibInputFile="%s"\n' % (calibconstants)

    return ostring
