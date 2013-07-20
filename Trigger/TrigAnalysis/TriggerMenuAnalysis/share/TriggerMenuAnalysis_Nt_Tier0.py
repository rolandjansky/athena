from TriggerJobOpts.TriggerFlags import TriggerFlags
from TriggerMenuAnalysis.TriggerMenuAnalysisConf import RoILinksCnvTool
from TriggerMenuAnalysis.TriggerMenuAnalysisConfig import TrigMenuNtupleAlg_default

if not hasattr(ToolSvc, 'RoILinksCnvTool'):
    ToolSvc += RoILinksCnvTool('RoILinksCnvTool')

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
if not hasattr(job, 'TrigMenuNtupleAlg'):
    job +=TrigMenuNtupleAlg_default('TrigMenuNtupleAlg')
    job.TrigMenuNtupleAlg.RoILinksCnvTool = ToolSvc.RoILinksCnvTool
    job.TrigMenuNtupleAlg.ntupleFile = TriggerFlags.NtupleProductionFlags.FileNames.forSlice('TrigMenu')
    job.TrigMenuNtupleAlg.ntupleTreeName = 'TrigMenu'
    job.TrigMenuNtupleAlg.ntupleComponents = [
        # components for Tier0
        'TrigDec',
        'L1RoI',
        'TrigMenuFlat',
        # 'HltJet', 
        ]
if globalflags.DataSource() == 'data':
    job.TrigMenuNtupleAlg.ntupleComponents += [
        'Lvl1Config,EventInfo:ByteStreamEventInfo',
        'HltConfig,EventInfo:ByteStreamEventInfo',
        'ES,EventInfo:ByteStreamEventInfo',
        'TriggerDecision', 
        ]
else:
    job.TrigMenuNtupleAlg.ntupleComponents += [
        'Lvl1Config,EventInfo:McEventInfo',
        'HltConfig,EventInfo:McEventInfo',
        'ES,EventInfo:McEventInfo',
        'TriggerDecision', 
        ]

