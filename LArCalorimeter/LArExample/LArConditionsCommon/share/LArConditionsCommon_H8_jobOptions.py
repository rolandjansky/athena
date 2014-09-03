###################################################################
#
# Job options file for accessing LArRawConditions objects from COOL
# (H8 version)
#==================================================================

include.block ( "LArConditionsCommon/LArConditionsCommon_H8_jobOptions.py" )

# LArH8DBConnection = CondDBCool.LAR

#include ("LArRawConditions/LArRawConditionsDict_joboptions.py")
include( "LArCondAthenaPool/LArCondAthenaPool_joboptions.py" )


#--------------------------------------------------------------
# Access to IOVSvc and IOVDbSvc
# Must list the folders to be used for reading
#--------------------------------------------------------------


# Set folders default tag if none is defined
#
if not 'LArTB04FolderTag_Calib'    in dir():
	LArTB04FolderTag_Calib     = "TB04-Default"
if not 'LArTB04FolderTag_Pedestal' in dir():       
	LArTB04FolderTag_Pedestal  = "TB04-Default"
if not 'LArTB04FolderTag_Ramp'     in dir():
	LArTB04FolderTag_Ramp      = "TB04-Default"
if not 'LArTB04FolderTag_OFC_RTM'  in dir():
	LArTB04FolderTag_OFC_RTM   = "TB04-Default"
if not 'LArTB04FolderTag_OFC_TCM'  in dir():
	LArTB04FolderTag_OFC_TCM   = "TB04-Default"
if not 'LArTB04FolderTag_MphyMcal' in dir():
	LArTB04FolderTag_MphyMcal  = "TB04-Xtlk1"
if not 'LArTB04FolderTag_MphyMcal' in dir():
        LArTB04FolderTag_MphyMcal  = "TB04-Xtlk1"

LArH8FolderList = [#"/LAR/LArElecCalibH8/LArCalibParams<tag>LArCalibParams-"+LArTB04FolderTag_Calib+"</tag>",
                   "/LAR/LArElecCalibH8/LArPedestal<tag>LArPedestal-"+LArTB04FolderTag_Pedestal+"</tag>",
                   "/LAR/LArElecCalibH8/LArAutoCorr<tag>LArAutoCorr-"+LArTB04FolderTag_Pedestal+"</tag>",
                   "/LAR/LArElecCalibH8/LArRamp<tag>LArRamp-"+LArTB04FolderTag_Ramp+"</tag>",
                   "/LAR/LArElecCalibH8/LAruA2MeV<tag>LAruA2MeV-"+LArTB04FolderTag_Ramp+"</tag>",
                   "/LAR/LArElecCalibH8/LArDAC2uA<tag>LArDAC2uA-"+LArTB04FolderTag_Ramp+"</tag>",
                   "/LAR/LArElecCalibH8/LArMphysOverMcal<tag>LArMphysOverMcal-"+LArTB04FolderTag_MphyMcal+"</tag>"
		   ]

if 'useRTMOFC' in dir() and useRTMOFC:
   LArH8FolderList +=[
     "/LAR/LArElecCalibH8/LArOFCPhys/RTM<tag>LArOFCPhysRTM-"+LArTB04FolderTag_OFC_RTM+"</tag>"#,
#     "/LAR/LArElecCalibH8/LArShape/RTM<tag>LArShapeRTM-"+LArTB04FolderTag_OFC_RTM+"</tag>"
     ]

if 'useTCMOFC' in dir() and useTCMOFC:
  LArH8FolderList +=[
    "/LAR/LArElecCalibH8/LArOFCPhys/TCM<tag>LArOFCPhysTCM-"+LArTB04FolderTag_OFC_TCM+"</tag>"#,
#    "/LAR/LArElecCalibH8/LArShape/TCM<tag>LArShapeTCM-"+LArTB04FolderTag_OFC_TCM+"</tag>"
    ]

#for i in range(len(LArH8FolderList)):
#  LArH8FolderList[i]+=LArH8DBConnection


from IOVDbSvc.CondDB import conddb
for f in LArH8FolderList :
   conddb.addFolder("LAR",f)

PoolSvc = Service( "PoolSvc" )
if not (hasattr(PoolSvc.ReadCatalog,'__len__') and "prfile:poolcond/PoolCat_oflcond.xml" in PoolSvc.ReadCatalog):
   PoolSvc.ReadCatalog+= [ "prfile:poolcond/PoolCat_oflcond.xml" ]
