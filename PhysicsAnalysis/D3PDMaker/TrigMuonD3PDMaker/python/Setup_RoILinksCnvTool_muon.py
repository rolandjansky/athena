# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Setup_RoILinksCnvTool_muon.py
# For the chain to be stored, I refered to TriggerMenuAnalysis package.
# 
# muon chains
muon_chains_1e31 = [
    'L2_mu4',         'EF_mu4', 
    'L2_mu4_SiTrk',   'EF_mu4_SiTrk', 
    'L2_mu4_MG',      'EF_mu4_MG', 
    'L2_mu6',         'EF_mu6', 
    'L2_mu6_MG',      'EF_mu6_MG', 
    'L2_mu10',        'EF_mu10', 
    'L2_mu10_SiTrk',  'EF_mu10_SiTrk', 
    'L2_mu10i_loose', 'EF_mu10i_loose', 
    'L2_mu10_MG',     'EF_mu10_MG', 
    'L2_mu13',        'EF_mu13', 
    'L2_mu15',        'EF_mu15', 
    'L2_mu20',        'EF_mu20', 
    'L2_mu20_MSonly', 'EF_mu20_MSonly', 
    # dimuon
    'L2_2mu4',    'EF_2mu4', 
    'L2_mu4_mu6', 'EF_mu4_mu6', 
    'L2_2mu6',    'EF_2mu6', 
    'L2_2mu10',   'EF_2mu10', 
    ]

tilemu_chains_1e31 = [
    'L2_mu4_tile', 'EF_mu4_tile', 
    'L2_mu4_trod', 'EF_mu4_trod', 
    ]

#passL2MS and L2MSonly_passL2 are combined to passHLT 20100125
#i_loose_MSonly and i_loose_MSonly_SiTrk are removedcombined to passHLT 20100125
muon_chains_initialBeam = [
    # InitialBeam_2010
    'L2_mu4',                 'L2_mu6',                 'L2_mu10',                'L2_mu13',                'L2_mu15',              'L2_mu20',
    'L2_mu4_passHLT',         'L2_mu6_passHLT',         'L2_mu10_passHLT',
    #'L2_mu4_passL2MS',        'L2_mu6_passL2MS',        'L2_mu10_passL2MS',
    'L2_mu4_MSonly',          'L2_mu6_MSonly',          'L2_mu10_MSonly',                                                           'L2_mu20_MSonly',
    #'L2_mu4_L2MSonly_passL2', 'L2_mu6_L2MSonly_passL2', 'L2_mu10_L2MSonly_passL2',
    'L2_mu4_SiTrk',           'L2_mu6_SiTrk',           'L2_mu10_SiTrk',
    'L2_mu4_MG',              'L2_mu6_MG',              'L2_mu10_MG',
    'L2_mu4_NoIDTrkCut'
    #
    'L2_mu10i_loose',
    #'L2_mu10i_loose_MSonly',
    #'L2_mu10i_loose_MSonly_SiTrk',
    #
    'EF_mu4',                 'EF_mu6',                 'EF_mu10',                'EF_mu13',                'EF_mu15',              'EF_mu20',
    'EF_mu4_passHLT',         'EF_mu6_passHLT',         'EF_mu10_passHLT',
    #'EF_mu4_passL2MS',        'EF_mu6_passL2MS',        'EF_mu10_passL2MS',
    'EF_mu4_MSonly',          'EF_mu6_MSonly',          'EF_mu10_MSonly',                                                           'EF_mu20_MSonly',
    #'EF_mu4_L2MSonly_passL2', 'EF_mu6_L2MSonly_passL2', 'EF_mu10_L2MSonly_passL2',
    'EF_mu4_SiTrk',           'EF_mu6_SiTrk',           'EF_mu10_SiTrk',
    'EF_mu4_MG',              'EF_mu6_MG',              'EF_mu10_MG',
    'EF_mu4_NoIDTrkCut'
    #
    'EF_mu10i_loose',
    #'EF_mu10i_loose_MSonly',
    #'EF_mu10i_loose_MSonly_SiTrk',
    # dimuon
    'L2_2mu4',        'EF_2mu4', 
    'L2_2mu4_MSonly', 'EF_2mu4_MSonly', 
    'L2_2mu6',        'EF_2mu6', 
    'L2_2mu10',       'EF_2mu10', 
    'L2_mu4_mu6',     'EF_mu4_mu6', 
    ]