## muon chains
#muon_chains_1e31 = [
#    'L2_mu4',         'EF_mu4', 
#    'L2_mu4_SiTrk',   'EF_mu4_SiTrk', 
#    'L2_mu4_MG',      'EF_mu4_MG', 
#    'L2_mu6',         'EF_mu6', 
#    'L2_mu6_MG',      'EF_mu6_MG', 
#    'L2_mu10',        'EF_mu10', 
#    'L2_mu10_SiTrk',  'EF_mu10_SiTrk', 
#    'L2_mu10i_loose', 'EF_mu10i_loose', 
#    'L2_mu10_MG',     'EF_mu10_MG', 
#    'L2_mu13',        'EF_mu13', 
#    'L2_mu15',        'EF_mu15', 
#    'L2_mu20',        'EF_mu20', 
#    'L2_mu20_MSonly', 'EF_mu20_MSonly', 
#    # dimuon
#    'L2_2mu4',    'EF_2mu4', 
#    'L2_mu4_mu6', 'EF_mu4_mu6', 
#    'L2_2mu6',    'EF_2mu6', 
#    'L2_2mu10',   'EF_2mu10', 
#    ]
#
#tilemu_chains_1e31 = [
#    'L2_mu4_tile', 'EF_mu4_tile', 
#    'L2_mu4_trod', 'EF_mu4_trod', 
#    ]
#
##passL2MS and L2MSonly_passL2 are combined to passHLT 20100125
##i_loose_MSonly and i_loose_MSonly_SiTrk are removed and combined to passHLT 20100125
#muon_chains_initialBeam = [
#    # InitialBeam_2010
#    'L2_mu4',                 'L2_mu6',                 'L2_mu10',                'L2_mu13',                'L2_mu15',              'L2_mu20',
#    'L2_mu4_passHLT',         'L2_mu6_passHLT',         'L2_mu10_passHLT',                                                          'L2_mu20_passHLT',
#    #'L2_mu4_passL2MS',        'L2_mu6_passL2MS',        'L2_mu10_passL2MS',
#    'L2_mu4_MSonly',          'L2_mu6_MSonly',          'L2_mu10_MSonly',                                                           'L2_mu20_MSonly',       'L2_mu30_MSonly',       'L2_mu40_MSonly',
#    #'L2_mu4_L2MSonly_passL2', 'L2_mu6_L2MSonly_passL2', 'L2_mu10_L2MSonly_passL2',
#    'L2_mu4_SiTrk',           'L2_mu6_SiTrk',           'L2_mu10_SiTrk',
#    'L2_mu4_muCombTag',       'L2_mu6_muCombTag',       'L2_mu10_muCombTag',
#    'L2_mu4_muCombTag_SiTrk', 'L2_mu6_muCombTag_SiTrk', 'L2_mu10_muCombTag_SiTrk', 
#    'L2_mu4_MG',              'L2_mu6_MG',              'L2_mu10_MG',
#    'L2_mu4_NoIDTrkCut',                                'L2_mu10_NoIDTrkCut',
#    'L2_mu4_MV',
#    #
#    'L2_mu10i_loose',
#    #'L2_mu10i_loose_MSonly',
#    #'L2_mu10i_loose_MSonly_SiTrk',
#    #
#    'EF_mu4',                 'EF_mu6',                 'EF_mu10',                'EF_mu13',                'EF_mu15',              'EF_mu20',
#    'EF_mu4_passHLT',         'EF_mu6_passHLT',         'EF_mu10_passHLT',                                                          'EF_mu20_passHLT',
#    #'EF_mu4_passL2MS',        'EF_mu6_passL2MS',        'EF_mu10_passL2MS',
#    'EF_mu4_MSonly',          'EF_mu6_MSonly',          'EF_mu10_MSonly',                                                           'EF_mu20_MSonly',       'EF_mu30_MSonly',       'EF_mu40_MSonly',
#    #'EF_mu4_L2MSonly_passL2', 'EF_mu6_L2MSonly_passL2', 'EF_mu10_L2MSonly_passL2',
#    'EF_mu4_SiTrk',           'EF_mu6_SiTrk',           'EF_mu10_SiTrk',
#    'EF_mu4_muCombTag',       'EF_mu6_muCombTag',       'EF_mu10_muCombTag',
#    'EF_mu4_muCombTag_SiTrk', 'EF_mu6_muCombTag_SiTrk', 'EF_mu10_muCombTag_SiTrk', 
#    'EF_mu4_MG',              'EF_mu6_MG',              'EF_mu10_MG',
#    'Ef_mu4_NoIDTrkCut',                                'EF_mu10_NoIDTrkCut',
#    'EF_mu4_MV',
#    #
#    'EF_mu10i_loose',
#    #'EF_mu10i_loose_MSonly',
#    #'EF_mu10i_loose_MSonly_SiTrk',
#    # dimuon
#    'L2_2mu4',        'EF_2mu4', 
#    'L2_2mu4_MSonly', 'EF_2mu4_MSonly', 
#    'L2_2mu6',        'EF_2mu6', 
#    'L2_2mu10',       'EF_2mu10', 
#    'L2_mu4_mu6',     'EF_mu4_mu6', 
#    ]
#
#muon_chain_initialBeam_FS = [
#    # EF Full Scan old ones
#    'EF_mu4_L2MSonly_passL2_EFFS',
#    'EF_mu6_L2MSonly_passL2_EFFS',
#    'EF_mu10_L2MSonly_passL2_EFFS',
#    'EF_mu4_MSonly_passL2_EFFS',
#    'EF_mu6_MSonly_passL2_EFFS',
#    'EF_mu10_MSonly_passL2_EFFS',
#    # EF Full Scan for IB_V3
#    'L2_mu4_L2MSonly_EFFS_passL2',
#    'L2_mu4_MSonly_EFFS_passL2',
#    'L2_mu4_MB2_noL2_EFFS',# disabled at online
#    'L2_mu4_MSonly_MB1_noL2_EFFS',
#    'L2_mu4_MSonly_MB2_noL2_EFFS',
#    'L2_mu6_MSonly_MB2_noL2_EFFS',
#    'L2_mu10_MSonly_MB2_noL2_EFFS',
#    # EF Full Scan for IB_V3
#    'EF_mu4_L2MSonly_EFFS_passL2',#seeded from L1_MU0, running at online with prescaling
#    'EF_mu4_MSonly_EFFS_passL2',#running at online with prescaling
#    'EF_mu4_MB2_noL2_EFFS',# disable at online
#    'EF_mu4_MSonly_MB1_noL2_EFFS',
#    'EF_mu4_MSonly_MB2_noL2_EFFS',#running at online with prescaling
#    'EF_mu6_MSonly_MB2_noL2_EFFS',
#    'EF_mu10_MSonly_MB2_noL2_EFFS',
#    ]
#
#tilemu_chains_initialBeam = [
#    # InitialBeam_v1
#    'L2_mu4_tile',     'L2_mu4_tile_SiTrk',
#    'L2_mu4_trod',     'L2_mu4_trod_SiTrk',
#    #
#    'EF_mu4_tile',     'EF_mu4_tile_SiTrk',
#    'EF_mu4_trod',     'EF_mu4_trod_SiTrk',
#    ]
#
#muon_chains_initialBeam_cosmic = [
#    # InitialBeam_2010
#    'L2_mu4_cosmic',                 'L2_mu6_cosmic',                 'L2_mu10_cosmic',                'L2_mu13_cosmic',                'L2_mu15_cosmic',              'L2_mu20_cosmic',
#    'L2_mu4_passHLT_cosmic',         'L2_mu6_passHLT_cosmic',         'L2_mu10_passHLT_cosmic',
#    #'L2_mu4_passL2MS_cosmic',        'L2_mu6_passL2MS_cosmic',        'L2_mu10_passL2MS_cosmic',
#    'L2_mu4_MSonly_cosmic',          'L2_mu6_MSonly_cosmic',          'L2_mu10_MSonly_cosmic',                                                                         'L2_mu20_MSonly_cosmic',
#    #'L2_mu4_L2MSonly_passL2_cosmic', 'L2_mu6_L2MSonly_passL2_cosmic', 'L2_mu10_L2MSonly_passL2_cosmic',
#    'L2_mu4_SiTrk_cosmic',           'L2_mu6_SiTrk_cosmic',           'L2_mu10_SiTrk_cosmic',
#    'L2_mu4_MG_cosmic',              'L2_mu6_MG_cosmic',              'L2_mu10_MG_cosmic',
#    'L2_mu4_NoIDTrkCut_cosmic',
#    #
#    'L2_mu10i_loose_cosmic',
#    #'L2_mu10i_loose_MSonly_cosmic',
#    #'L2_mu10i_loose_MSonly_SiTrk_cosmic',
#    #
#    'EF_mu4_cosmic',                 'EF_mu6_cosmic',                 'EF_mu10_cosmic',                'EF_mu13_cosmic',                'EF_mu15_cosmic',              'EF_mu20_cosmic',
#    'EF_mu4_passHLT_cosmic',         'EF_mu6_passHLTS_cosmic',        'EF_mu10_passHLT_cosmic',
#    #'EF_mu4_passL2MS_cosmic',        'EF_mu6_passL2MS_cosmic',        'EF_mu10_passL2MS_cosmic',
#    'EF_mu4_MSonly_cosmic',          'EF_mu6_MSonly_cosmic',          'EF_mu10_MSonly_cosmic',                                                                         'EF_mu20_MSonly_cosmic',
#    #'EF_mu4_L2MSonly_passL2_cosmic', 'EF_mu6_L2MSonly_passL2_cosmic', 'EF_mu10_L2MSonly_passL2_cosmic',
#    'EF_mu4_SiTrk_cosmic',           'EF_mu6_SiTrk_cosmic',           'EF_mu10_SiTrk_cosmic',
#    'EF_mu4_MG_cosmic',              'EF_mu6_MG_cosmic',              'EF_mu10_MG_cosmic',
#    'EF_mu4_NoIDTrkCut_cosmic',
#    #
#    'EF_mu10i_loose_cosmic',
#    #'EF_mu10i_loose_MSonly_cosmic',
#    #'EF_mu10i_loose_MSonly_SiTrk_cosmic',
#    # dimuon
#    'L2_2mu4_cosmic',        'EF_2mu4_cosmic', 
#    'L2_2mu4_MSonly_cosmic', 'EF_2mu4_MSonly_cosmic', 
#    'L2_2mu6_cosmic',        'EF_2mu6_cosmic', 
#    'L2_2mu10_cosmic',       'EF_2mu10_cosmic', 
#    'L2_mu4_mu6_cosmic',     'EF_mu4_mu6_cosmic', 
#    ]
#
#tilemu_chains_initialBeam_cosmic = [
#    # InitialBeam_v1
#    'L2_mu4_tile_cosmic',     'L2_mu4_tile_SiTrk_cosmic',
#    'L2_mu4_trod_cosmic',     'L2_mu4_trod_SiTrk_cosmic',
#    #
#    'EF_mu4_tile_cosmic',     'EF_mu4_tile_SiTrk_cosmic',
#    'EF_mu4_trod_cosmic',     'EF_mu4_trod_SiTrk_cosmic',
#    ]


