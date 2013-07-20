from TriggerMenuAnalysis.TriggerMenuAnalysisConfig import TrigMenuNtupleAlg_default
from TriggerMenuAnalysis.TriggerMenuAnalysisConf import RoILinksCnvTool
import logging
log = logging.getLogger('TriggerMenuAnalysis_Nt.py')

ToolSvc += RoILinksCnvTool('RoILinksCnvTool')
ToolSvc.RoILinksCnvTool.Chains_Muon = [
    'L2_mu4', 'EF_mu4', 
    'L2_mu6', 'EF_mu6', 
    'L2_mu10', 'EF_mu10', 
    'L2_mu20', 'EF_mu20',
    'L2_mu6_MSonly', 'EF_mu6_MSonly', 
    'L2_mu10_MSonly', 'EF_mu10_MSonly', 
    ]
ToolSvc.RoILinksCnvTool.Chains_TileMu = [
    'L2_mu4_tile', 'EF_mu4_tile', 
    'L2_mu4_trod', 'EF_mu4_trod', 
    ]
ToolSvc.RoILinksCnvTool.Chains_Electron = [
    'L2_e5_medium', 'EF_e5_medium', 
    'L2_e10_loose', 'EF_e10_loose', 
    'L2_e10_medium', 'EF_e10_medium', 
    ]
ToolSvc.RoILinksCnvTool.Chains_Tau = [
    'L2_tau16i_loose', 'EF_tau16i_loose', 
    ]
ToolSvc.RoILinksCnvTool.Chains_Jet = [
    'L2_j7', 'EF_j10v3', 
    ]
ToolSvc.RoILinksCnvTool.Chains_Any = []

theTrigMenuNtupleAlg = TrigMenuNtupleAlg_default('TrigMenuNtupleAlg')
theTrigMenuNtupleAlg.ntupleFile = TriggerFlags.NtupleProductionFlags.FileNames.forSlice('Muon')
theTrigMenuNtupleAlg.ntupleComponents = [
    # components for Tier0
    #'TrigMenu',
    'TrigDec',
    'L1RoI',
    'TrigMenuFlat',
    'HltJet',
    'TriggerDecision', 
    ]
if globalflags.DataSource() == 'data':
    theTrigMenuNtupleAlg.ntupleComponents += [
        'Lvl1Config,EventInfo:ByteStreamEventInfo',
        ]
else:
    theTrigMenuNtupleAlg.ntupleComponents += [
        'Lvl1Config,EventInfo:McEventInfo',
        ]
theTrigMenuNtupleAlg.RoILinksCnvTool = ToolSvc.RoILinksCnvTool

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += theTrigMenuNtupleAlg

print          theTrigMenuNtupleAlg

theApp.HistogramPersistency = "ROOT"