muon_chain_initialBeam_FS = [
    # EF Full Scan
    'EF_mu4_L2MSonly_passL2_EFFS',
    'EF_mu6_L2MSonly_passL2_EFFS',
    'EF_mu10_L2MSonly_passL2_EFFS',
    'EF_mu4_MSonly_passL2_EFFS',
    'EF_mu6_MSonly_passL2_EFFS',
    'EF_mu10_MSonly_passL2_EFFS',
    # EF Full Scan for IB_V3
    'L2_mu4_L2MSonly_EFFS_passL2',
    'L2_mu4_MSonly_EFFS_passL2',
    'L2_mu4_MB2_noL2_EFFS',# to be disabled soon
    'L2_mu4_MSonly_MB1_noL2_EFFS',
    'L2_mu4_MSonly_MB2_noL2_EFFS',
    # EF Full Scan for IB_V3
    'EF_mu4_L2MSonly_EFFS_passL2',
    'EF_mu4_MSonly_EFFS_passL2',
    'EF_mu4_MB2_noL2_EFFS',# to be disabled soon
    'EF_mu4_MSonly_MB1_noL2_EFFS',
    'EF_mu4_MSonly_MB2_noL2_EFFS',
    ]

tilemu_chains_initialBeam = [
    # InitialBeam_v1
    'L2_mu4_tile',     'L2_mu4_tile_SiTrk',
    'L2_mu4_trod',     'L2_mu4_trod_SiTrk',
    #
    'EF_mu4_tile',     'EF_mu4_tile_SiTrk',
    'EF_mu4_trod',     'EF_mu4_trod_SiTrk',
    ]

muon_chains_initialBeam_cosmic = [
    # InitialBeam_2010
    'L2_mu4_cosmic',                 'L2_mu6_cosmic',                 'L2_mu10_cosmic',                'L2_mu13_cosmic',                'L2_mu15_cosmic',              'L2_mu20_cosmic',
    'L2_mu4_passHLT_cosmic',         'L2_mu6_passHLT_cosmic',         'L2_mu10_passHLT_cosmic',
    #'L2_mu4_passL2MS_cosmic',        'L2_mu6_passL2MS_cosmic',        'L2_mu10_passL2MS_cosmic',
    'L2_mu4_MSonly_cosmic',          'L2_mu6_MSonly_cosmic',          'L2_mu10_MSonly_cosmic',                                                                         'L2_mu20_MSonly_cosmic',
    #'L2_mu4_L2MSonly_passL2_cosmic', 'L2_mu6_L2MSonly_passL2_cosmic', 'L2_mu10_L2MSonly_passL2_cosmic',
    'L2_mu4_SiTrk_cosmic',           'L2_mu6_SiTrk_cosmic',           'L2_mu10_SiTrk_cosmic',
    'L2_mu4_MG_cosmic',              'L2_mu6_MG_cosmic',              'L2_mu10_MG_cosmic',
    'L2_mu4_NoIDTrkCut_cosmic'
    #
    'L2_mu10i_loose_cosmic',
    #'L2_mu10i_loose_MSonly_cosmic',
    #'L2_mu10i_loose_MSonly_SiTrk_cosmic',
    #
    'EF_mu4_cosmic',                 'EF_mu6_cosmic',                 'EF_mu10_cosmic',                'EF_mu13_cosmic',                'EF_mu15_cosmic',              'EF_mu20_cosmic',
    'EF_mu4_passHLT_cosmic',         'EF_mu6_passHLTS_cosmic',        'EF_mu10_passHLT_cosmic',
    #'EF_mu4_passL2MS_cosmic',        'EF_mu6_passL2MS_cosmic',        'EF_mu10_passL2MS_cosmic',
    'EF_mu4_MSonly_cosmic',          'EF_mu6_MSonly_cosmic',          'EF_mu10_MSonly_cosmic',                                                                         'EF_mu20_MSonly_cosmic',
    #'EF_mu4_L2MSonly_passL2_cosmic', 'EF_mu6_L2MSonly_passL2_cosmic', 'EF_mu10_L2MSonly_passL2_cosmic',
    'EF_mu4_SiTrk_cosmic',           'EF_mu6_SiTrk_cosmic',           'EF_mu10_SiTrk_cosmic',
    'EF_mu4_MG_cosmic',              'EF_mu6_MG_cosmic',              'EF_mu10_MG_cosmic',
    'EF_mu4_NoIDTrkCut_cosmic'
    #
    'EF_mu10i_loose_cosmic',
    #'EF_mu10i_loose_MSonly_cosmic',
    #'EF_mu10i_loose_MSonly_SiTrk_cosmic',
    # dimuon
    'L2_2mu4_cosmic',        'EF_2mu4_cosmic', 
    'L2_2mu4_MSonly_cosmic', 'EF_2mu4_MSonly_cosmic', 
    'L2_2mu6_cosmic',        'EF_2mu6_cosmic', 
    'L2_2mu10_cosmic',       'EF_2mu10_cosmic', 
    'L2_mu4_mu6_cosmic',     'EF_mu4_mu6_cosmic', 
    ]

