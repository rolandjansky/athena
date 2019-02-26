# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from Digitization.DigitizationFlags import jobproperties
from AthenaCommon import CfgMgr

# The earliest bunch crossing time for which interactions will be sent
# to the RpcDigitizationTool.
def RPC_FirstXing():
    return -150

# The latest bunch crossing time for which interactions will be sent
# to the RpcDigitizationTool.
def RPC_LastXing():
    return 125

def getRpcRange(name="RpcRange", **kwargs):
    # bunch crossing range in ns
    kwargs.setdefault('FirstXing', RPC_FirstXing() )
    kwargs.setdefault('LastXing',  RPC_LastXing() )
    kwargs.setdefault('CacheRefreshFrequency', 1.0 ) #default 0 no dataproxy reset
    kwargs.setdefault('ItemList', ["RPCSimHitCollection#RPC_Hits"] )
    return CfgMgr.PileUpXingFolder(name, **kwargs)


def RpcDigitizationTool(name="RpcDigitizationTool", **kwargs):
    if jobproperties.Digitization.doXingByXingPileUp(): # PileUpTool approach
        # This should match the range for the RPC in Simulation/Digitization/share/MuonDigitization.py 
        kwargs.setdefault("FirstXing", RPC_FirstXing() ) 
        kwargs.setdefault("LastXing",  RPC_LastXing() ) 
    kwargs.setdefault("DeadTime", 100)  
    kwargs.setdefault("PatchForRpcTime"       ,True  )	    
    # kwargs.setdefault("PatchForRpcTimeShift"  ,9.6875)  
    kwargs.setdefault("PatchForRpcTimeShift"           ,12.5)  
    kwargs.setdefault("turnON_efficiency"              ,True  )  
    kwargs.setdefault("turnON_clustersize"             ,True  )  
    kwargs.setdefault("testbeam_clustersize"           ,0     )  
    kwargs.setdefault("ClusterSize1_2uncorr"           ,0     ) 
    kwargs.setdefault("CutProjectedTracks"             ,100   ) 
    kwargs.setdefault("RPCInfoFromDb"                  ,True  )  
    kwargs.setdefault("Efficiency_fromCOOL"            ,True  )
    kwargs.setdefault("EfficiencyPatchForBMShighEta"   ,False )   
    kwargs.setdefault("ClusterSize_fromCOOL"           ,True  )  
    kwargs.setdefault("DumpFromDbFirst"                ,False )  
    kwargs.setdefault("PanelId_OFF_fromlist"           ,False )  
    kwargs.setdefault("PanelId_OK_fromlist"            ,False ) 
    kwargs.setdefault("IgnoreRunDependentConfig"       ,False ) 
 
    #Printout Eff and CS average parameters
    kwargs.setdefault("PrintCalibrationVector",False )
    ############# average detector status parameters for 2010 data 
    ##Average efficiency sideA for stationname:2=BML,3=BMS,4=BOL,5=BOS,8=BMF,9=BOF,10=BOG
    #kwargs.setdefault("PhiAndEtaEff_A",[0.8661,0.8540,0.8417,0.8383,0.8461,0.7893,0.8708] )                                                                #parameters for 2010 data 
    #kwargs.setdefault("OnlyPhiEff_A"  ,[0.0410,0.0337,0.0470,0.0476,0.0968,0.0307,0.0535] )                                                                #parameters for 2010 data 
    #kwargs.setdefault("OnlyEtaEff_A"  ,[0.0307,0.0408,0.0455,0.0381,0.0209,0.1111,0.0316] )                                                                #parameters for 2010 data 
    # 
    ##Average efficiency side C for stationname:2=BML,3=BMS,4=BOL,5=BOS,8=BMF,9=BOF,10=BOG
    #kwargs.setdefault("PhiAndEtaEff_C",[0.8605,0.8234,0.8361,0.8421,0.8252,0.8481,0.8432] )                                                                #parameters for 2010 data 
    #kwargs.setdefault("OnlyPhiEff_C"  ,[0.0378,0.0295,0.0482,0.0485,0.0796,0.0459,0.0814] )                                                                #parameters for 2010 data 
    #kwargs.setdefault("OnlyEtaEff_C"  ,[0.0267,0.0462,0.0397,0.0376,0.0335,0.0369,0.0358] )                                                                #parameters for 2010 data 
    #
    ##Average parameter for Cluster Size distribution side A for (stationname:2=BML,3=BMS,4=BOL,5=BOS,8=BMF,9=BOF,10=BOG x(eta, phi) 
    ##Parameters are: CS1 fraction, CS2 fraction, CS more than 2 fraction, CS more than 2 Mean (meanCStail = FirstClusterSizeInTail+Mean)
    #kwargs.setdefault("FracClusterSize1_A"   ,[0.6484,0.6724,0.6325,0.7196,0.6568,0.6643,0.8053,0.5767,0.6454,0.6526,0.6924,0.5363,0.7146,0.6481] )        #parameters for 2010 data 
    #kwargs.setdefault("FracClusterSize2_A"   ,[0.2063,0.1950,0.1863,0.1759,0.1982,0.1941,0.1391,0.2771,0.2442,0.2250,0.2239,0.2775,0.2041,0.2431] )        #parameters for 2010 data 
    #kwargs.setdefault("FracClusterSizeTail_A",[0.1452,0.1326,0.1812,0.1045,0.1451,0.1416,0.0556,0.1462,0.1104,0.1224,0.0837,0.1861,0.0813,0.1088] )        #parameters for 2010 data 
    #kwargs.setdefault("MeanClusterSizeTail_A",[0.6619,0.6517,0.7484,0.4744,0.7177,0.7044,0.2462,0.6783,0.5415,0.5201,0.3761,1.0705,0.3543,0.4671] )        #parameters for 2010 data 
    #
    ##Average parameter for Cluster Size distribution side C for (stationname:2=BML,3=BMS,4=BOL,5=BOS,8=BMF,9=BOF,10=BOG)x(eta, phi) 
    ##Parameters are: CS1 fraction, CS2 fraction, CS more than 2 fraction, CS more than 2 Mean (meanCStail = FirstClusterSizeInTail+Mean)
    #kwargs.setdefault("FracClusterSize1_C"   ,[0.6368,0.6762,0.6290,0.7076,0.6624,0.6564,0.7819,0.5705,0.6532,0.6506,0.6641,0.5688,0.6851,0.6227] )        #parameters for 2010 data 
    #kwargs.setdefault("FracClusterSize2_C"   ,[0.2083,0.1909,0.1868,0.1811,0.2003,0.1930,0.1507,0.2797,0.2361,0.2237,0.2455,0.2610,0.2147,0.2594] )        #parameters for 2010 data 
    #kwargs.setdefault("FracClusterSizeTail_C",[0.1550,0.1330,0.1842,0.1113,0.1373,0.1506,0.0674,0.1498,0.1107,0.1257,0.0904,0.1702,0.1002,0.1179] )        #parameters for 2010 data 
    #kwargs.setdefault("MeanClusterSizeTail_C",[0.7116,0.6595,0.7679,0.4748,0.7029,0.7440,0.2829,0.7107,0.5601,0.5499,0.3955,0.9105,0.4809,0.5112] )        #parameters for 2010 data 
    

    ###### for MC15  #from  https://cds.cern.ch/record/1648012? (distribution peak)
    kwargs.setdefault("PhiAndEtaEff_A"    ,[0.938, 0.938, 0.938, 0.938, 0.938, 0.938, 0.938, 0.938, 0.938]) 
    kwargs.setdefault("OnlyPhiEff_A"      ,[0.022, 0.022, 0.022, 0.022, 0.022, 0.022, 0.022, 0.022, 0.022]) 
    kwargs.setdefault("OnlyEtaEff_A"      ,[0.022, 0.022, 0.022, 0.022, 0.022, 0.022, 0.022, 0.022, 0.022]) 
    kwargs.setdefault("PhiAndEtaEff_C"    ,[0.938, 0.938, 0.938, 0.938, 0.938, 0.938, 0.938, 0.938, 0.938]) 
    kwargs.setdefault("OnlyPhiEff_C"      ,[0.022, 0.022, 0.022, 0.022, 0.022, 0.022, 0.022, 0.022, 0.022]) 
    kwargs.setdefault("OnlyEtaEff_C"      ,[0.022, 0.022, 0.022, 0.022, 0.022, 0.022, 0.022, 0.022, 0.022]) 
    kwargs.setdefault("FracClusterSize1_A",   [0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664])
    kwargs.setdefault("FracClusterSize2_A",   [0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986])
    kwargs.setdefault("FracClusterSizeTail_A",[0.13035,  0.13035,  0.13035,  0.13035,  0.13035,  0.13035,  0.13035,  0.13035,  0.13035, 0.13035,  0.13035,  0.13035,  0.13035,  0.13035,  0.13035,  0.13035,  0.13035,  0.13035 ]) 
    kwargs.setdefault("MeanClusterSizeTail_A",[0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598])
    kwargs.setdefault("FracClusterSize1_C",   [0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664, 0.609664])
    kwargs.setdefault("FracClusterSize2_C",   [0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986, 0.259986])
    kwargs.setdefault("FracClusterSizeTail_C",[0.13035,  0.13035,  0.13035,  0.13035,  0.13035,  0.13035,  0.13035,  0.13035,  0.13035, 0.13035,  0.13035,  0.13035,  0.13035,  0.13035,  0.13035,  0.13035,  0.13035,  0.13035 ]) 
    kwargs.setdefault("MeanClusterSizeTail_C",[0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598, 0.548598])


    ###### used in MC11 and MC12 
    ############# average detector status parameters for 2011 data - computed on Feb 17th 2012  ---- extended with full. eff. numbers for 53=BME and 54=BOE
    ##Average efficiency sideA for stationname:2=BML,3=BMS,4=BOL,5=BOS,8=BMF,9=BOF,10=BOG                                                                                                         
