
# trigger configuration
from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
cfg = TriggerConfigGetter('ReadPool')


# TDT
from AthenaCommon.AppMgr import ToolSvc
from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
tdt = Trig__TrigDecisionTool("TrigDecisionTool")
ToolSvc += tdt


# sequence of algorithms
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()


# the tutorial
from TrigAnalysisExamples.TrigAnalysisExamplesConf import TriggerAnalysisTutorial
tat = TriggerAnalysisTutorial()
topSequence += tat

# Histogram routing
ServiceMgr += CfgMgr.THistSvc()
ServiceMgr.THistSvc.Output += ["file DATAFILE='TriggerAnalysisTutorial.root' TYP='ROOT' OPT='RECREATE'"]
ServiceMgr.THistSvc.Output += ["Trigger DATAFILE='TriggerAnalysisTutorial.root' TYP='ROOT' OPT='RECREATE'"]



photons = ["L1_2EM13", "L2_2g10_mu6", "EF_2g10"]
singletaus = ["EF_tau12_loose", "EF_tau16_loose", "EF_tau16i_loose", "EF_tau20_loose", "EF_tau20i_loose",
              "EF_tau29_loose", "EF_tau29i_loose", "EF_tau38_loose", "EF_tau50_loose", "EF_tau84_loose"]
twotaus = ["EF_2tau20i_loose", "EF_2tau29i_loose", "EF_2tau29i_medium"]
combinedtaus = ["EF_tau12_e10", "L2_tau16i_2j23", "EF_tau16i_2j23", "EF_tau16i_EFxe40"]
electrons = ["EF_e10_loose", "EF_e15_medium", "EF_e15i_medium", "EF_e20_loose", "EF_e20i_loose", "EF_e25i_loose",
             "EF_e105_loose1", "EF_e22i_tight", "EF_e20_loose_passL2", "EF_e20_loose_passEF", "EF_e5_medium", "EF_e10_medium",
             "EF_e12_medium", "EF_e25_loose", "EF_em15_passHLT", "EF_em15i_passHLT", "EF_em20_passHLT", "EF_em20i_passHLT",
             "EF_em105_passHLT", "EF_e5_medium_e10_loose", "EF_e20_loose_xe20", "EF_e20_loose_xe30", "EF_e10_loose_mu6",
             "EF_e25i_medium1", "EF_e55_loose1", "EF_e5_medium1", "EF_e6_medium", "EF_e6_medium1", "EF_e10_loose_IdScan",
             "EF_e10_loose_SiTrk", "EF_e10_medium_SiTrk_robust", "EF_e10_medium_SiTrk", "EF_e20_loose_SiTrk_robust",
             "EF_e20_loose_SiTrk", "EF_e5_medium_L2SW", "EF_e20_loose_IdScan", "EF_e5_medium_Ringer", "EF_e20_loose_NoTrkCut", "EF_e10_NoCut"]
tat.StatTriggerChains = photons + singletaus + twotaus + combinedtaus + electrons;

# for jet analysis use this one, otherwise comment it out
tat.JetTrigger = "EF_tau16i_2j23"

# for Zee analysis use this one, otherwise comment it out
#tat.ZeeTrigger = "EF_e5_medium"