# egamma chains
egamma_chains_1e31 = [
    'L2_e5_medium', 'EF_e5_medium', 
    'L2_e10_loose', 'EF_e10_loose', 
    'L2_e10_medium', 'EF_e10_medium', 
    ]
egamma_chains_cosmic = [
    'L2_e5_NoCut_cosmic', 'EF_e5_NoCut_cosmic', 
    'L2_e5_NoCut_SiTrk_cosmic', 'EF_e5_NoCut_SiTrk_cosmic', 
    'L2_e10_loose_cosmic', 'EF_e10_loose_cosmic', 
    ]
egamma_chains_beam = [
        "L2_e10_loose", "EF_e10_loose",
            "L2_e10_loose_passL2", "EF_e10_loose_passL2",
            "L2_e10_loose_passEF", "EF_e10_loose_passEF",
            "L2_e10_medium", "EF_e10_medium",
            "L2_e5_NoCut_Ringer", "EF_e5_NoCut_Ringer",
            "L2_e5_NoCut", "EF_e5_NoCut",
            "L2_e5_NoCut_IdScan", "EF_e5_NoCut_IdScan",
            "L2_e5_NoCut_TRT", "EF_e5_NoCut_TRT",
            "L2_e5_NoCut_SiTrk", "EF_e5_NoCut_SiTrk",
            "L2_e5_NoCut_FwdBackTrk", "EF_e5_NoCut_FwdBackTrk",
            "L2_e5_NoCut_cells", "EF_e5_NoCut_cells",
            "L2_e5_NoCut_L2SW", "EF_e0_mbts_a_EFfullcalo",
            "L2_e5_EFfullcalo", "EF_e5_EFfullcalo",
        ]
