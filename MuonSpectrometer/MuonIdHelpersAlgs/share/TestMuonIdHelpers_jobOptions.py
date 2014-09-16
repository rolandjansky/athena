###############################################################
#
# // AthExStoreGateExample Job options file reading ZebraTDR events
#
#==============================================================
#use ZebraEventSelector
theApp.setup( ZEBRA )

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
# Turn on the reading of the dictionary
DetDescrCnvSvc = Service( "DetDescrCnvSvc" )
DetDescrCnvSvc.DecodeIdDict = TRUE
# Set xml file to read in for each system 
#  - only needed if default is not ok
#DetDescrCnvSvc.AtlasIDFileName = "IdDictATLAS.xml";
#DetDescrCnvSvc.InDetIDFileName = "IdDictInnerDetector.xml";
#DetDescrCnvSvc.LArIDFileName   = "IdDictLArCalorimeter.xml";
#DetDescrCnvSvc.TileIDFileName  = "IdDictTileCalorimeter.xml";
#DetDescrCnvSvc.LVL1IDFileName  = "IdDictLVL1Calorimeter.xml";
# set the appropriate xml file for the Muons
#DetDescrCnvSvc.MuonIDFileName  = "IdDictMuonSpectrometer_M2.8.xml";
DetDescrCnvSvc.MuonIDFileName  = "IdDictMuonSpectrometer_P.03.xml"
#DetDescrCnvSvc.MuonIDFileName  = "IdDictMuonSpectrometer_H8_2002.xml";
#DetDescrCnvSvc.MuonIDFileName  = "IdDictMuonSpectrometer_H8_2003.xml";
# One may turn off the read of one of more system iddict files
#DetDescrCnvSvc.InDetIDFileName = "NULL";
#DetDescrCnvSvc.LArIDFileName   = "NULL";
#DetDescrCnvSvc.TileIDFileName  = "NULL";
#DetDescrCnvSvc.LVL1IDFileName  = "NULL";
#DetDescrCnvSvc.MuonIDFileName  = "NULL";
# Provide dictionary file
#DetDescrCnvSvc.IdDictName = "ATLAS_IDS.xml";
# use detector store conversion service
include( "MuonDetMgrDetDescrCnv/MuonDetMgrDetDescrCnv_joboptions.py" )

#load relevant libraries
theApp.Dlls += [ "MuonDigitZebraCnv", "MuonIdHelpersAlgs" ]
#top algorithms to be run
theApp.TopAlg = [ "TestMuonIdHelpers" ]
#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc.OutputLevel      = 3
StoreGateSvc = Service( "StoreGateSvc" )
StoreGateSvc.Dump = FALSE  #true will dump data store contents
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtSel = "EventSelector"
theApp.EvtMax = 100
EventSelector.firstRun=1
EventSelector.lastRun=10000
EventSelector.directConversion = TRUE
# H->eemumu
#EventSelector.dataSetNames = {"Y01606.1"} ;
EventSelector.readHits = FALSE                        
EventSelector.readDigits = FALSE                        
EventSelector.calos = FALSE                        
EventSelector.muons = FALSE                        
EventSelector.mdt   = FALSE                        
EventSelector.rpc   = FALSE                        
EventSelector.tgc   = FALSE                        
EventSelector.trt   = FALSE
EventSelector.sct   = FALSE
EventSelector.pixel = FALSE
#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------
TestMuonIdHelpers = Algorithm( "TestMuonIdHelpers" )
TestMuonIdHelpers.testMDT = TRUE
TestMuonIdHelpers.testCSC = TRUE
TestMuonIdHelpers.testRPC = TRUE
TestMuonIdHelpers.testTGC = TRUE
#==============================================================
#
# End of AthExStoreGateExample job options file
#
###############################################################
