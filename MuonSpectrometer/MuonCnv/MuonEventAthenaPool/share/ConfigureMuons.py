# DetDescr 
include( "TDRDetDescrAthenaRoot/TDRDetDescrAthenaRoot_jobOptions.py" )

# use detector store conversion service
include( "MuonDetMgrDetDescrCnv/MuonDetMgrDetDescrCnv_joboptions.py" )

# Turn on the reading of the dictionary
DetDescrCnvSvc = Service( "DetDescrCnvSvc" )
DetDescrCnvSvc.DecodeIdDict = TRUE
DetDescrCnvSvc.MuonIDFileName  = "IdDictMuonSpectrometer_P.03.xml"
# load Libraries and Services
include( "MuonByteStreamCnvTest/MuonRdoDigit_jobOptions.py" )

