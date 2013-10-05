# Test joboptions for the loading of the IdDictCnvTest
#
# For MC EventSelector:
theApp.setup( MONTECARLO )

#--------------------------------------------------------------
# Set flags and load det descr
#--------------------------------------------------------------
# For general flags
include( "RecExCommon/RecExCommon_flags.py" )

# Set local flags - only need LAr DetDescr
#DetFlags.detdescr.ID_setOff()
#DetFlags.detdescr.Tile_setOff()
#DetFlags.detdescr.Muon_setOff()
      
# Set version (must do so before AllDet_detDescr include)
DetDescrVersion = "Rome-Initial"

# set up all detector description description 
include ("RecExCommon/AllDet_detDescr.py")

# Need to override Tile tag to get TileIdentifier record
#GeoModelSvc.TileVersionOverride = "Tile-Rome-Initial-00"


# For ddexamples and ddcnvsvc

#include( "AthenaCommon/Atlas.UnixStandardJob.py" )
include( "InDetIdCnv/InDetIdCnv_jobOptions.py" )
#include( "TileDetMgrDetDescrCnv/TileDetMgrDetDescrCnv_jobOptions.py" )
#include( "MuonDetMgrDetDescrCnv/MuonDetMgrDetDescrCnv_joboptions.py" )
#include( "MuonIdCnv/MuonIdCnv_joboptions.py" )
#include( "LArIdCnv/LArIdCnv_joboptions.py" )


# CaloIdCnv included in LArIdCnv
##include "CaloIdCnv/CaloIdCnv_joboptions.txt"

# Turn on the reading of the dictionary
#DetDescrCnvSvc = Service( "DetDescrCnvSvc" )
#DetDescrCnvSvc.DecodeIdDict = TRUE

# Read dict file names from RDB:
DetDescrCnvSvc.IdDictFromRDB = True
#DetDescrCnvSvc.DoIdChecks    = True


# DLLs 
theApp.Dlls += [ "IdDictTest" ]

# Algs
theApp.TopAlg += [ "IdDictSimpleTest" ]

# Specify the tag to be used
#DetDescrCnvSvc.IdDictGlobalTag = "initial_layout"

# Set xml file to read in for each system 
#  - only needed if default is not ok
#DetDescrCnvSvc.AtlasIDFileName = "IdDictParser/IdDictATLAS.xml";
#DetDescrCnvSvc.InDetIDFileName = "IdDictParser/IdDictInnerDetector.xml";
#DetDescrCnvSvc.LArIDFileName   = "IdDictParser/IdDictLArCalorimeter_H8_2004.xml";
#DetDescrCnvSvc.TileIDFileName  = "IdDictParser/IdDictTileCalorimeter.xml";
#DetDescrCnvSvc.CaloIDFileName  = "IdDictParser/IdDictCalorimeter.xml";
#DetDescrCnvSvc.MuonIDFileName  = "IdDictParser/IdDictMuonSpectrometer.xml";
# One may turn off the read of one of more system iddict files
#DetDescrCnvSvc.InDetIDFileName = "NULL";
#DetDescrCnvSvc.LArIDFileName   = "NULL";
#DetDescrCnvSvc.TileIDFileName  = "NULL";
#DetDescrCnvSvc.CaloIDFileName  = "NULL";
#DetDescrCnvSvc.MuonIDFileName  = "NULL";

#--------------------------------------------------------------
# Set output level threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc.OutputLevel      = 2

# Number of events to be processed (default is 10)
theApp.EvtMax = 2

# Provide a first/last range or a list
#EventSelector.runs = {1, 100000};
#EventSelector.firstRun = 1;
#EventSelector.lastRun  = 100000;
