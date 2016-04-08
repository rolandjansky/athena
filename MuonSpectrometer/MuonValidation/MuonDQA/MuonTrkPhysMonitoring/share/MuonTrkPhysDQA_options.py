# # $Id: MuonTrkPhysDQA_options.py 685020 2015-07-23 08:20:23Z btong $
# # 
# # Offline Athena muon monitoring configuration.
# #======================================================================
print " ******** MuonTrkPhysMonitoring package is repalced by MuonTrackMon package. ******** "  
print " MuonTrkPhysDQA_options.py: package is out-of-date; fix monitoring job options! "
# from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
# manager = AthenaMonManager(name        = "MuonMonManager",
#                            FileKey     = DQMonFlags.monManFileKey(),
#                            Environment = DQMonFlags.monManEnvironment(),
#                            OutputLevel = muonOutputLevel,
#                            )

# from MuonTrkPhysMonitoring.MuonTrkPhysMonitoringConf import MuonTrkPhysMonitoring 
# monitor = MuonTrkPhysMonitoring(name                 = "MuonTrkPhysMon_NoTrig",
#                                 UseTriggerVector     = False,
#                                 MuonTriggerChainName = "NoMuonTriggerSelection",
#                                 OutputLevel          = WARNING,
#                                 )

# from AthenaCommon.AppMgr import ToolSvc
# ToolSvc += monitor
# manager.AthenaMonTools.append(monitor)

# from MuonResonanceTools.MuonResonanceToolsConf import MuonResonanceSelectionTool
# from MuonResonanceTools.MuonResonanceToolsConf import MuonResonancePairingTool

# # Z configuration
# ToolSvc += MuonResonanceSelectionTool("ZmumuResonanceSelectionTool") 
# ToolSvc += MuonResonancePairingTool("ZmumuResonancePairingTool")
# ToolSvc.ZmumuResonanceSelectionTool.PtCut        = 20000.0
# ToolSvc.ZmumuResonanceSelectionTool.EtaCut       = 2.5
# ToolSvc.ZmumuResonancePairingTool.HighMassWindow = 106000.0
# ToolSvc.ZmumuResonancePairingTool.LowMassWindow  =  76000.0

# # J/psi configuration
# ToolSvc += MuonResonanceSelectionTool("JpsimumuResonanceSelectionTool") 
# ToolSvc += MuonResonancePairingTool("JpsimumuResonancePairingTool")
# ToolSvc.JpsimumuResonanceSelectionTool.PtCut        = 5000.0
# ToolSvc.JpsimumuResonanceSelectionTool.EtaCut       = 2.5
# ToolSvc.JpsimumuResonancePairingTool.HighMassWindow = 3200.0
# ToolSvc.JpsimumuResonancePairingTool.LowMassWindow  = 3000.0

# from AthenaCommon.AlgSequence import AlgSequence
# topSequence = AlgSequence()
# topSequence += manager

