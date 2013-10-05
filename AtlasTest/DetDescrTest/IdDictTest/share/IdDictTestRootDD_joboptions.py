#
# Test joboptions for the loading of the DetectorStore
#
# For MC EventSelector:
theApp.setup( MONTECARLO )

theApp.Dlls   += [ "RootSvcModules" ]; 
theApp.ExtSvc += [ "RootSvc" ]
# For ddexamples and ddcnvsvc
##include "IdDictDetDescrCnv/IdDictDetDescrCnv_joboptions.txt"
include( "InDetMgrDetDescrCnv/InDetMgrDetDescrCnv_joboptions.py" )

include( "TileDetMgrDetDescrCnv/TileDetMgrDetDescrCnv_jobOptions.py" )

include( "MuonDetMgrDetDescrCnv/MuonDetMgrDetDescrCnv_joboptions.py" )

include( "LArDetMgrDetDescrCnv/LArDetMgrDetDescrCnv_joboptions.py" )

# CaloDetMgrDetDescrCnv included in LArDetMgrDetDescrCnv
##include "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.txt"
# Root DD:
include( "TDRDetDescrAthenaRoot/TDRDetDescrAthenaRoot_jobOptions.py" )

# Turn on the reading of the dictionary
DetDescrCnvSvc = Service( "DetDescrCnvSvc" )
DetDescrCnvSvc.DecodeIdDict = TRUE
RootDetDescrSvc = Service( "RootDetDescrSvc" )
RootDetDescrSvc.read=TRUE
DetDescrCnvSvc.ReadFromROOT = TRUE 
# DLLs 
theApp.Dlls += [ "IdDictTest" ]
# Algs
theApp.TopAlg += [ "IdDictTest" ]
#--------------------------------------------------------------
# Set output level threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc.OutputLevel      = 2
# Provide dictionary file
#DetDescrCnvSvc.IdDictName = "ATLAS_IDS.xml";
# Number of events to be processed (default is 10)
theApp.EvtMax = 2
# Input file 
#ApplicationMgr.EvtSel = "FILE ZEBRA.P";
