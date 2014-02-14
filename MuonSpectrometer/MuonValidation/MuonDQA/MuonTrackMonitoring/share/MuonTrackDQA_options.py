#======================================================================
# TOP JobOptions to run on MuonTrackMonitoring
#======================================================================
## #--------------------------#
## #   MuonTrack  Monitoring  # 
## #--------------------------#
#print '\n****************************************************************************************\n'
#print "   ******** Welcome to the Offline MuonTrackMonitoring package. ******** "  
#print "   ******** Documentation may be found at:******** "  
#print "   **** https://twiki.cern.ch/twiki/bin/view/Atlas/MuonOfflineDQA ***** "  
#print '\n****************************************************************************************\n'

muTrackMonMan = AthenaMonManager(name="MuonTrackMonManager",
                                 FileKey             = DQMonFlags.monManFileKey(),
                                 Environment         = DQMonFlags.monManEnvironment(),
                                 OutputLevel         = muonOutputLevel)
muRpcTrackMonMan = AthenaMonManager(name="MuonRPCTrackMonManager",
                                    FileKey             = DQMonFlags.monManFileKey(),
                                    Environment         = DQMonFlags.monManEnvironment(),
                                    OutputLevel         = muonOutputLevel)
muTgcTrackMonMan = AthenaMonManager(name="MuonTGCTrackMonManager",
                                    FileKey             = DQMonFlags.monManFileKey(),
                                    Environment         = DQMonFlags.monManEnvironment(),
                                    OutputLevel         = muonOutputLevel)
					

from RecExConfig.RecFlags import rec as recFlags
from MuonRecExample import MuonRecTools
from MuonTrackMonitoring.MuonTrackMonitoringConf import MuonSelectedTracksMon
from MuonTrackMonitoring.MuonTrackMonitoringConf import MuonGenericTracksMon
from MuonTrackMonitoring.MuonTrackMonitoringConf import TGCStandaloneTracksMon
from MuonTrackMonitoring.MuonTrackMonitoringConf import RPCStandaloneTracksMon

###########NoTriggerMonitoring##############
   
###########MuonSelectedTracksMon_NoTrig#############
MuonSelectedTracksMon_NoTrig = MuonSelectedTracksMon(name = "MuonSelectedTracksMon_NoTrig",
                                                     WhichTrackCollections = [0,1,1],
                                                     MuonTrackCollections = ["ExtrapolatedMuonSpectrometerTracks","ConvertedMBoyTracks", "MuidExtrapolatedTracks" ],
                                                     NPhiBins = 360,
                                                     UseTriggerVector = False,
                                                     MuonTriggerChainName = "NoMuonTriggerSelection",
                                                     OutputLevel = INFO)
MuonSelectedTracksMon_NoTrig.TrkSummaryTool = MuonRecTools.getPublicTool("MuonTrackSummaryTool")
###########MuonGenericTracksMon_NoTrig############
MuonGenericTracksMon_NoTrig = MuonGenericTracksMon(name = "MuonGenericTracksMon_NoTrig",
                                                   WhichTrackCollections = [0,1,1],
                                                   MuonTrackCollections = ["ExtrapolatedMuonSpectrometerTracks","ConvertedMBoyTracks", "MuidExtrapolatedTracks" ],
                                                   NPhiBins = 360,
                                                   UseTriggerVector = False,
                                                   MuonTriggerChainName = "NoMuonTriggerSelection",
                                                   OutputLevel = INFO)

###########RPCStandaloneTracksMon_NoTrig#############
from StoreGate.StoreGateConf import StoreGateSvc
if not hasattr(topSequence,"RpcClusterBuilderPRD"):
    from RpcClusterization.RpcClusterizationConf import RpcClusterBuilderPRD
    topSequence += RpcClusterBuilderPRD(ClusterTimeSpread = 10000,
                                        OutputLevel = FATAL)
    
RPCStandaloneTracksMon_NoTrig = RPCStandaloneTracksMon(name='RPCStandaloneTracksMon_NoTrig',
                                                       RpcFile               = False,
                                                       DoRpcEsd              = True,
                                                       RpcChamberHist        = False,
                                                       RPCStandaloneTracks   = True,
                                                       RpcReduceNbins        = 8,
                                                       RpcReduceNbinsStrip   = 8,
						       rpc_readout_window    = 0.2, ## micro sec
                                                       doRadiography         = False,
						       doCoolDB              = False,
                                                       ClusterContainer      = "rpcClusters",
                                                       OutputLevel = INFO) 



## --- Muon StandAlone Trk Monitoring and MuonCombined options
if recFlags.doInDet() and recFlags.doMuon():
    ###########TGCStandaloneTracksMon_NoTrig############
    TGCStandaloneTracksMon_NoTrig = TGCStandaloneTracksMon(name = "TGCStandaloneTracksMon_NoTrig",
                                                           TriggerAware = False,
                                                           OutputLevel = INFO)
    ToolSvc += TGCStandaloneTracksMon_NoTrig
    muTgcTrackMonMan.AthenaMonTools += [ TGCStandaloneTracksMon_NoTrig ]
    topSequence += muTgcTrackMonMan
    print muTgcTrackMonMan
    
############################################################
ToolSvc += MuonSelectedTracksMon_NoTrig
ToolSvc += MuonGenericTracksMon_NoTrig
ToolSvc += RPCStandaloneTracksMon_NoTrig

muTrackMonMan.AthenaMonTools += [ MuonSelectedTracksMon_NoTrig ]
muTrackMonMan.AthenaMonTools += [ MuonGenericTracksMon_NoTrig ]
muRpcTrackMonMan.AthenaMonTools += [ RPCStandaloneTracksMon_NoTrig ]

topSequence += muTrackMonMan
topSequence += muRpcTrackMonMan

print muTrackMonMan
print muRpcTrackMonMan
