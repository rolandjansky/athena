# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#------------------------------------------------------------------------#
# PhysicsP1_pp_run3_v1.py menu for the long shutdown development
#------------------------------------------------------------------------#

# This defines the input format of the chain and it's properties with the defaults set
# always required are: name, stream and groups
#['name', 'L1chainParts'=[], 'stream', 'groups', 'merging'=[], 'topoStartFrom'=False],
#from TriggerMenuMT.HLTMenuConfig.Menu.ChainDefInMenu import ChainProp

from TriggerMenuMT.HLTMenuConfig.Menu.ChainDefInMenu import ChainProp

import TriggerMenuMT.HLTMenuConfig.Menu.Physics_pp_run3_v1 as physics_menu 

from TriggerMenuMT.HLTMenuConfig.Menu.Physics_pp_run3_v1 import PhysicsStream,SingleMuonGroup,EgammaStreamersGroup,TauStreamersGroup,JetStreamersGroup,METStreamersGroup,MinBiasGroup,ZeroBiasGroup


def addP1Signatures():

    from TriggerJobOpts.TriggerFlags          import TriggerFlags
    from AthenaCommon.Logging import logging
    log = logging.getLogger( __name__ )
    log.info('[setupMenu] going to add the PhysicsP1 menu chains now')
    
    TriggerFlags.TestSlice.signatures = TriggerFlags.TestSlice.signatures() + []

    TriggerFlags.MuonSlice.signatures = TriggerFlags.MuonSlice.signatures() + [
          ChainProp(name='HLT_mu50_RPCPEBSecondaryReadout_L1MU20', stream=['RPCSecondaryReadout'], groups=SingleMuonGroup), # TODO: Move to Detector slice
     ]

    TriggerFlags.EgammaSlice.signatures = TriggerFlags.EgammaSlice.signatures() + [
    ]

    TriggerFlags.METSlice.signatures = TriggerFlags.METSlice.signatures() + [
    ]

    TriggerFlags.JetSlice.signatures = TriggerFlags.JetSlice.signatures() + [
    ]

    TriggerFlags.BjetSlice.signatures = TriggerFlags.BjetSlice.signatures() + [
    ] 

    TriggerFlags.TauSlice.signatures = TriggerFlags.TauSlice.signatures() +[
    ]
    TriggerFlags.BphysicsSlice.signatures = TriggerFlags.BphysicsSlice.signatures() + [
    ]
    TriggerFlags.CombinedSlice.signatures = TriggerFlags.CombinedSlice.signatures() + [ 
   ]
    TriggerFlags.HeavyIonSlice.signatures  = TriggerFlags.HeavyIonSlice.signatures() + []
    TriggerFlags.BeamspotSlice.signatures  = TriggerFlags.BeamspotSlice.signatures() + [
          ChainProp(name='HLT_beamspot_trkFS_trkfast_BeamSpotPEB_L1J15',  l1SeedThresholds=['FSNOSEED'], stream=['BeamSpot'], groups=['PS:Online', 'RATE:BeamSpot',  'BW:BeamSpot']),
          #ChainProp(name='HLT_beamspot_activeTE_trkfast_L1J15',  l1SeedThresholds=['FSNOSEED'], stream=['BeamSpot'], groups=['RATE:BeamSpot',  'BW:BeamSpot']),
          #ChainProp(name='HLT_beamspot_trkFS_trkfast_L1J15',  l1SeedThresholds=['FSNOSEED'], stream=['BeamSpot'], groups=['RATE:BeamSpot',  'BW:BeamSpot']),
    ]   
    TriggerFlags.MinBiasSlice.signatures   = TriggerFlags.MinBiasSlice.signatures() + [
        ChainProp(name='HLT_mb_sptrk_L1RD0_FILLED', l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=['PS:Online',MinBiasGroup]),
        ChainProp(name='HLT_mb_sp_L1RD0_FILLED', l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=['PS:Online',MinBiasGroup]),
        ChainProp(name='HLT_mb_alfaperf_L1RD0_FILLED', l1SeedThresholds=['FSNOSEED'], stream=['DISCARD'], groups=MinBiasGroup+['PS:Online']),
        ChainProp(name='HLT_mb_alfaperf_L1RD0_EMPTY', l1SeedThresholds=['FSNOSEED'], stream=['DISCARD'], groups=MinBiasGroup+['PS:Online']),
    ]
    TriggerFlags.CalibSlice.signatures     = TriggerFlags.CalibSlice.signatures() + [
        ChainProp(name='HLT_noalg_LArPEBCalib_L1RD0_EMPTY', l1SeedThresholds=['FSNOSEED'], stream=['LArPEB'], groups=['RATE:Calibration','BW:Detector']),
        ChainProp(name='HLT_noalg_LArPEBCalib_L1RD0_BGRP11', l1SeedThresholds=['FSNOSEED'], stream=['LArPEB'], groups=['RATE:Calibration','BW:Detector']),
        ChainProp(name='HLT_larnoiseburst_L1XE60', l1SeedThresholds=['FSNOSEED'], stream=['LArNoiseBurst'], groups=['RATE:Calibration','BW:Detector']),
    ]
    TriggerFlags.CosmicSlice.signatures    = TriggerFlags.CosmicSlice.signatures() + [
        ChainProp(name='HLT_noalg_SCTPEB_L1RD0_EMPTY', l1SeedThresholds=['FSNOSEED'], stream=['SCTNoise'], groups=['RATE:SCTCalibration','BW:Detector']), # HLT_sct_noise
        ChainProp(name='HLT_noalg_laser_TilePEB_L1CALREQ2', l1SeedThresholds=['FSNOSEED'], stream=['Tile'], groups=['RATE:TileCalibration','BW:Detector']), # HLT_tilecalib_laser 
        ChainProp(name='HLT_noalg_CIS_TilePEB_L1CALREQ1', l1SeedThresholds=['FSNOSEED'], stream=['Tile'], groups=['RATE:TileCalibration','BW:Detector']), # HLT_tilecalib_CIS 
    ]
    TriggerFlags.StreamingSlice.signatures = TriggerFlags.StreamingSlice.signatures() + [
        ChainProp(name='HLT_noalg_L1RD0_EMPTY',  l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=MinBiasGroup), 
        ChainProp(name='HLT_noalg_L1RD0_FILLED', l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=MinBiasGroup),  
        ChainProp(name='HLT_noalg_idmon_L1RD0_EMPTY', l1SeedThresholds=['FSNOSEED'], stream=['IDMonitoring','express'],groups=['RATE:Monitoring','BW:Detector']),
        ChainProp(name='HLT_noalg_idmon_L1RD0_FILLED', l1SeedThresholds=['FSNOSEED'], stream=['IDMonitoring'],groups=['RATE:Monitoring','BW:Detector']),
        ChainProp(name='HLT_noalg_idmon_L1RD0_UNPAIRED_ISO', l1SeedThresholds=['FSNOSEED'], stream=['IDMonitoring'],groups=['RATE:Monitoring','BW:Detector']),

        # L1 combined streamers
        ChainProp(name='HLT_noalg_bkg_L1Bkg',      l1SeedThresholds=['FSNOSEED'], stream=['Background'], groups=['RATE:SeededStreamers', 'BW:Other']),
        ChainProp(name='HLT_noalg_L1Standby',      l1SeedThresholds=['FSNOSEED'], stream=['Standby'], groups=['RATE:SeededStreamers', 'BW:Other']),
        ChainProp(name='HLT_noalg_L1Calo',      l1SeedThresholds=['FSNOSEED'], stream=['L1Calo'], groups=['RATE:SeededStreamers', 'BW:Other']),
        ChainProp(name='HLT_noalg_L1Calo_EMPTY',      l1SeedThresholds=['FSNOSEED'], stream=['L1Calo'], groups=['RATE:SeededStreamers', 'BW:Other']),
        ChainProp(name='HLT_noalg_l1calo_L1J400',      l1SeedThresholds=['FSNOSEED'], stream=['L1Calo'], groups=['RATE:SeededStreamers', 'BW:Other']),

        # L1 calo streamers
        ChainProp(name='HLT_noalg_L1EM3',        l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=EgammaStreamersGroup),
        ChainProp(name='HLT_noalg_L1EM7',        l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=EgammaStreamersGroup),
        ChainProp(name='HLT_noalg_L1EM12',       l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=EgammaStreamersGroup),
        ChainProp(name='HLT_noalg_L1EM15',       l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=EgammaStreamersGroup),
        ChainProp(name='HLT_noalg_L1EM8VH',      l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=EgammaStreamersGroup),
        ChainProp(name='HLT_noalg_L1EM10VH',     l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=EgammaStreamersGroup),
        ChainProp(name='HLT_noalg_L1EM15VH',     l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=EgammaStreamersGroup),
        ChainProp(name='HLT_noalg_L1EM20VH',     l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=EgammaStreamersGroup),
        ChainProp(name='HLT_noalg_L1EM22VHI',    l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=EgammaStreamersGroup),

        ChainProp(name='HLT_noalg_L1TAU8',       l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=TauStreamersGroup),
        #ChainProp(name='HLT_noalg_L1TAU12',      l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=TauStreamersGroup),
#        ChainProp(name='HLT_noalg_L1TAU20',      l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=TauStreamersGroup),
        ChainProp(name='HLT_noalg_L1TAU30',      l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=TauStreamersGroup),
        ChainProp(name='HLT_noalg_L1TAU40',      l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=TauStreamersGroup),
        ChainProp(name='HLT_noalg_L1TAU60',      l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=TauStreamersGroup),
        ChainProp(name='HLT_noalg_L1TAU12IM',    l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=TauStreamersGroup),        
        ChainProp(name='HLT_noalg_L1TAU20IM',    l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=TauStreamersGroup),        

#        ChainProp(name='HLT_noalg_L1J12',        l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=JetStreamersGroup),
        ChainProp(name='HLT_noalg_L1J15',        l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=JetStreamersGroup),
        ChainProp(name='HLT_noalg_L1J20',        l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=JetStreamersGroup),
        ChainProp(name='HLT_noalg_L1J25',        l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=JetStreamersGroup),
        ChainProp(name='HLT_noalg_L1J30',        l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=JetStreamersGroup),
        ChainProp(name='HLT_noalg_L1J40',        l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=JetStreamersGroup),
        ChainProp(name='HLT_noalg_L1J50',        l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=JetStreamersGroup),
        ChainProp(name='HLT_noalg_L1J75',        l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=JetStreamersGroup),
        ChainProp(name='HLT_noalg_L1J85',        l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=JetStreamersGroup),
        ChainProp(name='HLT_noalg_L1J100',       l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=JetStreamersGroup),
#        ChainProp(name='HLT_noalg_L1J120',       l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=JetStreamersGroup),
        ChainProp(name='HLT_noalg_L1J400',       l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=JetStreamersGroup),
        
        ChainProp(name='HLT_noalg_L1XE10',       l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=METStreamersGroup),
        ChainProp(name='HLT_noalg_L1XE30',       l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=METStreamersGroup),
        ChainProp(name='HLT_noalg_L1XE35',       l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=METStreamersGroup),
        ChainProp(name='HLT_noalg_L1XE40',       l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=METStreamersGroup),
        ChainProp(name='HLT_noalg_L1XE45',       l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=METStreamersGroup),
        ChainProp(name='HLT_noalg_L1XE50',       l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=METStreamersGroup),
        ChainProp(name='HLT_noalg_L1XE55',       l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=METStreamersGroup),
        ChainProp(name='HLT_noalg_L1XE60',       l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=METStreamersGroup),
        ChainProp(name='HLT_noalg_L1XE70',       l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=METStreamersGroup),
        ChainProp(name='HLT_noalg_L1XE80',       l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=METStreamersGroup),
        ChainProp(name='HLT_noalg_L1XE300',      l1SeedThresholds=['FSNOSEED'], stream=[PhysicsStream], groups=METStreamersGroup),

        # Cosmic calo stream
        ChainProp(name='HLT_noalg_cosmiccalo_L1EM3_EMPTY', stream=['CosmicCalo'],groups=['RATE:Cosmic_Calo','BW:MinBias']),
        ChainProp(name='HLT_noalg_cosmiccalo_L1RD1_EMPTY', l1SeedThresholds=['FSNOSEED'], stream=['CosmicCalo','express'],groups=['RATE:Calibration','BW:Detector']),
        ChainProp(name='HLT_noalg_cosmiccalo_L1J30_31ETA49_EMPTY', stream=['CosmicCalo'],groups=['RATE:Cosmic_Calo','BW:Jet']),
        ChainProp(name='HLT_noalg_cosmiccalo_L1J12_EMPTY', stream=['CosmicCalo','express'],groups=['RATE:Cosmic_Calo','BW:Jet']),
        ChainProp(name='HLT_noalg_cosmiccalo_L1EM7_EMPTY', stream=['CosmicCalo','express'],groups=['RATE:Cosmic_Calo','BW:Jet']),
        ChainProp(name='HLT_noalg_cosmiccalo_L1J30_EMPTY', stream=['CosmicCalo','express'],groups=['RATE:Cosmic_Calo','BW:Jet']),
        ChainProp(name='HLT_noalg_cosmiccalo_L1J12_FIRSTEMPTY', stream=['CosmicCalo','express'],groups=['RATE:Cosmic_Calo','BW:Jet']),
        ChainProp(name='HLT_noalg_cosmiccalo_L1J30_FIRSTEMPTY', stream=['CosmicCalo','express'],groups=['RATE:Cosmic_Calo','BW:Jet']),
        ChainProp(name='HLT_noalg_cosmiccalo_L1RD0_FIRSTEMPTY', l1SeedThresholds=['FSNOSEED'], stream=['CosmicCalo'], groups=['RATE:Calibration','BW:Detector']),

#        ChainProp(name='HLT_noalg_cosmiccalo_L1RD1_BGRP10', l1SeedThresholds=['FSNOSEED'], stream=['CosmicCalo','express'],groups=['RATE:Calibration','BW:Detector']),

        # MBTS
        ChainProp(name='HLT_noalg_L1MBTS_1_EMPTY', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=MinBiasGroup), #ATR-21740
        ChainProp(name='HLT_noalg_mb_L1RD2_EMPTY', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=MinBiasGroup), # ATR-21367
        ChainProp(name='HLT_noalg_zb_L1ZB',        l1SeedThresholds=['FSNOSEED'], stream=['ZeroBias'],groups=ZeroBiasGroup),# ATR-21367
        ChainProp(name='HLT_noalg_L1MBTS_2_EMPTY', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=MinBiasGroup), #ATR-21999
        ChainProp(name='HLT_noalg_L1MBTS_1_1_EMPTY', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=MinBiasGroup), #ATR-21999

        ChainProp(name='HLT_noalg_L1MBTSA0', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=MinBiasGroup), #ATR-23216
        ChainProp(name='HLT_noalg_L1MBTSA1', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=MinBiasGroup), #ATR-23216
        ChainProp(name='HLT_noalg_L1MBTSA2', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=MinBiasGroup), #ATR-23216
        ChainProp(name='HLT_noalg_L1MBTSA3', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=MinBiasGroup), #ATR-23216
        ChainProp(name='HLT_noalg_L1MBTSA4', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=MinBiasGroup), #ATR-23216
        ChainProp(name='HLT_noalg_L1MBTSA5', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=MinBiasGroup), #ATR-23216
        ChainProp(name='HLT_noalg_L1MBTSA6', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=MinBiasGroup), #ATR-23216
        ChainProp(name='HLT_noalg_L1MBTSA7', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=MinBiasGroup), #ATR-23216
        ChainProp(name='HLT_noalg_L1MBTSA8', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=MinBiasGroup), #ATR-23216
        ChainProp(name='HLT_noalg_L1MBTSA9', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=MinBiasGroup), #ATR-23216
        ChainProp(name='HLT_noalg_L1MBTSA10', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=MinBiasGroup), #ATR-23216
        ChainProp(name='HLT_noalg_L1MBTSA11', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=MinBiasGroup), #ATR-23216
        ChainProp(name='HLT_noalg_L1MBTSA12', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=MinBiasGroup), #ATR-23216
        ChainProp(name='HLT_noalg_L1MBTSA13', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=MinBiasGroup), #ATR-23216
        ChainProp(name='HLT_noalg_L1MBTSA14', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=MinBiasGroup), #ATR-23216
        ChainProp(name='HLT_noalg_L1MBTSA15', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=MinBiasGroup), #ATR-23216
        
        ChainProp(name='HLT_noalg_L1MBTSC0', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=MinBiasGroup), #ATR-23216
        ChainProp(name='HLT_noalg_L1MBTSC1', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=MinBiasGroup), #ATR-23216
        ChainProp(name='HLT_noalg_L1MBTSC2', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=MinBiasGroup), #ATR-23216
        ChainProp(name='HLT_noalg_L1MBTSC3', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=MinBiasGroup), #ATR-23216
        ChainProp(name='HLT_noalg_L1MBTSC4', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=MinBiasGroup), #ATR-23216
        ChainProp(name='HLT_noalg_L1MBTSC5', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=MinBiasGroup), #ATR-23216
        ChainProp(name='HLT_noalg_L1MBTSC6', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=MinBiasGroup), #ATR-23216
        ChainProp(name='HLT_noalg_L1MBTSC7', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=MinBiasGroup), #ATR-23216
        ChainProp(name='HLT_noalg_L1MBTSC8', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=MinBiasGroup), #ATR-23216
        ChainProp(name='HLT_noalg_L1MBTSC9', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=MinBiasGroup), #ATR-23216
        ChainProp(name='HLT_noalg_L1MBTSC10', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=MinBiasGroup), #ATR-23216
        ChainProp(name='HLT_noalg_L1MBTSC11', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=MinBiasGroup), #ATR-23216
        ChainProp(name='HLT_noalg_L1MBTSC12', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=MinBiasGroup), #ATR-23216
        ChainProp(name='HLT_noalg_L1MBTSC13', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=MinBiasGroup), #ATR-23216
        ChainProp(name='HLT_noalg_L1MBTSC14', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=MinBiasGroup), #ATR-23216
        ChainProp(name='HLT_noalg_L1MBTSC15', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=MinBiasGroup), #ATR-23216


    ]
    TriggerFlags.MonitorSlice.signatures   = TriggerFlags.MonitorSlice.signatures() + [
        ChainProp(name='HLT_timeburner_L1All', l1SeedThresholds=['FSNOSEED'], stream=['DISCARD'], groups=['PS:Online','RATE:DISCARD','BW:DISCARD']),

    ]

    # Random Seeded EB chains which select at the HLT based on L1 TBP bits
    TriggerFlags.EnhancedBiasSlice.signatures = TriggerFlags.EnhancedBiasSlice.signatures() + [ ]


def setupMenu():

    from AthenaCommon.Logging                 import logging
    log = logging.getLogger( 'TriggerMenuMT.HLTMenuConfig.Menu.PhysicsP1_pp_run3_v1.py' )
    log.info('setupMenu ...')

    physics_menu.setupMenu()
    addP1Signatures()

