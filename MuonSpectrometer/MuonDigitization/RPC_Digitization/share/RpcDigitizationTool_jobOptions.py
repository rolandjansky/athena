include.block ("RPC_Digitization/RpcDigitizationTool_jobOptions.py")
from Digitization.DigitizationFlags import jobproperties
from RPC_Digitization.RPC_DigitizationConf import RpcDigitizationTool
RpcDigitizationTool = RpcDigitizationTool("RpcDigitizationTool", 
                                          RndmSvc = jobproperties.Digitization.rndmSvc.get_Value(),
                                          RndmEngine = "RPC_Digitization")

if not jobproperties.Digitization.doXingByXingPileUp(): # Algorithm approach
    ToolSvc += RpcDigitizationTool
    
    
    
#include.block ("RPC_Digitization/RpcDigitizationTool_jobOptions.py")
##
## Import RPC_Digitization job properties
##
#from Digitization.DigitizationFlags import jobproperties
#
#from RPC_Digitization.RPC_DigitizationConf import RpcDigitizationTool



#RpcDigitizationTool = RpcDigitizationTool(
#                      DeadTime    = 100,
#                      RndmSvc     = jobproperties.Digitization.rndmSvc.get_Value(),
#                      RndmEngine  = "RPC_Digitization" )
#
#ToolSvc += RpcDigitizationTool
#RpcDigitizationTool.PatchForRpcTime      = True    
#RpcDigitizationTool.PatchForRpcTimeShift = 5   
#RpcDigitizationTool.turnON_efficiency    = True
#RpcDigitizationTool.turnON_clustersize   = True
#RpcDigitizationTool.testbeam_clustersize = 0
#RpcDigitizationTool.ClusterSize1_2uncorr = 0
#RpcDigitizationTool.CutProjectedTracks   = 50
# 
#RpcDigitizationTool.RPCInfoFromDb        = True 
#RpcDigitizationTool.Efficiency_fromCOOL  = True 
#RpcDigitizationTool.ClusterSize_fromCOOL = True  
#RpcDigitizationTool.DumpFromDbFirst      = False
#
#RpcDigitizationTool.PanelId_OFF_fromlist = False  
#RpcDigitizationTool.PanelId_OK_fromlist  = False 
#
##Average efficiency sideA for stationname:2=BML,3=BMS,4=BOL,5=BOS,8=BMF,9=BOF,10=BOG
#RpcDigitizationTool.PhiAndEtaEff_A=[0.8661,0.8540,0.8417,0.8383,0.8461,0.7893,0.8708]
#RpcDigitizationTool.OnlyPhiEff_A=[0.0410,0.0337,0.0470,0.0476,0.0968,0.0307,0.0535]
#RpcDigitizationTool.OnlyEtaEff_A=[0.0307,0.0408,0.0455,0.0381,0.0209,0.1111,0.0316]
#
##Average efficiency side C for stationname:2=BML,3=BMS,4=BOL,5=BOS,8=BMF,9=BOF,10=BOG
#RpcDigitizationTool.PhiAndEtaEff_C=[0.8605,0.8234,0.8361,0.8421,0.8252,0.8481,0.8432]
#RpcDigitizationTool.OnlyPhiEff_C=[0.0378,0.0295,0.0482,0.0485,0.0796,0.0459,0.0814]
#RpcDigitizationTool.OnlyEtaEff_C=[0.0267,0.0462,0.0397,0.0376,0.0335,0.0369,0.0358]
#
##Average parameter for Cluster Size distribution side A for (stationname:2=BML,3=BMS,4=BOL,5=BOS,8=BMF,9=BOF,10=BOG x(eta, phi) 
##Parameters are: CS1 fraction, CS2 fraction, CS more than 2 fraction, CS more than 2 Mean (meanCStail = FirstClusterSizeInTail+Mean)
#RpcDigitizationTool.FracClusterSize1_A=[0.6484,0.6724,0.6325,0.7196,0.6568,0.6643,0.8053,0.5767,0.6454,0.6526,0.6924,0.5363,0.7146,0.6481]
#RpcDigitizationTool.FracClusterSize2_A=[0.2063,0.1950,0.1863,0.1759,0.1982,0.1941,0.1391,0.2771,0.2442,0.2250,0.2239,0.2775,0.2041,0.2431]
#RpcDigitizationTool.FracClusterSizeTail_A=[0.1452,0.1326,0.1812,0.1045,0.1451,0.1416,0.0556,0.1462,0.1104,0.1224,0.0837,0.1861,0.0813,0.1088]
#RpcDigitizationTool.MeanClusterSizeTail_A=[0.6619,0.6517,0.7484,0.4744,0.7177,0.7044,0.2462,0.6783,0.5415,0.5201,0.3761,1.0705,0.3543,0.4671]     
#
##Average parameter for Cluster Size distribution side C for (stationname:2=BML,3=BMS,4=BOL,5=BOS,8=BMF,9=BOF,10=BOG)x(eta, phi) 
##Parameters are: CS1 fraction, CS2 fraction, CS more than 2 fraction, CS more than 2 Mean (meanCStail = FirstClusterSizeInTail+Mean)
#RpcDigitizationTool.FracClusterSize1_C=[0.6368,0.6762,0.6290,0.7076,0.6624,0.6564,0.7819,0.5705,0.6532,0.6506,0.6641,0.5688,0.6851,0.6227]
#RpcDigitizationTool.FracClusterSize2_C=[0.2083,0.1909,0.1868,0.1811,0.2003,0.1930,0.1507,0.2797,0.2361,0.2237,0.2455,0.2610,0.2147,0.2594]
#RpcDigitizationTool.FracClusterSizeTail_C=[0.1550,0.1330,0.1842,0.1113,0.1373,0.1506,0.0674,0.1498,0.1107,0.1257,0.0904,0.1702,0.1002,0.1179]
#RpcDigitizationTool.MeanClusterSizeTail_C=[0.7116,0.6595,0.7679,0.4748,0.7029,0.7440,0.2829,0.7107,0.5601,0.5499,0.3955,0.9105,0.4809,0.5112]
#
##Printout Eff and CS average parameters
#RpcDigitizationTool.PrintCalibrationVector = False ; 
#
#
#RpcDigitizationTool.OutputLevel = INFO