#    kwargs.setdefault("PhiAndEtaEff_A"    ,[0.883016,  0.859873,  0.906754,  0.909968,  0.789349,  0.805422, 0.903208, 1., 1.]  )                #parameters for 2011 data  - computed on Feb 17th 2012  
#    kwargs.setdefault("OnlyPhiEff_A"      ,[0.0431442, 0.0475649, 0.0303454, 0.0336113, 0.132593,  0.026511, 0.0483751, 0., 0.] )                #parameters for 2011 data  - computed on Feb 17th 2012  
#    kwargs.setdefault("OnlyEtaEff_A"      ,[0.0325655, 0.0391418, 0.0357217, 0.0247513, 0.0422091, 0.115178, 0.027125, 0., 0.]  )                #parameters for 2011 data  - computed on Feb 17th 2012  
#    ##Average efficiency sideA for stationname:2=BML,3=BMS,4=BOL,5=BOS,8=BMF,9=BOF,10=BOG                                                                                                         
#    kwargs.setdefault("PhiAndEtaEff_A"    ,[0.883016,  0.859873,  0.906754,  0.909968,  0.789349,  0.805422, 0.903208]  )                #parameters for 2011 data  - computed on Feb 17th 2012  
#    kwargs.setdefault("OnlyPhiEff_A"      ,[0.0431442, 0.0475649, 0.0303454, 0.0336113, 0.132593,  0.026511, 0.0483751] )                #parameters for 2011 data  - computed on Feb 17th 2012  
#    kwargs.setdefault("OnlyEtaEff_A"      ,[0.0325655, 0.0391418, 0.0357217, 0.0247513, 0.0422091, 0.115178, 0.027125]  )                #parameters for 2011 data  - computed on Feb 17th 2012  
    ########## the above numbers correspond to  the following measured eff.
    ##"GapEff_A"     [0.958726,0.94658,0.97282,0.96833,0.964151,0.947111,0.978708]                                                        #parameters for 2011 data  - computed on Feb 17th 2012  
    ##"PhiEff_A"     [0.92616,0.907438,0.937099,0.94358,0.921942,0.831933,0.951583]                                                       #parameters for 2011 data  - computed on Feb 17th 2012  
    ##"EtaEff_A"     [0.915582,0.899015,0.942476,0.93472,0.831558,0.9206,0.930333]                                                        #parameters for 2011 data  - computed on Feb 17th 2012  
    
    ##Average efficiency side C for stationname:2=BML,3=BMS,4=BOL,5=BOS,8=BMF,9=BOF,10=BOG 