jet_chains = [
    "L2_j7", "EF_j10v3", 
    "L2_j15", "EF_j20v2", 
    "L2_j30", "EF_j40", 
    "L2_j60", "EF_j80v2", 
    "L2_j90", "EF_j140", 
    "L2_j130", "EF_j200", 
]


if TriggerFlags.NtupleProductionFlags.SliceTuples.doSlice('MinBias'):
    pass

if TriggerFlags.NtupleProductionFlags.SliceTuples.doSlice('Muon'):
#    ToolSvc.RoILinksCnvTool.Chains_Muon   = muon_chains_initialBeam_cosmic   + muon_chains_initialBeam + muon_chain_initialBeam_FS
#    ToolSvc.RoILinksCnvTool.Chains_TileMu = tilemu_chains_initialBeam_cosmic + tilemu_chains_initialBeam
    ToolSvc.RoILinksCnvTool.Chains_Muon   = ['L2_mu.*', 'L2_2mu.*', 'L2_3mu.*', 'EF_mu.*', 'EF_2mu.*', 'EF_3mu.*']
    ToolSvc.RoILinksCnvTool.Chains_TileMu = ['L2_mu.*', 'L2_2mu.*', 'L2_3mu.*', 'EF_mu.*', 'EF_2mu.*', 'EF_3mu.*']
    pass

if TriggerFlags.NtupleProductionFlags.SliceTuples.doSlice('EGamma'):
    ToolSvc.RoILinksCnvTool.Chains_Electron = egamma_chains_cosmic + egamma_chains_beam
    pass
ToolSvc.RoILinksCnvTool.Chains_Jet = jet_chains

if TriggerFlags.NtupleProductionFlags.SliceTuples.doSlice('Tau'):
    pass

if TriggerFlags.NtupleProductionFlags.SliceTuples.doSlice('L1Calo'):
    pass
