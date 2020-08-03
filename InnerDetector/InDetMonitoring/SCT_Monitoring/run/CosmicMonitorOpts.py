# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#======================================================================
# TOP JobOptions to run ID SR1 Cosmic Reconstruction
# Author: Maria Jose Costa (based on the TB reconstruction)
#======================================================================

#--------------------------------------------------------------
# Internal flags for Inner Detector
#--------------------------------------------------------------

# --- setup flags with default values
include( "SCT_Monitoring/CosmicMonitorFlags.py" )

if CosmicMonitorFlags.doSim:
    CosmicMonitorFlags.doTruth = True
else:
    CosmicMonitorFlags.doTruth = False    

# if make alignment ntuple disable the CBNT ntuple
if CosmicMonitorFlags.makeAlignNtp:
    CosmicMonitorFlags.CBNT = False

# ------------------------------------
# import GlobalFlags from RecExCommon
# ------------------------------------
from AthenaCommon.GlobalFlags import GlobalFlags

GlobalFlags.DetGeo.set_atlas()

if CosmicMonitorFlags.doSim:
    # Reading G4 simulated data from Pool
    GlobalFlags.DataSource.set_geant4()
    GlobalFlags.InputFormat.set_pool()
else:
    # Reading ByteStream real data
    GlobalFlags.DataSource.set_data()
    GlobalFlags.InputFormat.set_bytestream() 

# ----------------------------------
# import DetFlags from AthenaCommon
# ----------------------------------
from AthenaCommon.DetFlags import DetFlags

DetFlags.pixel_setOff()
DetFlags.SCT_setOn()
DetFlags.TRT_setOff()

DetFlags.detdescr.pixel_setOff()
DetFlags.detdescr.SCT_setOn()
DetFlags.detdescr.TRT_setOff()

if CosmicMonitorFlags.doSim:
    DetFlags.readRDOPool.pixel_setOff()
    DetFlags.readRDOPool.SCT_setOn()
    DetFlags.readRDOPool.TRT_setOff()
else:
    DetFlags.readRDOBS.pixel_setOff()
    DetFlags.readRDOBS.SCT_setOn()
    DetFlags.readRDOBS.TRT_setOff()
    
DetFlags.makeRIO.pixel_setOff()
DetFlags.makeRIO.SCT_setOn()
DetFlags.makeRIO.TRT_setOff()

DetFlags.Print()

#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
if CosmicMonitorFlags.doSim:
    include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )
    # converters
    include( "InDetEventAthenaPool/InDetEventAthenaPool_joboptions.py" )
    include( "EventAthenaPool/EventAthenaPool_joboptions.py" )
    include( "GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPool_joboptions.py" )
    # Read Pool data
    EventSelector = Service( "EventSelector" )
    EventSelector.InputCollections = ["InDetCosmicDig.pool.root"]
    #EventSelector.InputCollections = ["rfio:/castor/cern.ch/user/b/batley/G4digi_1kSingleMuons.pool.root"]

else:
    print 'Real data not yet available'

#--------------------------------------------------------------
# GeoModel Detector description
#--------------------------------------------------------------
include ("AtlasGeoModel/SetGeometryVersion.py")
GeoModelSvc.InDetVersionOverride="InnerDetector-Cosmic-00"
DetDescrCnvSvc.InDetIDFileName="IdDictParser/IdDictInnerDetector_Cosmic.xml"
include ("AtlasGeoModel/GeoModelInit.py")

#----------------------------------------------------------------------
# Configure further tools needed by the trackers
#----------------------------------------------------------------------
if CosmicMonitorFlags.SiTBLineFitter:    
    # particle property service
    include( "PartPropSvc/PartPropSvc.py" )  

#--------------------------------------------------------------
# Read Simulated Hits
#--------------------------------------------------------------
#theApp.Dlls += [ "InDetReadExample","TruthExamples", "PileUpTools"  ]
#theApp.TopAlg += ["DumpMC"]
#theApp.TopAlg += ["ReadSiHit/SCT_Hit"]
#SCT_Hit = Algorithm( "SCT_Hit" )
#SCT_Hit.OutputLevel = 1;
#SCT_Hit.DataObjectName = "SCT_Hits";
#SCT_Hit.NtupleDirName = "SCT_Hits";
#SCT_Hit.NumberMaxHits = 500;
#SCT_Hit.NtupleIdentifier = "72";
#SCT_Hit.NtupleBlockName = "SCTHITS";

#--------------------------------------------------------------
# reconstruction
#--------------------------------------------------------------
include( "SCT_Monitoring/CosmicMonitorRec_jobOptions.py" )

#--------------------------------------------------------------
# ATLANTIS
#--------------------------------------------------------------
if CosmicMonitorFlags.JiveXML:
    include( "JiveXML/JiveXML_jobOptionBase.py" )
    #include( "JiveXML/DataTypes_InDet.py" )
    EventData2XML.PixelSpacePoints         = "PixelSpacePoints"
    EventData2XML.SCTSpacePoints           = "SCT_SpacePoints"
    EventData2XML.TRTClusters              = "TRT_DriftCircles";   
    EventData2XML.iPatTracks               = "iPatTracks" 
    EventData2XML.xKalmanTracks            = "Tracks"
    EventData2XML.DataTypes += ["SiSpacePoint","SiCluster","TRT",
                            "xKalmanTrack","iPatTrack"]
    if CosmicMonitorFlags.doSim:
        EventData2XML.DataTypes += ["TruthTrack"]
        EventData2XML.McEventCollection="GEN_EVENT"
    if CosmicMonitorFlags.AtlantisGeometry:
        EventData2XML.WriteGeometry = True


