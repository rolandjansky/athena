include.block ("sTGC_Digitization/sTgcDigitizationTool_jobOptions.py")
#
# Import sTGC_Digitization job properties
#
from Digitization.DigitizationFlags import jobproperties

from sTGC_Digitization.sTGC_DigitizationConf import sTgcDigitizationTool
job.PileUpToolsAlg.PileUpTools += [ sTgcDigitizationTool("sTgcDigitizationTool") ]
sTgcDigitizationTool = job.PileUpToolsAlg.PileUpTools[ "sTgcDigitizationTool" ]
sTgcDigitizationTool.DeadTime    = 100
sTgcDigitizationTool.RndmSvc     = jobproperties.Digitization.rndmSvc.get_Value()
sTgcDigitizationTool.RndmEngine  = "sTGC_Digitization"
sTgcDigitizationTool.FirstXing = -100
sTgcDigitizationTool.LastXing = 100
sTgcDigitizationTool.PatchForsTgcTime      = True    
sTgcDigitizationTool.PatchForsTgcTimeShift = 5   
sTgcDigitizationTool.turnON_efficiency    = True
sTgcDigitizationTool.turnON_clustersize   = True
sTgcDigitizationTool.testbeam_clustersize = 0
sTgcDigitizationTool.ClusterSize1_2uncorr = 0
sTgcDigitizationTool.CutProjectedTracks   = 50
 
sTgcDigitizationTool.sTGCInfoFromDb        = True 
sTgcDigitizationTool.Efficiency_fromCOOL  = True 
sTgcDigitizationTool.ClusterSize_fromCOOL = True  
sTgcDigitizationTool.DumpFromDbFirst      = False

sTgcDigitizationTool.PanelId_OFF_fromlist = False 
sTgcDigitizationTool.PanelId_OK_fromlist  = False  

#Average efficiency sideA for stationname:2=BML,3=BMS,4=BOL,5=BOS,8=BMF,9=BOF,10=BOG
sTgcDigitizationTool.PhiAndEtaEff_A=[0.8661,0.8540,0.8417,0.8383,0.8461,0.7893,0.8708]
sTgcDigitizationTool.OnlyPhiEff_A=[0.0410,0.0337,0.0470,0.0476,0.0968,0.0307,0.0535]
sTgcDigitizationTool.OnlyEtaEff_A=[0.0307,0.0408,0.0455,0.0381,0.0209,0.1111,0.0316]

#Average efficiency side C for stationname:2=BML,3=BMS,4=BOL,5=BOS,8=BMF,9=BOF,10=BOG
sTgcDigitizationTool.PhiAndEtaEff_C=[0.8605,0.8234,0.8361,0.8421,0.8252,0.8481,0.8432]
sTgcDigitizationTool.OnlyPhiEff_C=[0.0378,0.0295,0.0482,0.0485,0.0796,0.0459,0.0814]
sTgcDigitizationTool.OnlyEtaEff_C=[0.0267,0.0462,0.0397,0.0376,0.0335,0.0369,0.0358]

#Average parameter for Cluster Size distribution side A for (stationname:2=BML,3=BMS,4=BOL,5=BOS,8=BMF,9=BOF,10=BOG x(eta, phi) 
#Parameters are: CS1 fraction, CS2 fraction, CS more than 2 fraction, CS more than 2 Mean (meanCStail = FirstClusterSizeInTail+Mean)
sTgcDigitizationTool.FracClusterSize1_A=[0.6484,0.6724,0.6325,0.7196,0.6568,0.6643,0.8053,0.5767,0.6454,0.6526,0.6924,0.5363,0.7146,0.6481]
sTgcDigitizationTool.FracClusterSize2_A=[0.2063,0.1950,0.1863,0.1759,0.1982,0.1941,0.1391,0.2771,0.2442,0.2250,0.2239,0.2775,0.2041,0.2431]
sTgcDigitizationTool.FracClusterSizeTail_A=[0.1452,0.1326,0.1812,0.1045,0.1451,0.1416,0.0556,0.1462,0.1104,0.1224,0.0837,0.1861,0.0813,0.1088]
sTgcDigitizationTool.MeanClusterSizeTail_A=[0.6619,0.6517,0.7484,0.4744,0.7177,0.7044,0.2462,0.6783,0.5415,0.5201,0.3761,1.0705,0.3543,0.4671]     

