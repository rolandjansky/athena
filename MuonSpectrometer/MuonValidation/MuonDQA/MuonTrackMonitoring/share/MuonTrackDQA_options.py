#======================================================================
# JobOptions for MuonTrackMonitoring
#======================================================================

muTrackMonMan = AthenaMonManager(name="MuonTrackMonManager",
                                 FileKey             = DQMonFlags.monManFileKey(),
                                 Environment         = DQMonFlags.monManEnvironment(),
                                 DataType            = DQMonFlags.monManDataType(),
                                 OutputLevel         = muonOutputLevel)

from RecExConfig.RecFlags import rec as recFlags
from MuonRecExample import MuonRecTools
from MuonTrackMonitoring.MuonTrackMonitoringConf import MuonGenericTracksMon

# No trigger config
MuonGenericTracksMon_NoTrig = MuonGenericTracksMon(name        = "MuonGenericTracksMon_NoTrig",
                                                   EnableLumi  = DQMonFlags.enableLumiAccess(),
                                                   OutputLevel = WARNING,
                                                   )
from AthenaCommon.AppMgr import ToolSvc
ToolSvc += MuonGenericTracksMon_NoTrig
muTrackMonMan.AthenaMonTools.append(MuonGenericTracksMon_NoTrig)

# from MuonSelectorTools.MuonSelectorToolsConf import CP__MuonSelectionTool
# SelectionTool = CP__MuonSelectionTool(OutputLevel=ERROR)

# if not hasattr(ToolSvc, 'monTrigDecTool') and DQMonFlags.useTrigger():
#     from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
#     monTrigDecTool = Trig__TrigDecisionTool(name=DQMonFlags.nameTrigDecTool())
#     ToolSvc += monTrigDecTool
# else
    
#for Muon Resonance Selection
from MuonResonanceTools.MuonResonanceToolsConf import MuonResonanceSelectionTool
from MuonResonanceTools.MuonResonanceToolsConf import MuonResonancePairingTool

ToolSvc += MuonResonanceSelectionTool("ZmumuResonanceSelectionTool")
ToolSvc += MuonResonancePairingTool("ZmumuResonancePairingTool")
ToolSvc.ZmumuResonanceSelectionTool.OutputLevel        = ERROR
ToolSvc.ZmumuResonanceSelectionTool.PtCut              = 20000.0
ToolSvc.ZmumuResonanceSelectionTool.EtaCut             = 2.5
ToolSvc.ZmumuResonanceSelectionTool.z0Cut              = 100 #to overwrite Z0 cut
ToolSvc.ZmumuResonanceSelectionTool.Calibrate          = False
ToolSvc.ZmumuResonanceSelectionTool.EfficiencyCorr     = False
#ToolSvc.ZmumuResonanceSelectionTool.TriggerDecisionTool = monTrigDecTool
ToolSvc.ZmumuResonanceSelectionTool.MuonSelectionTool.OutputLevel = ERROR
ToolSvc.ZmumuResonancePairingTool.OutputLevel          = ERROR
ToolSvc.ZmumuResonancePairingTool.HighMassWindow       = 106000.0
ToolSvc.ZmumuResonancePairingTool.LowMassWindow        =  76000.0

# J/psi configuration
ToolSvc += MuonResonanceSelectionTool("JpsimumuResonanceSelectionTool")
ToolSvc += MuonResonancePairingTool("JpsimumuResonancePairingTool")
ToolSvc.JpsimumuResonanceSelectionTool.OutputLevel        = ERROR
###               loose selections for J/psi                    ###
ToolSvc.JpsimumuResonanceSelectionTool.PtCut              = 4000.0
ToolSvc.JpsimumuResonanceSelectionTool.EtaCut             = 2.5
ToolSvc.JpsimumuResonanceSelectionTool.IsoCaloCut         = 100
ToolSvc.JpsimumuResonanceSelectionTool.IsoTrkCut          = 100
ToolSvc.JpsimumuResonanceSelectionTool.Sigd0Cut           = 100 
ToolSvc.JpsimumuResonanceSelectionTool.z0Cut              = 100
ToolSvc.JpsimumuResonanceSelectionTool.Max_d0             = 100
ToolSvc.JpsimumuResonanceSelectionTool.Calibrate          = False #no calibration on data
ToolSvc.JpsimumuResonanceSelectionTool.EfficiencyCorr     = False #no efficiency correction on data
#ToolSvc.JpsimumuResonanceSelectionTool.TriggerDecisionTool = monTrigDecTool
ToolSvc.JpsimumuResonanceSelectionTool.MuonSelectionTool.OutputLevel = ERROR
ToolSvc.JpsimumuResonancePairingTool.OutputLevel          = ERROR
ToolSvc.JpsimumuResonancePairingTool.HighMassWindow       = 3400.0
ToolSvc.JpsimumuResonancePairingTool.LowMassWindow        = 2800.0


MuonGenericTracksMon_NoTrig.JpsimumuResonanceSelectionTool = ToolSvc.JpsimumuResonanceSelectionTool
MuonGenericTracksMon_NoTrig.ZmumuResonanceSelectionTool = ToolSvc.ZmumuResonanceSelectionTool

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += muTrackMonMan