#    kwargs.setdefault("PhiAndEtaEff_C"     ,[0.899653,  0.858507,  0.918899,  0.902046,  0.804579,  0.83786,   0.874458, 1., 1.]  )              #parameters for 2011 data  - computed on Feb 17th 2012  
#    kwargs.setdefault("OnlyPhiEff_C"       ,[0.0360032, 0.0377371, 0.0249484, 0.043977,  0.106648,  0.0525583, 0.0737916, 0., 0.] )              #parameters for 2011 data  - computed on Feb 17th 2012  
#    kwargs.setdefault("OnlyEtaEff_C"       ,[0.0291845, 0.0476676, 0.0293888, 0.0234373, 0.0532956, 0.073884,  0.0303749, 0., 0.] )              #parameters for 2011 data  - computed on Feb 17th 2012  
#    ##Average efficiency side C for stationname:2=BML,3=BMS,4=BOL,5=BOS,8=BMF,9=BOF,10=BOG 
#    kwargs.setdefault("PhiAndEtaEff_C"     ,[0.899653,  0.858507,  0.918899,  0.902046,  0.804579,  0.83786,   0.874458]  )              #parameters for 2011 data  - computed on Feb 17th 2012  
#    kwargs.setdefault("OnlyPhiEff_C"       ,[0.0360032, 0.0377371, 0.0249484, 0.043977,  0.106648,  0.0525583, 0.0737916] )              #parameters for 2011 data  - computed on Feb 17th 2012  
#    kwargs.setdefault("OnlyEtaEff_C"       ,[0.0291845, 0.0476676, 0.0293888, 0.0234373, 0.0532956, 0.073884,  0.0303749] )              #parameters for 2011 data  - computed on Feb 17th 2012  
    ########## the above numbers correspond to the following measured eff. 
    ##"GapEff_C"     [0.96484,0.94391,0.973235,0.96946,0.964523,0.964302,0.978625]                                                        #parameters for 2011 data  - computed on Feb 17th 2012  
    ##"PhiEff_C"     [0.935657,0.896244,0.943847,0.946023,0.911227,0.890419,0.94825]                                                      #parameters for 2011 data  - computed on Feb 17th 2012  
    ##"EtaEff_C"     [0.928838,0.906174,0.948287,0.925483,0.857875,0.911744,0.904833]                                                     #parameters for 2011 data  - computed on Feb 17th 2012  
    
    #Average parameter for Cluster Size distribution side A for (stationname:2=BML,3=BMS,4=BOL,5=BOS,8=BMF,9=BOF,10=BOG x(eta, phi) ... extended to describe clustersize constant and equal to 1 for BME and BOE
    #Parameters are: CS1 fraction, CS2 fraction, CS more than 2 fraction, CS more than 2 Mean (meanCStail = FirstClusterSizeInTail+Mean) 
