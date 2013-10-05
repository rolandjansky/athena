#
# Test joboptions for the loading of the IdDictTest
#
include( "IdDictTest/IdDictTest_joboptions.py" )

# Set flag to use compact ids
#DetDescrCnvSvc.CompactIDsOnly = true;
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
# Set flag to check ids
#DetDescrCnvSvc.DoIdChecks = true;
# Set message level for IdDictTest
IdDictTest = Algorithm( "IdDictTest" )
IdDictTest.OutputLevel      = 2
# de-activate test for (groups of) subdetectors
#IdDictTest.print_ids= false;
#IdDictTest.atlas    = false;
#IdDictTest.indet    = false;
#IdDictTest.pixel    = false;
#IdDictTest.sct      = false;
#IdDictTest.trt      = false;
#IdDictTest.calos    = false;                        
#IdDictTest.caloTT    = false;                        
#IdDictTest.emBarrel = false;                        
#IdDictTest.emEndcap = false;                        
#IdDictTest.hec      = false;                        
#IdDictTest.fcal     = false;                        
#IdDictTest.tile     = false;                        
#IdDictTest.muons    = false;                        
#IdDictTest.csc      = false;
#IdDictTest.mdt      = false;
#IdDictTest.rpc      = false;
#IdDictTest.tgc      = false;
#IdDictTest.helperViaMgr      = true;    // will work only when the LArIdManager converter is written
#IdDictTest.readHits = true;
