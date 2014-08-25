include.block( "MissingETPerformance/MuonTool_jobOptions.py" )

from MissingETPerformance.MissingETPerformanceConf import \
     MuonTool as ConfiguredMuonTool
MuonTool = ConfiguredMuonTool(
    JetCollectionKey            =    'AntiKt6LCTopoJets',
    CaloMuonCollectionKey       =    'CaloMuonCollection'
    )

ToolSvc += MuonTool
print      MuonTool