#Average parameter for Cluster Size distribution side C for (stationname:2=BML,3=BMS,4=BOL,5=BOS,8=BMF,9=BOF,10=BOG)x(eta, phi) 
#Parameters are: CS1 fraction, CS2 fraction, CS more than 2 fraction, CS more than 2 Mean (meanCStail = FirstClusterSizeInTail+Mean)
sTgcDigitizationTool.FracClusterSize1_C=[0.6368,0.6762,0.6290,0.7076,0.6624,0.6564,0.7819,0.5705,0.6532,0.6506,0.6641,0.5688,0.6851,0.6227]
sTgcDigitizationTool.FracClusterSize2_C=[0.2083,0.1909,0.1868,0.1811,0.2003,0.1930,0.1507,0.2797,0.2361,0.2237,0.2455,0.2610,0.2147,0.2594]
sTgcDigitizationTool.FracClusterSizeTail_C=[0.1550,0.1330,0.1842,0.1113,0.1373,0.1506,0.0674,0.1498,0.1107,0.1257,0.0904,0.1702,0.1002,0.1179]
sTgcDigitizationTool.MeanClusterSizeTail_C=[0.7116,0.6595,0.7679,0.4748,0.7029,0.7440,0.2829,0.7107,0.5601,0.5499,0.3955,0.9105,0.4809,0.5112]

#Printout Eff and CS average parameters
sTgcDigitizationTool.PrintCalibrationVector = False ; 


sTgcDigitizationTool.OutputLevel = INFO

## Database interface
#--------------------------------------------------------------
# Load IOVDbSvc
#--------------------------------------------------------------
IOVDbSvc = Service("IOVDbSvc")
from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
from IOVDbSvc.CondDB import conddb
IOVDbSvc.OutputLevel = 3
#IOVDbSvc.DBInstance="COOLOFL_DCS"
include("RegistrationServices/IOVRegistrationSvc_jobOptions.py")

##-----------------------------------------------------------------

from MuonCondSvc.MuonCondSvcConf import sTGC_STATUSConditionsSvc
ServiceMgr +=sTGC_STATUSConditionsSvc()

from MuonCondTool.MuonCondToolConf import sTgcDetectorStatusDbTool
sTgcDetectorStatusDbTool = sTgcDetectorStatusDbTool("sTgcDetectorStatusDbTool")
sTgcDetectorStatusDbTool.OutputLevel = INFO
MessageSvc.OutputLevel = INFO
#sTgcDetectorStatusDbTool.sTgcDqFolder = "/OFFLINE/FINAL"
sTgcDetectorStatusDbTool.sTgcDqFolder = "/sTGC/DQMF/ELEMENT_STATUS"
ToolSvc += sTgcDetectorStatusDbTool

#dbConn_sTgc="oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_sTGC;dbname=OFLP200"
dbConn_sTgc="COOLOFL_sTGC/OFLP200"
#dbConn_sTgc="oracle://intr;schema=ATLAS_COOL_sTGCDQ;dbname=sTGC_DQA"
#dbConn_sTgc="sqlite://;schema=sTGCConditionDB_MC.db;dbname=sTGC_DQA"
#dbConn_sTgc="sqlite://;schema=sTGCConditionDB_gab2.db;dbname=sTGC_DQA"
#dbConn_sTgc="sqlite://;schema=sTGCConditionDB_gab3.db;dbname=sTGC_DQA"
#dbConn_sTgc="sqlite://;schema=sTGCConditionDB_2010_Aug_20.db;dbname=sTGC_DQA"
#folder_sTgc="/OFFLINE/FINAL"
folder_sTgc="/sTGC/DQMF/ELEMENT_STATUS"

ServiceMgr.IOVDbSvc.dbConnection=dbConn_sTgc
#ServiceMgr.IOVDbSvc.Folders+=[folder_sTgc+" <tag>MC</tag> <dbConnection>"+dbConn_sTgc+"</dbConnection>"]
#ServiceMgr.IOVDbSvc.Folders+=[folder_sTgc+" <tag>RecoCondDB</tag> <dbConnection>"+dbConn_sTgc+"</dbConnection>"]
ServiceMgr.IOVDbSvc.Folders+=[folder_sTgc+" <tag>sTGCConditionDB_2010_Aug_20</tag> <dbConnection>"+dbConn_sTgc+"</dbConnection>"]

from MuonCondSvc.MuonCondSvcConf import sTGCCondSummarySvc
ServiceMgr +=sTGCCondSummarySvc()


sTGCCondSummary=ServiceMgr.sTGCCondSummarySvc
sTGCCondSummary.ConditionsServices=["sTGC_STATUSConditionsSvc"]

#
# set rndm seeds
#
try:
    jobproperties.Digitization.rndmSeedList.addSeed(sTgcDigitizationTool.RndmEngine, 49261510, 105132394 )
except AttributeError:
    jobproperties.Digitization.rndmSeedList.addSeed(sTgcDigitizationTool.getDefaultProperty('RndmEngine'), 49261510, 105132394 )
