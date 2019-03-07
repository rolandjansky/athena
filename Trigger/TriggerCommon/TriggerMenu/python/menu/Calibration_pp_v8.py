# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

import TriggerMenu.menu.Physics_pp_v8 as physics_menu
from TriggerJobOpts.TriggerFlags          import TriggerFlags
from AthenaCommon.Logging import logging
log = logging.getLogger( 'Calibration_pp_v8.py' )


#---------------------------------------------------------------------
#---------------------------------------------------------------------
def setupMenu():

    physics_menu.setupMenu()
    PhysicsStream="Main"

    ### Remove HLT items that have a remapped L1 threshold and therefore not available in MC
    L1toRemove = []

    for prop in dir(TriggerFlags):
        if prop[-5:]!='Slice': continue
        sliceName=prop
        m_slice=getattr(TriggerFlags,sliceName).signatures()
        for chain in reversed(m_slice):
            if chain[1] in L1toRemove:
                del m_slice[m_slice.index(chain)]

    # stream, BW and RATE tags for Bphysics items that appear in Muon and Bphysics slice.signatures
    #BMultiMuonStream   = "Main"  
    RATE_BphysTag      = 'RATE:Bphysics'
    #RATE_BMultiMuonTag = 'RATE:MultiMuon'  # can become RATE:BMultiMuon' with one letter change 
    BW_BphysTag        = 'BW:Bphys'
    #RATE_DYTag         = 'RATE:Muon'
    #BW_DYTag           = 'BW:Muon'   
    
    
    # ---------------------------------------------------------------------------------------
    # INPUT FORMAT FOR CHAINS:
    # ['chainName',  'L1itemforchain', [L1 items for chainParts], [stream], [groups], EBstep], OPTIONAL: [mergingStrategy, offset,[mergingOrder] ]], topoStartsFrom = False
    # ----------------------------------------------------------------------------------------

    TriggerFlags.TestSlice.signatures = TriggerFlags.TestSlice.signatures() + []
    TriggerFlags.AFPSlice.signatures = TriggerFlags.AFPSlice.signatures() + []

    TriggerFlags.MuonSlice.signatures = TriggerFlags.MuonSlice.signatures() + [
        # muon calibration
        ['mu0_muoncalib',             'L1_MU20MU21', ['L1_MU20'],['Muon_Calibration'],["RATE:Calibration","BW:Muon"],-1],
        ]

    TriggerFlags.JetSlice.signatures = TriggerFlags.JetSlice.signatures() + []
    TriggerFlags.BjetSlice.signatures = TriggerFlags.BjetSlice.signatures() + []
    TriggerFlags.METSlice.signatures = TriggerFlags.METSlice.signatures() + []
    TriggerFlags.TauSlice.signatures = TriggerFlags.TauSlice.signatures() + []
    TriggerFlags.EgammaSlice.signatures = TriggerFlags.EgammaSlice.signatures() + [
        ['g12_loose_larpeb',                      'L1_EM10VH',     [], ['LArCells'], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
    	['g20_loose_larpeb_L1EM15',              'L1_EM15', [], ['LArCells'], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g40_loose_larpeb',                     'L1_EM24VHI', [], ['LArCells'], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g60_loose_larpeb',                     'L1_EM24VHI', [], ['LArCells'], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g80_loose_larpeb',                     'L1_EM24VHI', [], ['LArCells'], ['RATE:SinglePhoton', 'BW:Egamma'],-1],        
        # Supporting trigger
        ['e0_perf_L1EM15',              'L1_EM15',[], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['g0_perf_L1EM15',                'L1_EM15',  [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1], 
    ]
    TriggerFlags.BphysicsSlice.signatures = TriggerFlags.BphysicsSlice.signatures() + [
        # ATR-16163
        ['mu4_bJpsi_TrkPEB',            'L1_MU4',  [], ['BphysPEB'], [RATE_BphysTag,BW_BphysTag], -1],
        ['mu6_bJpsi_TrkPEB',            'L1_MU6',  [], ['BphysPEB'], [RATE_BphysTag,BW_BphysTag], -1],
        ['mu6_bJpsi_lowpt_TrkPEB',      'L1_MU6',  [], ['BphysPEB'], [RATE_BphysTag,BW_BphysTag], -1],
	    # ATR-17600 
	    ['mu6_mu2noL1_msonly_bJpsimumu_noid_PEB','L1_MU6', ['L1_MU6',''], ['BphysPEB'], ['RATE:IDMonitoring',BW_BphysTag, 'BW:ID'], -1, ['serial',-1,['mu6','mu2noL1_msonly']]],
    ]

    TriggerFlags.CombinedSlice.signatures = TriggerFlags.CombinedSlice.signatures() + [] 

    TriggerFlags.MinBiasSlice.signatures = TriggerFlags.MinBiasSlice.signatures() + []
    TriggerFlags.BeamspotSlice.signatures = TriggerFlags.BeamspotSlice.signatures()+ [
        ['beamspot_allTE_trkfast_peb',           'L1_4J20', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
        ['beamspot_activeTE_trkfast_peb',           'L1_4J20', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
        ['beamspot_trkFS_trkfast', 'L1_4J20', [], ["BeamSpot"], ['RATE:BeamSpot', 'BW:BeamSpot'], -1],
    ]
    TriggerFlags.CalibSlice.signatures   = TriggerFlags.CalibSlice.signatures() + [
        ['lumipeb_L1RD0_EMPTY',          'L1_RD0_EMPTY',        [], ['PixelNoise'], ["RATE:Calibration", "RATE:PixelCalibration", "BW:Detector"], -1],
        ['lumipeb_L1RD0_FILLED',            'L1_RD0_FILLED',            [], ['PixelBeam'], ["RATE:Calibration", "RATE:PixelBeamCalibration", "BW:Detector"], -1],
        ['lumipeb_L1RD0_UNPAIRED_ISO',      'L1_RD0_UNPAIRED_ISO',      [], ['PixelBeam'], ["RATE:Calibration", "RATE:PixelBeamCalibration", "BW:Detector"], -1],
        # triggers for VdM scan
        ['lumipeb_vdm_L1MBTS_1_BGRP11',       'L1_MBTS_1_BGRP11',       [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1MBTS_2_BGRP11',       'L1_MBTS_2_BGRP11',       [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1RD0_BGRP11',          'L1_RD0_BGRP11',          [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1MBTS_1_BGRP9',        'L1_MBTS_1_BGRP9',        [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1MBTS_2_BGRP9',        'L1_MBTS_2_BGRP9',        [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1RD0_BGRP9',           'L1_RD0_BGRP9',           [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1MBTS_1_UNPAIRED_ISO', 'L1_MBTS_1_UNPAIRED_ISO', [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1MBTS_2_UNPAIRED_ISO', 'L1_MBTS_2_UNPAIRED_ISO', [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1RD0_UNPAIRED_ISO',    'L1_RD0_UNPAIRED_ISO',    [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1RD2_BGRP12',          'L1_RD2_BGRP12',          [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1RD0_BGRP10',          'L1_RD0_BGRP10',          [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1MBTS_1', 'L1_MBTS_1', [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1MBTS_2', 'L1_MBTS_2', [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1RD0_FILLED',         'L1_RD0_FILLED', [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1MBTS_2_LUCID',                 'L1_MBTS_2,L1_LUCID',    [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1], 
        ['lumipeb_vdm_L1MBTS_2_LUCID_UNPAIRED_ISO',    'L1_MBTS_2_UNPAIRED_ISO,L1_LUCID_UNPAIRED_ISO',    [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1MBTS_1_LUCID_BGRP9',  'L1_MBTS_1_BGRP9,L1_LUCID_BGRP9',     [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1MBTS_2_LUCID_BGRP9',  'L1_MBTS_2_BGRP9,L1_LUCID_BGRP9',     [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1MBTS_2_LUCID_BGRP11', 'L1_MBTS_2_BGRP11,L1_LUCID_BGRP11',   [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_L1RD0_ABORTGAPNOTCALIB',    'L1_RD0_ABORTGAPNOTCALIB',    [], ['PixelBeam'], ["RATE:Calibration", "BW:Detector"], -1],

        ['larpebcalib_L1RD0_BGRP11',          'L1_RD0_BGRP11',         [], ['LArPEB'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larpebcalib_L1RD0_EMPTY',           'L1_RD0_EMPTY',          [], ['LArPEB'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larnoiseburst_L1XE60',            'L1_XE60',       [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larnoiseburst_L1XE70',            'L1_XE70',       [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larnoiseburst_L1J40_XE60',            'L1_J40_XE60',       [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larnoiseburst_L1XE80', 'L1_XE80', [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larnoiseburst_L1J100', 'L1_J100', [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],

        #Special re-run chains
        ['larnoiseburst_rerun',                  '',             [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larcalib_L1EM3_EMPTY',    'L1_EM3_EMPTY',      [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larcalib_L1EM7_EMPTY',    'L1_EM7_EMPTY',      [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larcalib_L1TAU8_EMPTY',   'L1_TAU8_EMPTY',     [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larcalib_L1J12_EMPTY',    'L1_J12_EMPTY',      [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larcalib_L1J3031ETA49_EMPTY',  'L1_J30.31ETA49_EMPTY',     [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],

        ['conej40_larpebj',  'L1_J20',     [], ['LArCells'], ['RATE:Calibration', 'BW:Detector'],-1],
        ['conej165_larpebj', 'L1_J100',     [], ['LArCells'], ['RATE:Calibration', 'BW:Detector'],-1], 
        ['conej75_320eta490_larpebj',  'L1_J30.31ETA49',     [], ['LArCells'], ['RATE:Calibration', 'BW:Detector'],-1],
        ['conej140_320eta490_larpebj', 'L1_J75.31ETA49',     [], ['LArCells'], ['RATE:Calibration', 'BW:Detector'],-1],

        # Monitoritems for saturated towers
        ['satu20em_l1satmon_L1J100_FIRSTEMPTY',  'L1_J100_FIRSTEMPTY',     [], ['L1Calo'], ['RATE:Calibration', 'BW:Detector'],-1],
        ['satu20em_l1satmon_L1EM20VH_FIRSTEMPTY',  'L1_EM20VH_FIRSTEMPTY',     [], ['L1Calo'], ['RATE:Calibration', 'BW:Detector'],-1],

        #AFP calib ATR-16124
        ['calibAFP_L1RD0_FILLED',  'L1_RD0_FILLED', [],  ['AFP'], ['RATE:CalibrationAFP','BW:Detector'], -1],

        ['idpsl1_L1IDprescaled','', [], ['IDprescaledL1'], ["RATE:SeededStreamers","BW:Other"], -1],
    ]
    TriggerFlags.CosmicSlice.signatures  = TriggerFlags.CosmicSlice.signatures() + [
        ['tilecalib_laser',   'L1_CALREQ2', [], ['Tile'], ["RATE:Calibration", "RATE:Cosmic_TileCalibration", "BW:Detector"], -1],
        ['sct_noise',          'L1_RD0_EMPTY', [], ['SCTNoise'], ["RATE:Calibration", "RATE:SCTCalibration", "BW:Detector"], -1],
        
        # CosmicCalo
        ['larps_L1EM3_EMPTY',  'L1_EM3_EMPTY', [], ['CosmicCalo'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        ['larps_L1EM7_EMPTY',  'L1_EM7_EMPTY', [], ['CosmicCalo'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        ['larps_L1EM7_FIRSTEMPTY',  'L1_EM7_FIRSTEMPTY', [], ['LArCellsEmpty'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        ['larps_L1J12_EMPTY',  'L1_J12_EMPTY', [], ['CosmicCalo'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        ['larps_L1J30_EMPTY',  'L1_J30_EMPTY', [], ['CosmicCalo'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        ['larps_L1TAU8_EMPTY', 'L1_TAU8_EMPTY', [], ['CosmicCalo'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
	
        ['larps_L1J3031ETA49_EMPTY', 'L1_J30.31ETA49_EMPTY', [], ['CosmicCalo'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        #JetTauEtmiss 
        ['larps_L1J12_FIRSTEMPTY', 'L1_J12_FIRSTEMPTY', [], ['LArCellsEmpty'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        ['larps_L1J30_FIRSTEMPTY', 'L1_J30_FIRSTEMPTY', [], ['LArCellsEmpty'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],

        ['larhec_L1J12_FIRSTEMPTY', 'L1_J12_FIRSTEMPTY', [], ['LArCellsEmpty'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        ['larhec_L1EM7_FIRSTEMPTY', 'L1_EM7_FIRSTEMPTY', [], ['LArCellsEmpty'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        ['larhec_L1TAU8_FIRSTEMPTY', 'L1_TAU8_FIRSTEMPTY', [], ['LArCellsEmpty'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        ['larhec_L1J30_FIRSTEMPTY', 'L1_J30_FIRSTEMPTY', [], ['LArCellsEmpty'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        #CosmicID
        ['id_cosmicid_L1MU4_EMPTY',         'L1_MU4_EMPTY', [],  ['HLT_IDCosmic',  'express'], ['RATE:CosmicSlice', 'RATE:Cosmic_Tracking', 'BW:Detector'], -1],
        ['id_cosmicid_L1MU11_EMPTY',        'L1_MU11_EMPTY', [], ['HLT_IDCosmic',  'express'], ['RATE:CosmicSlice', 'RATE:Cosmic_Tracking', 'BW:Detector'], -1],
    ]
    TriggerFlags.StreamingSlice.signatures = TriggerFlags.StreamingSlice.signatures() + [
        ['noalg_L1RD0_EMPTY', 'L1_RD0_EMPTY', [],   [PhysicsStream, 'express'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['noalg_L1RD0_FILLED', 'L1_RD0_FILLED', [], [PhysicsStream, 'express'], ["RATE:MinBias", "BW:MinBias"], -1],

        # Muon streamers
        ['noalg_L1MU20',           'L1_MU20',           [], [PhysicsStream, 'express'], ['RATE:SeededStreamers', 'BW:Muon'], -1],
        ['noalg_L1MU21',           'L1_MU21',           [], [PhysicsStream, 'express'], ['RATE:SeededStreamers', 'BW:Muon'], -1],
        ['noalg_L1MU10',           'L1_MU10',           [], [PhysicsStream, 'express'], ['RATE:SeededStreamers', 'BW:Muon'], -1],
        ['noalg_L1MU11',           'L1_MU11',           [], [PhysicsStream, 'express'], ['RATE:SeededStreamers', 'BW:Muon'], -1],
        ['noalg_L12MU6',           'L1_2MU6',           [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Muon'], -1],
        ['noalg_L12MU10',          'L1_2MU10',          [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Muon'], -1],
        ['noalg_L12MU4',           'L1_2MU4',           [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Muon'], -1],

        ['noalg_L1MU4_EMPTY',      'L1_MU4_EMPTY',      [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Muon'], -1],
        ['noalg_L1MU4_FIRSTEMPTY', 'L1_MU4_FIRSTEMPTY', [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Muon'], -1],
        ['noalg_L1MU11_EMPTY',      'L1_MU11_EMPTY',      [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Muon'], -1],
        ['noalg_L1MU4_UNPAIRED_ISO', 'L1_MU4_UNPAIRED_ISO', [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Muon'], -1],
        
        # MET streamers
        ['noalg_L1XE35',             'L1_XE35',             [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:MET'], -1],
        ['noalg_L1XE30',             'L1_XE30',             [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:MET'], -1],
        ['noalg_L1XE300',            'L1_XE300',             [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:MET'], -1],

        # Chains for Week1 menu (ATR-11119)
        ['noalg_L1MU4',   'L1_MU4',  [], [PhysicsStream, 'express'], ["RATE:SeededStreamers", "BW:Muon"], -1 ], #AFP request
        ['noalg_L1MU6',   'L1_MU6',  [], [PhysicsStream,'express'], ["RATE:SeededStreamers", "BW:Muon"], -1 ],

        ['noalg_to_L12MU20_OVERLAY',   'L1_2MU20_OVERLAY', [], ['TauOverlay'], ["RATE:TauOverlay",  "BW:TauOverlay"], -1],
        ['noalg_L1TGC_BURST',   'L1_TGC_BURST', [], ['TgcNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],
        #background streamers
        ['noalg_bkg_L1MU4_UNPAIRED_ISO',  'L1_MU4_UNPAIRED_ISO',  [], ['Background'], ["RATE:SeededStreamers", "BW:Other"], -1], 
        ['noalg_bkg_L1RD0_UNPAIRED_ISO', 'L1_RD0_UNPAIRED_ISO', [], ['Background'], ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_bkg_L1J15.31ETA49_UNPAIRED_ISO', 'L1_J15.31ETA49_UNPAIRED_ISO', [], ['Background'], ["RATE:SeededStreamers", "BW:Other"], -1],
        
        ['noalg_larcells_L1LAR-EM',                'L1_LAR-EM',           [], ['LArCells'],     ["RATE:SeededStreamers", "BW:Other"], -1], 
        ['noalg_larcells_L1LAR-J',                 'L1_LAR-J',            [], ['LArCells'],     ["RATE:SeededStreamers", "BW:Other"], -1], 
        ['noalg_larcells_L1LAR-ZEE_LAR-EM',        'L1_LAR-ZEE_LAR-EM',   [], ['LArCells'],     ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_larcells_L1LAR-ZEE',               'L1_LAR-ZEE',          [], ['LArCells'],     ["RATE:SeededStreamers", "BW:Other"], -1],
      
        #Zero bias streamer
        ['noalg_zb_L1ZB','L1_ZB', [], ['ZeroBias'], ["BW:ZeroBias", "RATE:ZeroBias"], -1],
        #Zero bias plus HLT jet
        ['j40_L1ZB','L1_ZB', [], ['ZeroBias'], ["BW:ZeroBias", "RATE:ZeroBias"], -1],
        
        # Cosmic calo streamer
        ['noalg_cosmiccalo_L1EM3_EMPTY',         'L1_EM3_EMPTY',        [], ['CosmicCalo'], ["BW:MinBias", "RATE:Cosmic_Calo"], -1],        
        ['noalg_cosmiccalo_L1RD1_EMPTY',         'L1_RD1_EMPTY',        [], ['CosmicCalo',  'express'], ["RATE:Calibration", "BW:Detector"], -1],        
        ['noalg_cosmiccalo_L1J3031ETA49_EMPTY',  'L1_J30.31ETA49_EMPTY',[], ['CosmicCalo'], ["BW:Jet", "RATE:Cosmic_Calo"], -1],
        ['noalg_cosmiccalo_L1J12_EMPTY',         'L1_J12_EMPTY',        [], ['CosmicCalo',  'express'], ["BW:Jet", "RATE:Cosmic_Calo"], -1],
        ['noalg_cosmiccalo_L1EM7_EMPTY',         'L1_EM7_EMPTY',        [], ['CosmicCalo',  'express'], ["BW:Jet", "RATE:Cosmic_Calo"], -1],
        ['noalg_cosmiccalo_L1J30_EMPTY',         'L1_J30_EMPTY',        [], ['CosmicCalo',  'express'], ["BW:Jet", "RATE:Cosmic_Calo"], -1],

        #idmon streamer
        ['noalg_idmon_L1RD0_EMPTY',          'L1_RD0_EMPTY',        [], ['IDMonitoring', 'express'], ["RATE:Monitoring", "BW:Detector"], -1],
        ['noalg_idmon_L1RD0_FILLED',         'L1_RD0_FILLED',        [], ['IDMonitoring'], ["RATE:Monitoring", "BW:Detector"], -1],
        ['noalg_idmon_L1RD0_UNPAIRED_ISO',   'L1_RD0_UNPAIRED_ISO', [], ['IDMonitoring'], ["RATE:Monitoring", "BW:Detector"], -1],
	
        #L1Calo requested streamers
        ['noalg_L1J400',  'L1_J400',   [], [PhysicsStream, 'express'], ["Primary:20000","RATE:SeededStreamers", "BW:Jet"], -1 ],
        ['noalg_L1EM15VH',  'L1_EM15VH',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:Egamma"], -1 ],

        ['noalg_L1XE50',  'L1_XE50',   [], [PhysicsStream, 'express'], ["RATE:SeededStreamers", "BW:MET"], -1 ],
        ['noalg_L1XE45',  'L1_XE45',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:MET"], -1 ],
        ['noalg_L1XE40',  'L1_XE40',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:MET"], -1 ],
	
        # Note: These EB noalg chains (bar RD3) are multi-seeded. All L1 seeds must be PS=1 during EB campaign for unweighting to work
        ## Enhanced Bias Physics ##
        ['noalg_eb_L1RD3_FILLED',             'L1_RD3_FILLED',             [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1PhysicsLow_noPS',        'L1_PhysicsLow_noPS',        [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1PhysicsHigh_noPS',       'L1_PhysicsHigh_noPS',       [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ## Enhanced Bias Background ##
        ['noalg_eb_L1RD3_EMPTY',              'L1_RD3_EMPTY',              [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1EMPTY_noPS',             'L1_EMPTY_noPS',             [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1FIRSTEMPTY_noPS',        'L1_FIRSTEMPTY_noPS',        [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1UNPAIRED_ISO_noPS',      'L1_UNPAIRED_ISO_noPS',      [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1UNPAIRED_NONISO_noPS',   'L1_UNPAIRED_NONISO_noPS',   [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1ABORTGAPNOTCALIB_noPS',  'L1_ABORTGAPNOTCALIB_noPS',  [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
]

    TriggerFlags.MonitorSlice.signatures = TriggerFlags.MonitorSlice.signatures() +[
        ['l1calooverflow','', [], ['L1Calo'], ['RATE:Monitoring','BW:Other'], -1],
        ['l1topodebug', '', [], ['L1TopoMismatches'], ['RATE:Monitoring','BW:Other'], -1],
        
        ['mistimemonl1bccorr', '', [], ['Mistimed'], ["RATE:Monitoring","BW:Other"], -1],
        ['mistimemonl1bccorrnomu', '', [], ['Mistimed'], ["RATE:Monitoring","BW:Other"], -1],
        ['mistimemoncaltimenomu', '', [], ['Mistimed'], ["RATE:Monitoring","BW:Other"], -1],
        ['mistimemoncaltime', '', [], ['Mistimed'], ["RATE:Monitoring","BW:Other"], -1],
        ['mistimemonj400', '', [], ['Mistimed'], ["RATE:Monitoring","BW:Other"], -1],
        
        ['costmonitor', '', [], ['CostMonitoring'], ['RATE:Monitoring','BW:Other'],1],
        ['cscmon_L1All', 'L1_All',         [], ['CSC'], ["RATE:Monitoring", "BW:Detector"], -1]
    ]
    TriggerFlags.EnhancedBiasSlice.signatures = TriggerFlags.EnhancedBiasSlice.signatures() + [
        # Enhanced bias HLT items
        ['eb_low_L1RD2_FILLED',               'L1_RD2_FILLED',             [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['eb_high_L1RD2_FILLED',              'L1_RD2_FILLED',             [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['eb_low_L1RD0_FILLED',               'L1_RD0_FILLED',             [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['eb_high_L1RD0_FILLED',              'L1_RD0_FILLED',             [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
    ]


######################################################
Prescales = physics_menu.Prescales
#StreamConfig = physics_menu.StreamConfig

######################################################
# DEFAULT MC prescales
######################################################
Prescales.L1Prescales = dict([(ctpid,1) for ctpid in Prescales.L1Prescales])  # setting all L1 prescales to 1