#    kwargs.setdefault("FracClusterSize1_A"     ,[0.627304,0.658215,0.615346,0.658012,0.661349,0.587244,0.749, 1., 1.,  0.540146,0.609927, 0.612526,0.611873, 0.514733,0.620422,0.531583, 1., 1.] )  #parameters for 2011 data  - computed on Feb 17th 2012  
#    kwargs.setdefault("FracClusterSize2_A"     ,[0.232038,0.21971, 0.21273, 0.210508,0.208895,0.231356,0.178083, 0., 0., 0.31945, 0.290177, 0.267311,0.295923, 0.320884,0.275844,0.325542, 0., 0.] )  #parameters for 2011 data  - computed on Feb 17th 2012  
#    kwargs.setdefault("FracClusterSizeTail_A"  ,[0.140658,0.122075,0.171924,0.131481,0.129756,0.1814,  0.0729167, 0.,0., 0.140404,0.0998962,0.120163,0.0922038,0.164384,0.103733,0.142875, 0., 0.] )  #parameters for 2011 data  - computed on Feb 17th 2012  
#    kwargs.setdefault("MeanClusterSizeTail_A"  ,[0.605203,0.565736,0.684106,0.584015,0.598023,0.887267,0.314125, 0., 0., 0.587461,0.426413, 0.456752,0.381531, 0.757233,0.4362,  0.596042, 0., 0.] )  #parameters for 2011 data  - computed on Feb 17th 2012  

    #Average parameter for Cluster Size distribution side C for (stationname:2=BML,3=BMS,4=BOL,5=BOS,8=BMF,9=BOF,10=BOG)x(eta, phi) 
    #Parameters are: CS1 fraction, CS2 fraction, CS more than 2 fraction, CS more than 2 Mean (meanCStail = FirstClusterSizeInTail+Mean) 
