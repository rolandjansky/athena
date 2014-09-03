###################################################################
#
# Job options file for accessing LArRawConditions objects from COOL
# (H6-G4 version)
#==================================================================

include.block ( "LArConditionsCommon/LArConditionsCommon_H6G4_jobOptions.py" )

# Set up db connection 
LArH6DBServer     = "ATLAS_COOLPROD"
LArH6DataBase     = "TBP200"      #  DB name

#LArH6DBConnection = " <dbConnection>impl=cool;techno=oracle;schema=ATLAS_COOL_LAR;"+LArH6DBServer+":"+LArH6DataBase+":ATLAS_COOL_READER:</dbConnection> "
LArH6DBConnection = " <dbConnection>sqlite://;schema=sqlite200/TBP200.db;dbname="+LArH6DataBase+"</dbConnection> "
#LArH6DBConnection = CondDBCool.LAR


#include ("LArRawConditions/LArRawConditionsDict_joboptions.py")
include( "LArCondAthenaPool/LArCondAthenaPool_joboptions.py" )

#--------------------------------------------------------------
# Access to IOVSvc and IOVDbSvc
# Must list the folders to be used for reading
#--------------------------------------------------------------


LArEC04IOVDbTag="EC04-5"


LArEC04FolderList=[
#    "/LAR/LArElecCalibDC2/LArAutoCorr<tag>LArH6G4LArAutoCorrDC2-",
#    "/LAR/LArElecCalibDC2/LArDAC2uA<tag>LArH6G4LArDAC2uADC2-",  
    "/LAR/LArElecCalibDC2/LArfSampl<tag>LArH6G4fSamplDC2-",     
#    "/LAR/LArElecCalibDC2/LArNoise<tag>LArH6G4NoiseDC2-",      
    "/LAR/LArElecCalibDC2/LArPedestal<tag>LArH6G4PedestalDC2-",   
#    "/LAR/LArElecCalibDC2/LArRamp<tag>LArH6G4RampDC2-",       
    "/LAR/LArElecCalibDC2/LArShape<tag>LArH6G4LArShapeDC2-",   
#    "/LAR/LArElecCalibDC2/LAruA2MeV<tag>LArH6G4LAruA2MeVDC2-",
    ]

LArEC04FolderList+=[
    "/LAR/LArElecCalibEC04/LArNoise<tag>LArH6G4Noise-",
#    "/LAR/LArElecCalibEC04/LArPedestal<tag>LArH6Pedestal-",
    "/LAR/LArElecCalibEC04/LArAutoCorr<tag>LArH6AutoCorr-",
    "/LAR/LArElecCalibEC04/LArDAC2uA<tag>LArH6DAC2uA-",
    "/LAR/LArElecCalibEC04/LArRamp<tag>LArH6Ramp-",
    "/LAR/LArElecCalibEC04/LAruA2MeV<tag>LArH6uA2MeV-"
    ]

LArEC04FolderList+=[
    "/LAR/LArElecCalibEC04/LArOFC<tag>LArH6OFC-",      
#    "/LAR/LArElecCalibEC04/LArRampOld<tag>LArH6G4RampOld-",      
#    "/LAR/LArElecCalibEC04/LArMphysOverMcal<tag>LArH6G4MphysOverMcal-"
    ]

for i in range(len(LArEC04FolderList)) :
    LArEC04FolderList[i] += LArEC04IOVDbTag + '</tag>' + LArH6DBConnection
#    LArEC04FolderList[i] += LArEC04IOVDbTag + '</tag>'


from IOVDbSvc.CondDB import conddb
for f in LArEC04FolderList :
#   conddb.addFolder("LAR",f)
   conddb.addFolder("",f)