tilemu_chains_initialBeam_cosmic = [
    # InitialBeam_v1
    'L2_mu4_tile_cosmic',     'L2_mu4_tile_SiTrk_cosmic',
    'L2_mu4_trod_cosmic',     'L2_mu4_trod_SiTrk_cosmic',
    #
    'EF_mu4_tile_cosmic',     'EF_mu4_tile_SiTrk_cosmic',
    'EF_mu4_trod_cosmic',     'EF_mu4_trod_SiTrk_cosmic',
    ]
muon_chains_physicsPP = [
    'L2_mu10_tight',
    'L2_mu13_tight',
    'L2_mu10_MSonly_tight', 'L2_mu30_MSonly', 'L2_mu40_MSonly',
    'L2_mu13_MG', 'L2_mu13_MG_tight',
    'L2_2mu6_MG',
    'L2_mu4_IDTrkNoCut', 'L2_mu6_IDTrkNoCut', 'L2_mu10_IDTrkNoCut',
    'L2_mu0_NoAlg', 'L2_mu6_NoAlg', 'L2_mu10_NoAlg', 'L2_mu15_NoAlg', 'L2_mu20_NoAlg',
    'L2_2mu0_NoAlg', 'L2_2mu6_NoAlg', 'L2_2mu10_NoAlg',
    'L2_mu20_passHLT',
    'L2_mu20_slow',
    'L2_mu10i_loose',
    'L2_mu4_muCombTag', 'L2_mu6_muCombTag', 'L2_mu10_muCombTag',
    'L2_mu13_muCombTag', 'L2_mu15_muCombTag',

    'EF_mu10_tight',
    'EF_mu13_tight', 
    'EF_mu10_MSonly_tight', 'EF_mu30_MSonly', 'EF_mu40_MSonly',
    'EF_mu13_MG', 'EF_mu13_MG_tight',
    'EF_2mu6_MG',
    'EF_mu4_IDTrkNoCut', 'EF_mu6_IDTrkNoCut', 'EF_mu10_IDTrkNoCut',
    'EF_mu0_NoAlg', 'EF_mu6_NoAlg', 'EF_mu10_NoAlg', 'EF_mu15_NoAlg', 'EF_mu20_NoAlg',
    'EF_2mu0_NoAlg', 'EF_2mu6_NoAlg', 'EF_2mu10_NoAlg',
    'EF_mu20_passHLT',
    'EF_mu20_slow',
    'EF_mu10i_loose',
    'EF_mu4_muCombTag', 'EF_mu6_muCombTag', 'EF_mu10_muCombTag',
    'EF_mu13_muCombTag', 'EF_mu15_muCombTag',
    ]

from AthenaCommon.AppMgr import ToolSvc
if not hasattr (ToolSvc, 'RoILinksCnvTool'):
    from TriggerMenuAnalysis.TriggerMenuAnalysisConf import RoILinksCnvTool
    ToolSvc += RoILinksCnvTool('RoILinksCnvTool')

ToolSvc.RoILinksCnvTool.Chains_Muon   = muon_chains_initialBeam_cosmic   + muon_chains_initialBeam + muon_chain_initialBeam_FS + muon_chains_physicsPP
ToolSvc.RoILinksCnvTool.Chains_TileMu = tilemu_chains_initialBeam_cosmic + tilemu_chains_initialBeam
