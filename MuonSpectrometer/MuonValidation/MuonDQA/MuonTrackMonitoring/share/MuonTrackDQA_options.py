#======================================================================
# JobOptions for MuonTrackMonitoring
#======================================================================

muTrackMonMan = AthenaMonManager(name="MuonTrackMonManager",
                                 FileKey             = DQMonFlags.monManFileKey(),
                                 Environment         = DQMonFlags.monManEnvironment(),
                                 DataType            = DQMonFlags.monManDataType(),
                                 OutputLevel         = WARNING)

from RecExConfig.RecFlags import rec as recFlags
from MuonRecExample import MuonRecTools
from MuonTrackMonitoring.MuonTrackMonitoringConf import MuonGenericTracksMon
from AthenaCommon.AppMgr import ToolSvc

############TriggerAware Trk Monitoring#######################
# set to true in case you would like to use trigger-aware monitoring
# only do trigger-aware monitoring if monTrigDecTool known by ToolSvc
# NoTrig requirement part is intended to run on all streams
MuonGenericTracksMon_NoTrig = MuonGenericTracksMon(name        = "MuonGenericTracksMon_NoTrig",
	                                               MuonTriggerChainName = "NoTrig/",
	                                               OutputLevel = WARNING,
	                                               )
#ToolSvc += MuonGenericTracksMon_NoTrig
muTrackMonMan.AthenaMonTools += [MuonGenericTracksMon_NoTrig]

# L1_Trig part is intended to only select events passing L1_MU triggers
MuonGenericTracksMon_L1Trig = MuonGenericTracksMon(name        = "MuonGenericTracksMon_L1Trig",
	                                               MuonTriggerChainName = "",
	                                               OutputLevel = WARNING,
	                                               )

if not DQMonFlags.useTrigger():
    from AthenaCommon.Logging import log
    log.error("MuonTrigTrackDQA_options.py: trigger decision tool not found: don't run trigger-aware monitoring")
else:
    MuonGenericTracksMon_L1Trig.TriggerChain = "L1_MU4, L1_MU6, L1_MU10, L1_MU11, L1_MU15, L1_MU20, L1_2MU4, L1_2MU6, L1_2MU10"
    MuonGenericTracksMon_L1Trig.MuonTriggerChainName = ""
    MuonGenericTracksMon_L1Trig.TrigDecisionTool = monTrigDecTool
    #ToolSvc += MuonGenericTracksMon_L1Trig
    muTrackMonMan.AthenaMonTools += [MuonGenericTracksMon_L1Trig]

#for Muon Quality Selection
if not hasattr(ToolSvc,"MuonSelectorTool"):        
    from MuonSelectorTools.MuonSelectorToolsConf import CP__MuonSelectionTool
    ToolSvc += CP__MuonSelectionTool("MuonSelectorTool", OutputLevel = ERROR)
    ToolSvc.MuonSelectorTool.MaxEta = 2.5
    
#for Muon Resonance Selection
from MuonResonanceTools.MuonResonanceToolsConf import MuonResonanceSelectionTool
from MuonResonanceTools.MuonResonanceToolsConf import MuonResonancePairingTool

ToolSvc += MuonResonanceSelectionTool("ZmumuResonanceSelectionTool")
ToolSvc += MuonResonancePairingTool("ZmumuResonancePairingTool")
ToolSvc.ZmumuResonanceSelectionTool.OutputLevel        = ERROR
ToolSvc.ZmumuResonanceSelectionTool.PtCut              = 20000.0
ToolSvc.ZmumuResonanceSelectionTool.EtaCut             = 2.5
ToolSvc.ZmumuResonanceSelectionTool.IsoCaloCut         = 0.2
ToolSvc.ZmumuResonanceSelectionTool.IsoTrkCut          = 0.2
ToolSvc.ZmumuResonanceSelectionTool.z0Cut              = 100 #to overwrite Z0 cut
ToolSvc.ZmumuResonanceSelectionTool.Max_d0             = 100
ToolSvc.ZmumuResonanceSelectionTool.Calibrate          = False
ToolSvc.ZmumuResonanceSelectionTool.EfficiencyCorr     = False
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
ToolSvc.JpsimumuResonanceSelectionTool.IsoCaloCut         = 1
ToolSvc.JpsimumuResonanceSelectionTool.IsoTrkCut          = 1
ToolSvc.JpsimumuResonanceSelectionTool.z0Cut              = 100
ToolSvc.JpsimumuResonanceSelectionTool.Max_d0             = 100
ToolSvc.JpsimumuResonanceSelectionTool.Calibrate          = False #no calibration on data
ToolSvc.JpsimumuResonanceSelectionTool.EfficiencyCorr     = False #no efficiency correction on data
ToolSvc.JpsimumuResonanceSelectionTool.MuonSelectionTool.OutputLevel = ERROR
ToolSvc.JpsimumuResonancePairingTool.OutputLevel          = ERROR
ToolSvc.JpsimumuResonancePairingTool.HighMassWindow       = 3600.0
ToolSvc.JpsimumuResonancePairingTool.LowMassWindow        = 2600.0


MuonGenericTracksMon_NoTrig.JpsimumuResonanceSelectionTool = ToolSvc.JpsimumuResonanceSelectionTool
MuonGenericTracksMon_NoTrig.ZmumuResonanceSelectionTool = ToolSvc.ZmumuResonanceSelectionTool
MuonGenericTracksMon_L1Trig.JpsimumuResonanceSelectionTool = ToolSvc.JpsimumuResonanceSelectionTool
MuonGenericTracksMon_L1Trig.ZmumuResonanceSelectionTool = ToolSvc.ZmumuResonanceSelectionTool

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += muTrackMonMan

