###################################################################
#
# Job options file for accessing LArRawConditions objects from COOL
# (H6 version)
#==================================================================

include.block ( "LArConditionsCommon/LArConditionsCommon_H6_jobOptions.py" )

# Set up db connection 
LArH6DBServer     = "ATLAS_COOLPROD"
LArTBH6DataBase     = "TBP200"      #  DB name

#LArH6DBConnection = " <dbConnection>impl=cool;techno=oracle;schema=ATLAS_COOL_LAR;"+LArH6DBServer+":"+LArH6DataBase+":ATLAS_COOL_READER:</dbConnection> "
LArH6DBConnection = " <dbConnection>sqlite://;schema=sqlite200/TBP200.db;dbname="+LArTBH6DataBase+"</dbConnection> "
#LArH6DBConnection = CondDBCool.LAR


#include ("LArRawConditions/LArRawConditionsDict_joboptions.py")
include( "LArCondAthenaPool/LArCondAthenaPool_joboptions.py" )

#--------------------------------------------------------------
# Access to IOVSvc and IOVDbSvc
# Must list the folders to be used for reading
#--------------------------------------------------------------


LArEC04IOVDbTag="EC04-5"


LArEC04FolderList=["/LAR/LArElecCalibEC04/LArPedestal<tag>LArH6Pedestal-",
                   "/LAR/LArElecCalibEC04/LArNoise<tag>LArH6G4Noise-",
                   "/LAR/LArElecCalibEC04/LArAutoCorr<tag>LArH6AutoCorr-",
                   "/LAR/LArElecCalibEC04/LArDAC2uA<tag>LArH6DAC2uA-",
                   "/LAR/LArElecCalibEC04/LArPhysWaveContainer/LArPhysWave<tag>LArH6PhysWave-",
                   "/LAR/LArElecCalibEC04/LArShapeComplete/LArPhysWave<tag>LArH6Shape-"]
        
if doUseRampBuilder:
    LArEC04FolderList +=["/LAR/LArElecCalibEC04/LArRampBuilder<tag>LArH6RampBuilder-"]
    LArEC04FolderList +=["/LAR/LArElecCalibEC04/LAruA2MeVBuilder<tag>LArH6uA2MeVBuilder-"]
    LArEC04FolderList +=["/LAR/LArElecCalibEC04/LArMphysOverMcal<tag>LArH6G4MphysOverMcal-"]
#    Could not be here, LArADC2MeVTool not yet defined
#    ToolSvc.LArADC2MeVTool.keyADC2DAC = "LArRampBuilder"
#    ToolSvc.LArADC2MeVTool.keyuA2MeV = "LAruA2MeVBuilder"
#    ToolSvc.LArADC2MeVTool.UseMphysOverMcal = True
else:
    LArEC04FolderList +=["/LAR/LArElecCalibEC04/LArRamp<tag>LArH6Ramp-"]
    LArEC04FolderList +=["/LAR/LArElecCalibEC04/LAruA2MeV<tag>LArH6uA2MeV-"]
#    ToolSvc.LArADC2MeVTool.UseMphysOverMcal = False
    
if doOFCPhysShape:
    LArEC04FolderList +=["/LAR/LArElecCalibEC04/LArOFCPhys/LArOFCPhysShape<tag>LArH6OFCPhys-"]
else:
    LArEC04FolderList +=["/LAR/LArElecCalibEC04/LArOFC<tag>LArH6OFC-"]
    LArEC04FolderList +=["/LAR/LArElecCalibEC04/LArFEBTimeOffset<tag>LArH6FEBTimeOffset-"]

if doOscillationCorrection:
    LArEC04FolderList +=["/LAR/LArElecCalibEC04/LArH6Oscillation<tag>LArH6Oscillation-"]
 
for i in range(len(LArEC04FolderList)) :
    LArEC04FolderList[i] += LArEC04IOVDbTag + '</tag>'+LArH6DBConnection 
#    LArEC04FolderList[i] += LArEC04IOVDbTag + '</tag>'
                
from IOVDbSvc.CondDB import conddb
for f in LArEC04FolderList :
   conddb.addFolder("",f)

