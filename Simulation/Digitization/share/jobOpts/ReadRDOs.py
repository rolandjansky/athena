###############################################################
#
# Job options file to run Digitization
#
#==============================================================
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )

# GeoModel stuff:
#
# Load NOVA objects into the Detector store
include( "NovaCnvSvc/NovaCnvSvc_jobOptions.py" )

include( "NovaConverters/NovaConverters_jobOptions.py" )

include( "PartPropSvc/PartPropSvc.py" )

NovaCnvSvc = Service( "NovaCnvSvc" )
NovaCnvSvc.OutputLevel = 5
NovaCnvSvc.Host = "atlasdev1.cern.ch"
NovaCnvSvc.Database = "parameters"
# Turn on the reading of the dictionary
DetDescrCnvSvc = Service( "DetDescrCnvSvc" )
DetDescrCnvSvc.DecodeIdDict = TRUE
#load relevant libraries
theApp.Dlls += [ "GeoModelSvc"]
# Setup Geometry Model Service
theApp.ExtSvc += [ "GeoModelSvc"]
# Pool Converters
#
include( "InDetEventAthenaPool/InDetEventAthenaPool_joboptions.py" )

include( "LArAthenaPool/LArAthenaPool_joboptions.py" )

include( "EventAthenaPool/EventAthenaPool_joboptions.py" )

include( "GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPool_joboptions.py" )

# Pool input
EventSelector = Service( "EventSelector" )
EventSelector.InputCollection = "SimplePoolFile2.root"; #CHANGEME!!
#
# Inner Detector 
# --------------
theApp.Dlls += [ "SCT_GeoModel", "PixelGeoModel", "TRT_GeoModel"]
# Setup Geometry Model Tools to be instantiated
GeoModelSvc = Service( "GeoModelSvc" )
GeoModelSvc.Detectors = [ "PixelDetectorTool", "SCT_DetectorTool", "TRT_DetectorTool" ]
GeoModelSvc.SCT_DetectorTool.G3Compatible = FALSE; # Default is true
GeoModelSvc.TRTDetectorTool.G3Compatible = FALSE; # Default is true
# Reading of RDOs
theApp.Dlls += [ "InDetReadExample"]
theApp.TopAlg += ["ReadInDetRawData/ReadPixelRDO"]
ReadPixelRDO = Algorithm( "ReadPixelRDO" )
ReadPixelRDO.DataObjectName = "PixelRDOs"
ReadPixelRDO.OutputLevel = 1
theApp.TopAlg += ["ReadInDetRawData/ReadSCT_RDO"]
ReadSCT_RDO = Algorithm( "ReadSCT_RDO" )
ReadSCT_RDO.DataObjectName = "SCT_RDOs"
ReadSCT_RDO.OutputLevel = 1
#
# Wait for Joe to put the TRT_ID in the DetectorStore
#
# ApplicationMgr.TopAlg += {"ReadInDetRawData/ReadTRT_RDO"};
# ReadTRT_RDO.DataObjectName = "TRT_RDOs";
# ReadTRT_RDO.OutputLevel = 1;
# LAr
# the LAr and Calo detector description package
include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )

include( "LArDetMgrDetDescrCnv/LArDetMgrDetDescrCnv_joboptions.py" )

# Read LArRawChannel
theApp.Dlls += [ "LArRawUtils", "LArROD" ]
# Algs
theApp.TopAlg += [ "ReadLArRaw" ]
ReadLArRaw = Algorithm( "ReadLArRaw" )
ReadLArRaw.LArRawChannelContainerName = "LArRawChannels" 
ReadLArRaw.HistogramID = 168 
ReadLArRaw.NtupleLocID = "/FILE1/LARG" 
ReadLArRaw.OutputLevel = 1
#
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel      = 4
#
# Histograms
#
theApp.Dlls += ["HbookCnv"]
theApp.HistogramPersistency = "HBOOK"
HistogramPersistencySvc = Service( "HistogramPersistencySvc" )
HistogramPersistencySvc.OutputFile  = "histo.hbook"
NTupleSvc = Service( "NTupleSvc" )
NTupleSvc.Output = [ "FILE1 DATAFILE='ntuple.rz' OPT='NEW'" ]
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 10
theApp.EvtSel = "EventSelector"