#--------------------------------------------------------------
# InDetAlignment ntuple
#--------------------------------------------------------------
if CosmicMonitorFlags.makeAlignNtp:
    include ("InDetAlignAlgs/InDetAlignNt.py")    
    InDetAlignNt.newtrkcol="Tracks"
    InDetAlignNt.newtrk=True
    InDetAlignNt.TRT=1

        
# -------------------------------------------------------------
# Fill CBNT
# -------------------------------------------------------------

# algorithms to fill the CBNT_Athena ntuple

CBNT_Athena = Algorithm( "CBNT_Athena" )

if CosmicMonitorFlags.CBNT:
    # Fill event info
    CBNT_Athena.Members += [ "CBNT_EventInfo" ]
    # Fill InDet reconstructed variables    
    include( "InDetCosmicRecExample/InDetCosmicNtuple_jobOptions.py" )
#--------------------------------------------------------------
# Monitoring
#--------------------------------------------------------------	
if CosmicMonitorFlags.doMonitoring:
#	# SCT monitoring
	ToolSvc = Service("ToolSvc")
	SCTCosmicHisto = Algorithm("SCTCosmicHisto")
##	#SCTCosmicHisto.TestBeamYear = 2004;
	ToolSvc.MonTool.OutputLevel = 2;
	ToolSvc.MonTool.firstPlane = 10;
	ToolSvc.MonTool.secondPlane = 1;
	ToolSvc.MonTool.localSummary= 1;
	theApp.Dlls += [ "SCT_Monitoring" ];
	theApp.TopAlg += ["SCTCosmicHisto"];
	SCTCosmicHisto.OutputLevel = 1

#--------------------------------------------------------------
# Select Options
#--------------------------------------------------------------
# Ntuple
theApp.Dlls += [ "CBNT_Utils","CBNT_Athena" ]

if CosmicMonitorFlags.makeAlignNtp:
    theApp.Dlls += ["HbookCnv"]
    theApp.HistogramPersistency = "HBOOK"
else:    
    theApp.Dlls += [ "RootHistCnv" ]
    theApp.HistogramPersistency = "ROOT"
    
HistogramPersistencySvc = Service( "HistogramPersistencySvc" )
HistogramPersistencySvc.OutputFile  = "monitoring.root"
NTupleSvc = Service( "NTupleSvc" )
if CosmicMonitorFlags.makeAlignNtp:
    NTupleSvc.Output = [ "FILE1 DATAFILE='alignment.hbook' TYP='HBOOK' OPT='NEW'" ]
else:    
    NTupleSvc.Output = [ "FILE1 DATAFILE='ntuple.root' TYP='ROOT' OPT='NEW'" ]
theApp.TopAlg += [ "CBNT_Athena"]
CBNT_Athena = Algorithm( "CBNT_Athena" )
CBNT_Athena.NtupleLocID="/FILE1/Cosmic/tree"

#-------------------------------------------------
# Write out Transient Event Store content in POOL
#-------------------------------------------------

if CosmicMonitorFlags.doWriteESD:
    if not CosmicMonitorFlags.doSim:
        include ("AthenaPoolCnvSvc/WriteAthenaPool_jobOptions.py")
    else:
        # WriteAthenaPool is inconsistent with ReadAthenaPool
        # there should be a specific file for that
        theApp.OutStream += [ "Stream1" ]
        theApp.OutStreamType = "AthenaOutputStream"
        Stream1 = Algorithm( "Stream1" )
        Stream1.EvtConversionSvc = "AthenaPoolCnvSvc"

    #check dictionary
    AthenaSealSvc.CheckDictionary = True

    # Define the output file name
    Stream1.OutputFile    = "InDetESD.root"

    # list of output objects
    # Converters:
    include ( "EventAthenaPool/EventAthenaPool_joboptions.py")
    include ( "GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPool_joboptions.py")    
    include( "InDetEventAthenaPool/InDetEventAthenaPool_joboptions.py" )
    include( "TrkEventAthenaPool/TrkEventAthenaPool_joboptions.py" )
    include ( "RecAthenaPool/RecAthenaPool_joboptions.py")
    include ("InDetEventCnvTools/InDetEventCnvTools_jobOptions.py") 
    try:
        fullItemList += ["EventInfo#*"]
    except:
        fullItemList = []
        fullItemList += ["EventInfo#*"]

    #HepMC record (be careful it has to be read by something to be written out)
    #Stream1.ItemList+=["McEventCollection#*"]
    include ( "GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPoolItemList_joboptions.py")

    #full tracking
    fullItemList +=["TrackCollection#*"]
    fullItemList += [ "Rec::TrackParticleContainer#*" ]

    # Inner Detector
    fullItemList +=["InDet::SCT_ClusterContainer#*"]
    fullItemList +=["InDet::PixelClusterContainer#*"]
    fullItemList +=["InDet::TRT_DriftCircleContainer#*"]
    #fullItemList +=["SpacePointContainer#*"]
   
    #Reconstruction
    include ( "RecAthenaPool/RecAthenaPoolItemList_joboptions.py")
    
    Stream1.ItemList = fullItemList

#--------------------------------------------------------------
# write out a summary of the time spent
#--------------------------------------------------------------
theApp.Dlls += [ "GaudiAud" ]
theAuditorSvc = AuditorSvc()
theAuditorSvc.Auditors  += [ "ChronoAuditor"]
# -------------------------------------------------------------
#Output level
# -------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
# you can override this for individual modules if necessary
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel             = 2
#StoreGateSvc.Dump=true;
# Number of events to be processed (default is 10)
theApp.EvtMax = 100;
MessageSvc.defaultLimit=100000000;
#EventSelector.SkipEvents=10
