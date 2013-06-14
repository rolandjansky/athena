# Test joboptions for the loading of the IdDictCnvTest
#
# For MC EventSelector:
theApp.setup( MONTECARLO )

# For ZebraTDR EventSelector
##include "AthenaCommon/Atlas_ZebraTDR.UnixStandardJob.txt"
include( "IdDictDetDescrCnv/IdDictDetDescrCnv_joboptions.py" )

# Turn on the reading of the dictionary
DetDescrCnvSvc = Service( "DetDescrCnvSvc" )
DetDescrCnvSvc.DecodeIdDict = TRUE
# Algs
theApp.TopAlg += [ "IdDictCnvTest" ]
# Set xml file to read in for each system 
#  - only needed if default is not ok
#DetDescrCnvSvc.AtlasIDFileName = "IdDictATLAS.xml";
#DetDescrCnvSvc.InDetIDFileName = "IdDictInnerDetector.xml";
#DetDescrCnvSvc.LArIDFileName   = "IdDictLArCalorimeter.xml";
#DetDescrCnvSvc.TileIDFileName  = "IdDictTileCalorimeter.xml";
#DetDescrCnvSvc.LVL1IDFileName  = "IdDictLVL1Calorimeter.xml";
DetDescrCnvSvc.MuonIDFileName  = "IdDictMuonSpectrometer_P.03.xml";
# One may turn off the read of one of more system iddict files
#DetDescrCnvSvc.InDetIDFileName = "NULL";
#DetDescrCnvSvc.LArIDFileName   = "NULL";
#DetDescrCnvSvc.TileIDFileName  = "NULL";
#DetDescrCnvSvc.LVL1IDFileName  = "NULL";
#DetDescrCnvSvc.MuonIDFileName  = "NULL";
#--------------------------------------------------------------
# Set output level threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel      = 2
# Provide dictionary file
#DetDescrCnvSvc.IdDictName = "ATLAS_IDS.xml";
# Number of events to be processed (default is 10)
theApp.EvtMax = 2
# Provide a first/last range or a list
#EventSelector.runs = {1, 100000};
#EventSelector.firstRun = 1;
#EventSelector.lastRun  = 100000;