#    kwargs.setdefault("FracClusterSize1_C"     ,[0.598065,0.661227,0.599827,0.638833,0.6845,  0.603163,0.754375, 1., 1., 0.524719,0.620468, 0.589353,0.588779, 0.535523,0.582558,0.519708, 1., 1.] )   #parameters for 2011 data  - computed on Feb 17th 2012  
#    kwargs.setdefault("FracClusterSize2_C"     ,[0.243769,0.21774, 0.220941,0.221844,0.210239,0.229419,0.17975, 0., 0., 0.329536,0.281698, 0.280159,0.316243, 0.306261,0.300442,0.336583, 0., 0.] )   #parameters for 2011 data  - computed on Feb 17th 2012  
#    kwargs.setdefault("FracClusterSizeTail_C"  ,[0.158166,0.121033,0.179232,0.139323,0.105261,0.167419,0.065875, 0., 0., 0.145745,0.0978336,0.130488,0.0949772,0.158216,0.117,   0.143708, 0., 0.] )   #parameters for 2011 data  - computed on Feb 17th 2012  
#    kwargs.setdefault("MeanClusterSizeTail_C"  ,[0.694407,0.562872,0.705286,0.578338,0.455239,0.826372,0.28075, 0., 0., 0.612939,0.432854, 0.514194,0.38546,  0.721636,0.569558,0.595542, 0., 0.] )   #parameters for 2011 data  - computed on Feb 17th 2012  


    #setting common reference values for efficiencies 24 Feb 2012 - Chiodini - Spagnolo - Corradi - 
    #these values are intended to reproduce a low pt trigger eff. at plateau (w.r.t. reconstructed CB muons) equal to 77.7% (measured in 2011 data)
    #Average efficiency sideA for stationname:2=BML,3=BMS,4=BOL,5=BOS,8=BMF,9=BOF,10=BOG                                            
    #kwargs.setdefault("PhiAndEtaEff_A"    ,[0.905,  0.905,  0.905,  0.905,  0.905,  0.905, 0.905] ) #ref. par.s low pt plateau w.r.t. reco = 77.7% 
    #kwargs.setdefault("OnlyPhiEff_A"      ,[0.030,  0.030,  0.030,  0.030,  0.030,  0.030, 0.030] ) #ref. par.s low pt plateau w.r.t. reco = 77.7%
    #kwargs.setdefault("OnlyEtaEff_A"      ,[0.030,  0.030,  0.030,  0.030,  0.030,  0.030, 0.030] ) #ref. par.s low pt plateau w.r.t. reco = 77.7%
    #kwargs.setdefault("PhiAndEtaEff_C"    ,[0.905,  0.905,  0.905,  0.905,  0.905,  0.905, 0.905] ) #ref. par.s low pt plateau w.r.t. reco = 77.7% 
    #kwargs.setdefault("OnlyPhiEff_C"      ,[0.030,  0.030,  0.030,  0.030,  0.030,  0.030, 0.030] ) #ref. par.s low pt plateau w.r.t. reco = 77.7%
    #kwargs.setdefault("OnlyEtaEff_C"      ,[0.030,  0.030,  0.030,  0.030,  0.030,  0.030, 0.030] ) #ref. par.s low pt plateau w.r.t. reco = 77.7%

    return CfgMgr.RpcDigitizationTool(name, **kwargs)

def Rpc_OverlayDigitizationTool(name="RpcDigitizationTool", **kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    kwargs.setdefault("EvtStore", overlayFlags.evtStore())
    kwargs.setdefault("OutputObjectName",overlayFlags.evtStore()+"+RPC_DIGITS")
    if not overlayFlags.isDataOverlay():
        kwargs.setdefault("OutputSDOName",overlayFlags.evtStore()+"+RPC_SDO")
    return RpcDigitizationTool(name, **kwargs)

def getRPC_OverlayDigitizer(name="RPC_OverlayDigitizer", **kwargs):
    kwargs.setdefault("DigitizationTool","Rpc_OverlayDigitizationTool")
    return CfgMgr.RPC_Digitizer(name,**kwargs)
