
import os,sys,time,glob,fnmatch

def collision(config,inputfiles,calibconstants,nevents):
#    print "Loading from CollisionTemplate.py"
#    print "Based on Thjis jO: "
    ostring="""
# Collision for HEAVY IONS!!!  JOs  Full Reco. RAW fils
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
###doReadBS        = False
doReadBS        = True
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
jobproperties.Beam.numberOfCollisions = 10.0 # default in RecExCommon for 900 GeV
#jobproperties.Beam.energy             = 3500.*Units.GeV
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
rec.doHeavyIon=True
#rec.Commissioning=False
#rec.doMonitoring=False

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
#InDetFlags.doHeavyIon = True
#InDetFlags.doNewTracking  = False
#InDetFlags.doLowPt        = False
#InDetFlags.doBeamGas      = True
#InDetFlags.doBeamHalo     = True
#InDetFlags.doBackTracking = False
#InDetFlags.doTRTStandalone = True
#InDetFlags.doSingleSpBackTracking = True
#InDetFlags.doV0Finder         = False
#InDetFlags.doConversions      = False
#InDetFlags.doStatistics       = False
#InDetFlags.primaryVertexSetup = "DefaultFastFinding"
#InDetFlags.useZvertexTool     = True
#InDetFlags.primaryVertexCutSetup = "NoLumi"
#InDetFlags.doSingleCollisionVertexReco=True
#InDetFlags.primaryVertexCutSetup = "StartUp"
#InDetFlags.useBeamConstraint  = False
#InDetFlags.doVertexFinding    = True


# --- Turn off track slimming
#InDetFlags.doSlimming = False

# --- possibility to run tracking on subdetectors separately (and independent from each other)
#InDetFlags.doTrackSegmentsPixel = True
#InDetFlags.doTrackSegmentsSCT   = True
#InDetFlags.doTrackSegmentsTRT   = True

# --- possibility to change the trackfitter
#InDetFlags.trackFitterType = 'KalmanFitter'

# --- deactivate monitorings
#InDetFlags.doMonitoringGlobal    = False
#InDetFlags.doMonitoringPixel     = False
#InDetFlags.doMonitoringSCT       = False
#InDetFlags.doMonitoringTRT       = False
#InDetFlags.doMonitoringAlignment = False

# --- activate (memory/cpu) monitoring
#InDetFlags.doPerfMon = True

# --- activate creation of standard plots
#InDetFlags.doPhysValMon  = True

# --- produce various ntuples (all in one root file)
#InDetFlags.doTrkNtuple      = False
#InDetFlags.doPixelTrkNtuple = False
#InDetFlags.doSctTrkNtuple   = False
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
#-------------------------------------------------
# change Configured parameters
#------------------------------------------------

from InDetRecExample.ConfiguredNewTrackingCuts import ConfiguredNewTrackingCuts
InDetNewTrackingCuts = ConfiguredNewTrackingCuts("HeavyIon")
InDetNewTrackingCuts._ConfiguredNewTrackingCuts__minPT            =470    #1000.0
#InDetNewTrackingCuts._ConfiguredNewTrackingCuts__minClusters      =9      #9
#InDetNewTrackingCuts._ConfiguredNewTrackingCuts__minSiNotShared   =7      #7
#InDetNewTrackingCuts._ConfiguredNewTrackingCuts__maxShared        =2      #2
#InDetNewTrackingCuts._ConfiguredNewTrackingCuts__maxHoles         =1      #2
#InDetNewTrackingCuts._ConfiguredNewTrackingCuts__maxPixelHoles    =0      #2
#InDetNewTrackingCuts._ConfiguredNewTrackingCuts__maxSctHoles      =1      #2
#InDetNewTrackingCuts._ConfiguredNewTrackingCuts__maxDoubleHoles   =1      #1
#InDetNewTrackingCuts._ConfiguredNewTrackingCuts__nHolesMax        =2      #2
#InDetNewTrackingCuts._ConfiguredNewTrackingCuts__nHolesGapMax     =2      #2
#InDetNewTrackingCuts._ConfiguredNewTrackingCuts__Xi2max           =4.     #6.
#InDetNewTrackingCuts._ConfiguredNewTrackingCuts__Xi2maxNoAdd      =10.    #10
#InDetNewTrackingCuts._ConfiguredNewTrackingCuts__seedFilterLevel  =1      #1
#InDetNewTrackingCuts._ConfiguredNewTrackingCuts__radMax           =600    #600 old 330
#InDetNewTrackingCuts._ConfiguredNewTrackingCuts__RoadWidth        =20     #20
#InDetNewTrackingCuts._ConfiguredNewTrackingCuts__maxPrimaryImpact =10     #10
#InDetNewTrackingCuts._ConfiguredNewTrackingCuts__maxZImpact       =250    #250

# seed maker tool parameters
from SiSpacePointsSeedTool_xk.SiSpacePointsSeedTool_xkConf import InDet__SiSpacePointsSeedMaker_HeavyIon as SiSpacePointsSeedMaker
#SiSpacePointsSeedMaker.maxSize         =20000    # 20000
#SiSpacePointsSeedMaker.maxSizeSP       =4000     # 4000 used to be 1500
#SiSpacePointsSeedMaker.mindRadius      =10.00    # 10
#SiSpacePointsSeedMaker.maxdRadius      =270.0    # 270
#SiSpacePointsSeedMaker.maxdZdRver      =0.       # 0.
#SiSpacePointsSeedMaker.maxdImpactPPS   =0.1      # 0.1
#SiSpacePointsSeedMaker.maxdZver        =5.0      # 5.
#SiSpacePointsSeedMaker.radStep         =10.0     # 10.
#SiSpacePointsSeedMaker.polarAngleStep =0.5      # 0.5

SiSpacePointsSeedMaker.OutputLevel = DEBUG

#--------------------------------------------------------------
# load master joboptions file
#--------------------------------------------------------------

include("InDetRecExample/InDetRec_all.py")
ToolSvc.InDetPriVxFinderTool.chi2CutMethod          = 1  # defaul 2
ToolSvc.InDetPriVxFinderTool.maxChi2PerTrack        = 15.0
ToolSvc.InDetPriVxFinderTool.enableMultipleVertices = 0
ToolSvc.InDetPriVxFinderTool.useBeamConstraint      = False


#from D3PDMakerCoreComps.MakerAlg import *
#alg = MakerAlg("InDetTrackTree",topSequence,file="InDetTrackD3PD.root",D3PDSvc='D3PD::RootD3PDSvc')
#alg = MakerAlg("HID3PD",topSequence,file="myD3PD.root",D3PDSvc='D3PD::RootD3PDSvc')
#alg += PixelClusterD3PDObject(10)


#--------------------------------------------------------------
# load master joboptions file
#--------------------------------------------------------------

#include("InDetRecExample/InDetRec_all.py")

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
theApp.EvtMax = %i""" % nevents
    ostring+="""
#ServiceMgr.EventSelector.SkipEvents = 2
#ServiceMgr.StoreGateSvc.Dump = True

ServiceMgr.EventSelector.Input = ["""
    for i in range(len(inputfiles)-1):
        ostring+='"%s", ' % inputfiles[i]
    ostring+='"%s"]' % inputfiles[len(inputfiles)-1]
    ostring+="""

from AthenaCommon.AppMgr import ToolSvc


from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_StrawNeighbourSvc
TRTStrawNeighbourSvc=TRT_StrawNeighbourSvc()
ServiceMgr += TRTStrawNeighbourSvc

from TRT_CalibTools.TRT_CalibToolsConf import FitTool
TRTCalFitTool = FitTool (name = 'TRTCalFitTool')
ToolSvc += TRTCalFitTool
print      TRTCalFitTool

from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_CalDbTool
InDetCalDbTool=TRT_CalDbTool(name = "TRT_CalDbTool")

from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_StrawStatusSummaryTool
InDetStrawSummaryTool=TRT_StrawStatusSummaryTool(name = "TRT_StrawStatusSummaryTool",
                             isGEANT4=(globalflags.DataSource == 'geant4'))



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
                                      TRTCalDbTool = InDetCalDbTool,
                                      TRTStrawSummaryTool = InDetStrawSummaryTool)

ToolSvc += FillAlignTRTHits
print      FillAlignTRTHits

# select good quality tracks
#from TRT_AlignAlgs.TRT_AlignAlgsConf import TRTTrackSelectionAlg
#SelectTRTAlignTracks = TRTTrackSelectionAlg(    name = "SelectTRTAlignTracks",
#                                                inputTrackList = "CombinedInDetTracks",
##                                                inputTrackList = "StandaloneTRTTracks",
#                                                outputTrackList = "TRTCalibTracks",
#                                                SummaryTool = InDetTrackSummaryTool,
#                                                MaxChisqPerDof = 50.,
#                                                D0Max                   = 30       ,
#                                                D0Min                   = -30      ,
#                                                DoPtCut                 = True     ,
#                                                PtMax                   = 9.9999999999999998e+23,
#                                                PtMin                   = 1000          ,
#                                                EtaMax                  = 10000         ,
#                                                EtaMin                  = -10000        ,
#                                                PhiMax                  = 10000         ,
#                                                PhiMin                  = -10000        ,
#                                                MaxEventPhase           = 10000         ,
#                                                MinEventPhase           = -10000        ,
#                                                MinTRTHits              = 20            ,
#                                                MinBarrelHits           = 0            ,
#                                                MinEndcapHits           = 0             ,
#                                                RequireEndcapHits       = False         ,
#                                                MinPixelHits            = -1000         ,
#                                                MinSCTHits              = 6         ,
##                                                MinSCTHits              = -1000         ,
#                                                UseBeamSpotConstraint   = False         )
#
#topSequence += SelectTRTAlignTracks
#print          SelectTRTAlignTracks

from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetDetailedTrackSelectorTool
TRTTrackSelectorTool = InDet__InDetDetailedTrackSelectorTool(name = "InDetDetailedTrackSelectorTool",
                                        pTMin                   =    1000    ,
                                        fitChi2OnNdfMax         = 50. ,
                                        z0Max                   = 30000  ,
                                        nHitBLayer              = 0 ,
                                        nHitPix                 = 0 ,
                                        nHitBLayerPlusPix       = 0 ,
                                        nHitSct                 = 4 ,
                                        nHitSi                  = 0 ,
                                        nHitTrt                         = 20,
                                        #nHitTrtPlusOutliers             =0,
                                        #nHitTrtHighE                    =0,
                                        #nHitTrtPlusOutliersHighE        =0,
                                        #nHitTrtHighEFractionMax         =0,
                                        #nHitTrtHighEFractionWithOutliersMax=0,
                                        d0MaxPreselection                    = 1000000.0,
                                        nHitSiPhysical                       = 0,
                                        nSharedSct                           = 0,
					IPd0Max                              = 20.0,
					IPz0Max                              = 1000.0,
                                        sigIPd0Max                           = 500000.0,
                                        sigIPz0Max                           = 100000.0,
                                        )

ToolSvc += TRTTrackSelectorTool
if (InDetFlags.doPrintConfigurables()):
        print TRTTrackSelectorTool




from TRT_CalibAlgs.TRT_CalibAlgsConf import TRTCalibrationMgr
CosmicsTRTCalibMgr = TRTCalibrationMgr(name                = 'CosmicsTRTCalibMgr',
                                       StreamTool          = TRTCondStream,
                                       TrackSelectorTool   = TRTTrackSelectorTool,
                                       #TrkCollections      = [ 'StandaloneTRTTracks' ],
                                       #TrkCollections      = [ 'CombinedInDetTracks' ],
                                       TrkCollections      = [ 'Tracks' ],
                                       #TrkCollections      = [ 'TRTCalibTracks' ],
                                       #TrkCollections      = [ 'CombinedInDetTracks' ],
                                       #TrkCollections      = [ 'Tracks' ],
                                       AlignTrkTools       = [ FillAlignTrkInfo, FillAlignTRTHits ],
                                       #AccumulatorTools    = [ TRTCalAccumulator ],
                                       TrackFitter         = InDetTrackFitter,
                                       FitTools            = [ TRTCalFitTool] )

topSequence += CosmicsTRTCalibMgr
print CosmicsTRTCalibMgr


from TRT_ConditionsAlgs.TRT_ConditionsAlgsConf import TRTCondWrite
TRTCondWrite = TRTCondWrite( name = "TRTCondWrite")
topSequence+=TRTCondWrite 

#==============================
#====  Straw Status stuff  ====
#==============================

from TRT_CalibAlgs.TRT_CalibAlgsConf import InDet__TRT_StrawStatus
TRT_StrawStatus = InDet__TRT_StrawStatus(       name                    = "TRT_StrawStatus",
                                                outputFileName          = "TRT_StrawStatusOutput",
                                                tracksCollection        = "CombinedInDetTracks"                                          
                                                )
topSequence += TRT_StrawStatus
print TRT_StrawStatus




topSequence.InDetTRT_RIO_Maker.TRTEffiCut=0.5
topSequence.InDetSCT_Clusterization.TRTEffiCut=0.5
topSequence.InDetPixelClusterization.TRTEffiCut=0.5

#conddb.addFolder('TRT_OFL','/TRT/Calib/DX')
#conddb.addOverride('/TRT/Calib/DX','TRTCalibDX_L3BarrelEndcap_01')
#conddb.addOverride('/Indet/Beampos','IndetBeampos-ES1-UPD2')  
#conddb.addOverride('/TRT/Align/','TRTAlign_Collision_7T_2010_07')  
#conddb.addOverride('/Indet/Align/','InDetAlign_Collision_7T_2010_07')  
#conddb.addOverride('/Indet/TrkErrorScaling','TrkErrorScaling_7T_2010_02')
#conddb.addFolderWithTag('TRT_OFL','/TRT/Calib/errors','TrtCalibErrors-ErrorVal-00-00')

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

