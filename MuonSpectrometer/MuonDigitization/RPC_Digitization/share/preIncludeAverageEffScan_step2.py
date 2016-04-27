#----------- private options 
#--- cabling & trigger roads
from AthenaCommon.AppMgr import ServiceMgr; 
import MuonRPC_Cabling.MuonRPC_CablingConfig ; 
ServiceMgr.MuonRPC_CablingSvc.RPCTriggerRoadsfromCool=False


from AthenaCommon import CfgGetter
from AthenaCommon.CfgGetter import getPublicTool, getPrivateTool, getService, getAlgorithm

RPC_DigitizationTool=getPublicTool("RpcDigitizationTool")
#RPC_DigitizationTool.ValidationSetup = 1   
RPC_DigitizationTool.IgnoreRunDependentConfig       = False
#RPC_DigitizationTool.Force_BOG_BOF_DoubletR2_OFF    = False
#RPC_DigitizationTool.Efficiency_fromCOOL            = True  # dead strips from COOL          
#RPC_DigitizationTool.ClusterSize_fromCOOL           = True  
#RPC_DigitizationTool.RPCInfoFromDb                  = True
#RPC_DigitizationTool.CutProjectedTracks             = 999999999; #always apply python eff. and cs values; dead strips from COOL
#RPC_DigitizationTool.CutProjectedTracks             = 1;         #always apply COOL   eff. and cs values; dead strips from COOL
#RPC_DigitizationTool.CutProjectedTracks             = 10;         # apply COOL   eff. and cs values, if at least 10 tracks were used for the meas.; dead strips from COOL
#RPC_DigitizationTool.DumpFromDbFirst=True #### if True OutputLevel must be DEBUG
#RPC_DigitizationTool.OutputLevel=DEBUG

scaleFactor = 1./1.0195652173913;
effEtaAndPhiMC15a = 0.938;
effOnlyPhiMC15a   = 0.022;
effOnlyEtaMC15a   = 0.022;
print "scaleFasctor w.r.t. MC15a = ", scaleFactor
effEtaAndPhi = scaleFactor*effEtaAndPhiMC15a;
effOnlyPhi   = scaleFactor*effOnlyPhiMC15a;
effOnlyEta   = scaleFactor*effOnlyEtaMC15a;
print "***** MC15b test eff EtaAndPhi/onlyPhi/onlyEta = ", effEtaAndPhi, " ", effOnlyPhi, " ", effOnlyEta
RPC_DigitizationTool.PhiAndEtaEff_A =[effEtaAndPhi, effEtaAndPhi, effEtaAndPhi, effEtaAndPhi, effEtaAndPhi, effEtaAndPhi, effEtaAndPhi, effEtaAndPhi, effEtaAndPhi]
RPC_DigitizationTool.PhiAndEtaEff_C =[effEtaAndPhi, effEtaAndPhi, effEtaAndPhi, effEtaAndPhi, effEtaAndPhi, effEtaAndPhi, effEtaAndPhi, effEtaAndPhi, effEtaAndPhi]
RPC_DigitizationTool.OnlyPhiEff_A   =[effOnlyPhi,   effOnlyPhi,   effOnlyPhi,   effOnlyPhi,   effOnlyPhi,   effOnlyPhi,   effOnlyPhi,   effOnlyPhi,   effOnlyPhi  ]
RPC_DigitizationTool.OnlyPhiEff_C   =[effOnlyPhi,   effOnlyPhi,   effOnlyPhi,   effOnlyPhi,   effOnlyPhi,   effOnlyPhi,   effOnlyPhi,   effOnlyPhi,   effOnlyPhi  ]
RPC_DigitizationTool.OnlyEtaEff_A   =[effOnlyEta,   effOnlyEta,   effOnlyEta,   effOnlyEta,   effOnlyEta,   effOnlyEta,   effOnlyEta,   effOnlyEta,   effOnlyEta  ]
RPC_DigitizationTool.OnlyEtaEff_C   =[effOnlyEta,   effOnlyEta,   effOnlyEta,   effOnlyEta,   effOnlyEta,   effOnlyEta,   effOnlyEta,   effOnlyEta,   effOnlyEta  ]
RPC_DigitizationTool.OutputLevel=INFO

### cond db tool 
RpcDetectorStatusDbTool=getPublicTool("RpcDetectorStatusDbTool")
#RpcDetectorStatusDbTool.OutputLevel=DEBUG

