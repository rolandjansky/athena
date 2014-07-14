include.block ("MuonTGC_CnvTools/TgcPrepDataReplicationAlg_jopOptions.py")

# Muon::TgcPrepDataReplicationTool
from MuonTGC_CnvTools.MuonTGC_CnvToolsConf import Muon__TgcPrepDataReplicationTool
TgcPrepDataReplicationTool = Muon__TgcPrepDataReplicationTool("Muon::TgcPrepDataReplicationTool")
ToolSvc += TgcPrepDataReplicationTool

# Muon::TgcPrepDataReplicationAlg
from MuonTGC_CnvTools.MuonTGC_CnvToolsConf import Muon__TgcPrepDataReplicationAlg
TgcPrepDataReplicationAlg = Muon__TgcPrepDataReplicationAlg("Muon::TgcPrepDataReplicationAlg")
topSequence += TgcPrepDataReplicationAlg
