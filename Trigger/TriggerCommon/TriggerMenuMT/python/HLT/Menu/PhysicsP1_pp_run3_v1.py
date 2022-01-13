# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#------------------------------------------------------------------------#
# PhysicsP1_pp_run3_v1.py menu for the long shutdown development
#------------------------------------------------------------------------#

# This defines the input format of the chain and it's properties with the defaults set
# always required are: name, stream and groups
#['name', 'L1chainParts'=[], 'stream', 'groups', 'merging'=[], 'topoStartFrom'=False],
#from TriggerMenuMT.HLT.Menu.ChainDefInMenu import ChainProp

from TriggerMenuMT.HLT.Menu.ChainDefInMenu import ChainProp
from .SignatureDicts import ChainStore

import TriggerMenuMT.HLT.Menu.Physics_pp_run3_v1 as physics_menu 

from TriggerMenuMT.HLT.Menu.Physics_pp_run3_v1 import (
    SingleMuonGroup,
    SinglePhotonGroup,
    SingleJetGroup,
    JetStreamersGroup,
    MinBiasGroup,
    ZeroBiasGroup,
    SupportGroup,
    SupportLegGroup,
    SupportPhIGroup
)


def addP1Signatures(chains):

    from AthenaCommon.Logging import logging
    log = logging.getLogger( __name__ )
    log.info('[setupMenu] going to add the PhysicsP1 menu chains now')

    chainsP1 = ChainStore()
    
    chainsP1['Muon'] = [
        ChainProp(name='HLT_mu50_RPCPEBSecondaryReadout_L1MU14FCH', stream=['RPCSecondaryReadout'], groups=['PS:Online']+SingleMuonGroup), # TODO: Move to Detector slice
     ]

    chainsP1['Egamma'] = [
        # ATR-21355 - cannot be moved to the calibSlice because they need to configure the photon/ sequence
        ChainProp(name='HLT_g3_loose_LArPEBHLT_L1EM3', stream=['LArCells'], groups=['PS:Online']+SinglePhotonGroup+SupportLegGroup),
        ChainProp(name='HLT_g12_loose_LArPEBHLT_L1EM10VH', stream=['LArCells'], groups=['PS:Online']+SinglePhotonGroup+SupportLegGroup),
        ChainProp(name='HLT_g20_loose_LArPEBHLT_L1EM15', stream=['LArCells'], groups=['PS:Online']+SinglePhotonGroup+SupportLegGroup),
        ChainProp(name='HLT_g40_loose_LArPEBHLT_L1EM22VHI', stream=['LArCells'], groups=['PS:Online']+SinglePhotonGroup+SupportLegGroup),
        ChainProp(name='HLT_g60_loose_LArPEBHLT_L1EM22VHI', stream=['LArCells'], groups=['PS:Online']+SinglePhotonGroup+SupportLegGroup),
        ChainProp(name='HLT_g80_loose_LArPEBHLT_L1EM22VHI', stream=['LArCells'], groups=['PS:Online']+SinglePhotonGroup+SupportLegGroup),
    ]

    chainsP1['Jet'] = [

        # ATR-21355 - cannot be moved to the calibSlice because they need to configure the photon/ sequence
        ChainProp(name='HLT_j25_LArPEBHLT_L1J15', l1SeedThresholds=['FSNOSEED'], stream=['LArCells'], groups=['PS:Online']+SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j25_320eta490_LArPEBHLT_L1J15p31ETA49', l1SeedThresholds=['FSNOSEED'], stream=['LArCells'], groups=['PS:Online']+SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j40_LArPEBHLT_L1J20', l1SeedThresholds=['FSNOSEED'], stream=['LArCells'], groups=['PS:Online']+SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j75_320eta490_LArPEBHLT_L1J30p31ETA49', l1SeedThresholds=['FSNOSEED'], stream=['LArCells'], groups=['PS:Online']+SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j140_320eta490_LArPEBHLT_L1J75p31ETA49', l1SeedThresholds=['FSNOSEED'], stream=['LArCells'], groups=['PS:Online']+SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j165_LArPEBHLT_L1J100', l1SeedThresholds=['FSNOSEED'], stream=['LArCells'], groups=['PS:Online']+SingleJetGroup+SupportLegGroup),
    ]

    chainsP1['Beamspot'] = [
          ChainProp(name='HLT_beamspot_trkFS_trkfast_BeamSpotPEB_L1J15',  l1SeedThresholds=['FSNOSEED'], stream=['BeamSpot'], groups=['PS:Online', 'RATE:BeamSpot',  'BW:BeamSpot']+SupportLegGroup),
          #ChainProp(name='HLT_beamspot_activeTE_trkfast_L1J15',  l1SeedThresholds=['FSNOSEED'], stream=['BeamSpot'], groups=['RATE:BeamSpot',  'BW:BeamSpot']),
          #ChainProp(name='HLT_beamspot_trkFS_trkfast_L1J15',  l1SeedThresholds=['FSNOSEED'], stream=['BeamSpot'], groups=['RATE:BeamSpot',  'BW:BeamSpot']),
    ]

    chainsP1['Calib'] = [
        ChainProp(name='HLT_noalg_LArPEBCalib_L1RD0_EMPTY', l1SeedThresholds=['FSNOSEED'], stream=['LArPEB'], groups=['PS:Online','RATE:Calibration','BW:Detector']),
        ChainProp(name='HLT_noalg_LArPEBCalib_L1RD0_BGRP11', l1SeedThresholds=['FSNOSEED'], stream=['LArPEB'], groups=['PS:Online','RATE:Calibration','BW:Detector']),

        ChainProp(name='HLT_noalg_LArPEBCalib_L1J400_LAR', l1SeedThresholds=['FSNOSEED'], stream=['LArPEBDigitalTrigger'], groups=['PS:Online','RATE:Calibration','BW:Detector']+SupportLegGroup),
        ChainProp(name='HLT_noalg_LArPEBCalib_L1jJ500_LAR', l1SeedThresholds=['FSNOSEED'], stream=['LArPEBDigitalTrigger'], groups=['PS:Online','RATE:Calibration','BW:Detector']+SupportPhIGroup),
        ChainProp(name='HLT_noalg_LArPEBCalib_L1LAR-ZEE', l1SeedThresholds=['FSNOSEED'], stream=['LArCells'], groups=['PS:Online','RATE:Calibration','BW:Detector']+SupportLegGroup),
        ChainProp(name='HLT_noalg_LArPEBCalib_L1LAR-ZEE-eEM', l1SeedThresholds=['FSNOSEED'], stream=['LArCells'], groups=['PS:Online','RATE:Calibration','BW:Detector']+SupportPhIGroup),

        ChainProp(name='HLT_noalg_LATOMEPEB_L1RD0_FILLED', l1SeedThresholds=['FSNOSEED'], stream=['LArPEBDigitalTrigger'], groups=['PS:Online','RATE:Calibration','BW:Detector']),
        ChainProp(name='HLT_noalg_LATOMEPEB_L1RD0_EMPTY', l1SeedThresholds=['FSNOSEED'], stream=['LArPEBDigitalTrigger'], groups=['PS:Online','RATE:Calibration','BW:Detector']),
        ChainProp(name='HLT_noalg_LATOMEPEB_L1RD0_FIRSTEMPTY', l1SeedThresholds=['FSNOSEED'], stream=['LArPEBDigitalTrigger'], groups=['PS:Online','RATE:Calibration','BW:Detector']),
        ChainProp(name='HLT_noalg_LATOMEPEB_L1RD0_BGRP7', l1SeedThresholds=['FSNOSEED'], stream=['LArPEBDigitalTrigger'], groups=['PS:Online','RATE:Calibration','BW:Detector']),

        ChainProp(name='HLT_noalg_LArPEBCalib_L1EM3', l1SeedThresholds=['FSNOSEED'], stream=['LArCells'], groups=['PS:Online','RATE:Calibration','BW:Detector']+SupportLegGroup),
        ChainProp(name='HLT_noalg_LArPEBCalib_L1EM10VH', l1SeedThresholds=['FSNOSEED'], stream=['LArCells'], groups=['PS:Online','RATE:Calibration','BW:Detector']+SupportLegGroup),
        ChainProp(name='HLT_noalg_LArPEBCalib_L1EM15', l1SeedThresholds=['FSNOSEED'], stream=['LArCells'], groups=['PS:Online','RATE:Calibration','BW:Detector']+SupportLegGroup),
        ChainProp(name='HLT_noalg_LArPEBCalib_L1EM22VHI', l1SeedThresholds=['FSNOSEED'], stream=['LArCells'], groups=['PS:Online','RATE:Calibration','BW:Detector']+SupportLegGroup),
        ChainProp(name='HLT_noalg_LArPEBCalib_L1J15', l1SeedThresholds=['FSNOSEED'], stream=['LArCells'], groups=['PS:Online','RATE:Calibration','BW:Detector']+SupportLegGroup),
        ChainProp(name='HLT_noalg_LArPEBCalib_L1J15p31ETA49', l1SeedThresholds=['FSNOSEED'], stream=['LArCells'], groups=['PS:Online','RATE:Calibration','BW:Detector']+SupportLegGroup),
        ChainProp(name='HLT_noalg_LArPEBCalib_L1J20', l1SeedThresholds=['FSNOSEED'], stream=['LArCells'], groups=['PS:Online','RATE:Calibration','BW:Detector']+SupportLegGroup),
        ChainProp(name='HLT_noalg_LArPEBCalib_L1J30p31ETA49', l1SeedThresholds=['FSNOSEED'], stream=['LArCells'], groups=['PS:Online','RATE:Calibration','BW:Detector']+SupportLegGroup),
        ChainProp(name='HLT_noalg_LArPEBCalib_L1J75p31ETA49', l1SeedThresholds=['FSNOSEED'], stream=['LArCells'], groups=['PS:Online','RATE:Calibration','BW:Detector']+SupportLegGroup),
        ChainProp(name='HLT_noalg_LArPEBCalib_L1J100', l1SeedThresholds=['FSNOSEED'], stream=['LArCells'], groups=['PS:Online','RATE:Calibration','BW:Detector']+SupportLegGroup),

        # LAr noise burst chains
        ChainProp(name='HLT_larnoiseburst_L1XE60', l1SeedThresholds=['FSNOSEED'], stream=['LArNoiseBurst'], groups=['PS:Online','RATE:Calibration','BW:Detector']+SupportLegGroup),
        ChainProp(name='HLT_larnoiseburst_L1J75', l1SeedThresholds=['FSNOSEED'], stream=['LArNoiseBurst'], groups=['PS:Online','RATE:Calibration','BW:Detector']+SupportLegGroup),
        ChainProp(name='HLT_larnoiseburst_L1J100', l1SeedThresholds=['FSNOSEED'], stream=['LArNoiseBurst'], groups=['PS:Online','RATE:Calibration','BW:Detector']+SupportLegGroup),
        ChainProp(name='HLT_larnoiseburst_L1J40_XE50', l1SeedThresholds=['FSNOSEED'], stream=['LArNoiseBurst'], groups=['PS:Online','RATE:Calibration','BW:Detector']+SupportLegGroup),
        ChainProp(name='HLT_larnoiseburst_L1J40_XE60', l1SeedThresholds=['FSNOSEED'], stream=['LArNoiseBurst'], groups=['PS:Online','RATE:Calibration','BW:Detector']+SupportLegGroup),
        ChainProp(name='HLT_larnoiseburst_L1All', l1SeedThresholds=['FSNOSEED'], stream=['LArNoiseBurst'], groups=['PS:Online','RATE:Calibration','BW:Detector']),
        
        ## larpsall/em*FIRSTEMPTY
        ChainProp(name='HLT_larpsallem_L1EM7_FIRSTEMPTY', l1SeedThresholds=['FSNOSEED'], stream=['LArNoiseBurst'], groups=['PS:Online','RATE:Calibration','BW:Detector']+SupportLegGroup),
        ChainProp(name='HLT_larpsall_L1J12_FIRSTEMPTY', l1SeedThresholds=['FSNOSEED'], stream=['LArNoiseBurst'], groups=['PS:Online','RATE:Calibration','BW:Detector']+SupportLegGroup),
        ChainProp(name='HLT_larpsall_L1J30_FIRSTEMPTY', l1SeedThresholds=['FSNOSEED'], stream=['LArNoiseBurst'], groups=['PS:Online','RATE:Calibration','BW:Detector']+SupportLegGroup),
        
        ## larpsall/em*EMPTY
        ChainProp(name='HLT_larpsallem_L1EM3_EMPTY', l1SeedThresholds=['FSNOSEED'], stream=['CosmicCalo','express'],groups=['PS:Online','RATE:Cosmic_Calo','RATE:Calibration','BW:Jet']+SupportLegGroup),
        ChainProp(name='HLT_larpsallem_L1EM7_EMPTY', l1SeedThresholds=['FSNOSEED'], stream=['CosmicCalo','express'],groups=['PS:Online','RATE:Cosmic_Calo','RATE:Calibration','BW:Jet']+SupportLegGroup),
        ChainProp(name='HLT_larpsall_L1J12_EMPTY', l1SeedThresholds=['FSNOSEED'], stream=['CosmicCalo','express'],groups=['PS:Online','RATE:Cosmic_Calo','RATE:Calibration','BW:Jet']+SupportLegGroup),
        ChainProp(name='HLT_larpsall_L1J30_EMPTY', l1SeedThresholds=['FSNOSEED'], stream=['CosmicCalo','express'],groups=['PS:Online','RATE:Cosmic_Calo','RATE:Calibration','BW:Jet']+SupportLegGroup),
        ChainProp(name='HLT_larpsall_L1TAU8_EMPTY', l1SeedThresholds=['FSNOSEED'], stream=['CosmicCalo','express'],groups=['PS:Online','RATE:Cosmic_Calo','RATE:Calibration','BW:Jet']+SupportLegGroup),
        ChainProp(name='HLT_larpsall_L1J30p31ETA49_EMPTY', l1SeedThresholds=['FSNOSEED'], stream=['CosmicCalo','express'],groups=['PS:Online','RATE:Cosmic_Calo','RATE:Calibration','BW:Jet']+SupportLegGroup),
    ]

    chainsP1['Cosmic'] = [
        ChainProp(name='HLT_noalg_SCTPEB_L1RD0_EMPTY', l1SeedThresholds=['FSNOSEED'], stream=['SCTNoise'], groups=['PS:Online','RATE:SCTCalibration','BW:Detector']), # HLT_sct_noise
        ChainProp(name='HLT_noalg_laser_TilePEB_L1CALREQ2', l1SeedThresholds=['FSNOSEED'], stream=['Tile'], groups=['PS:Online','RATE:TileCalibration','BW:Detector']), # HLT_tilecalib_laser 
        ChainProp(name='HLT_noalg_CIS_TilePEB_L1CALREQ1', l1SeedThresholds=['FSNOSEED'], stream=['Tile'], groups=['PS:Online','RATE:TileCalibration','BW:Detector']), # HLT_tilecalib_CIS 
    ]

    chainsP1['Streaming'] = [

        ChainProp(name='HLT_noalg_L1RD0_UNPAIRED_ISO', l1SeedThresholds=['FSNOSEED'], stream=['Background'], groups=['PS:Online','BW:Other']),  
        
        ChainProp(name='HLT_noalg_mb_L1RD0_EMPTY',  l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup),
        ChainProp(name='HLT_noalg_mb_L1RD0_FILLED', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup),  

        # ID monitoring
        ChainProp(name='HLT_noalg_idmon_L1RD0_EMPTY', l1SeedThresholds=['FSNOSEED'], stream=['IDMonitoring','express'],groups=['PS:Online','RATE:Monitoring','BW:Detector']),
        ChainProp(name='HLT_noalg_idmon_L1RD0_FILLED', l1SeedThresholds=['FSNOSEED'], stream=['IDMonitoring'],groups=['PS:Online','RATE:Monitoring','BW:Detector']),
        ChainProp(name='HLT_noalg_idmon_L1RD0_UNPAIRED_ISO', l1SeedThresholds=['FSNOSEED'], stream=['IDMonitoring'],groups=['PS:Online','RATE:Monitoring','BW:Detector']),

        # L1 combined streamers
        ChainProp(name='HLT_noalg_L1Bkg',      l1SeedThresholds=['FSNOSEED'], stream=['Background'], groups=['PS:Online','RATE:SeededStreamers', 'BW:Other']+SupportLegGroup),
        ChainProp(name='HLT_noalg_L1Standby',      l1SeedThresholds=['FSNOSEED'], stream=['Standby'], groups=['PS:Online','RATE:SeededStreamers', 'BW:Other']+SupportLegGroup),
        ChainProp(name='HLT_noalg_L1Calo',      l1SeedThresholds=['FSNOSEED'], stream=['L1Calo'], groups=['PS:Online','RATE:SeededStreamers', 'BW:Other']+SupportLegGroup),
        ChainProp(name='HLT_noalg_L1Calo_EMPTY',      l1SeedThresholds=['FSNOSEED'], stream=['L1Calo'], groups=['PS:Online','RATE:SeededStreamers', 'BW:Other']+SupportLegGroup),

        # muon streamers
        ChainProp(name='HLT_noalg_L1MU3V_UNPAIRED_ISO', l1SeedThresholds=['FSNOSEED'], stream=['Background'], groups=['PS:Online']+SingleMuonGroup),
        ChainProp(name='HLT_noalg_cosmicmuons_L1MU3V_EMPTY', l1SeedThresholds=['FSNOSEED'], stream=['CosmicMuons','express'],groups=['PS:Online', 'RATE:Cosmic_Muon','BW:Muon']),
        ChainProp(name='HLT_noalg_cosmicmuons_L1MU8VF_EMPTY', l1SeedThresholds=['FSNOSEED'], stream=['CosmicMuons','express'],groups=['PS:Online', 'RATE:Cosmic_Muon','BW:Muon']),
        ChainProp(name='HLT_noalg_bkg_L1MU3V_EMPTY', l1SeedThresholds=['FSNOSEED'], stream=['Background'],groups=['PS:Online','BW:Other']),
        ChainProp(name='HLT_noalg_bkg_L1MU8VF_EMPTY', l1SeedThresholds=['FSNOSEED'], stream=['Background'],groups=['PS:Online','BW:Other']),

        # L1 calo streamers
        ChainProp(name='HLT_noalg_l1calo_L1J400', l1SeedThresholds=['FSNOSEED'], stream=['L1Calo'], groups=['PS:Online']+JetStreamersGroup+['BW:Other']+SupportLegGroup),
        
        # Cosmic calo stream
        ChainProp(name='HLT_noalg_L1EM3_EMPTY', l1SeedThresholds=['FSNOSEED'], stream=['CosmicCalo'],groups=['PS:Online','RATE:Cosmic_Calo','BW:MinBias','RATE:Calibration']+SupportLegGroup),
        ChainProp(name='HLT_noalg_L1RD1_EMPTY', l1SeedThresholds=['FSNOSEED'], stream=['CosmicCalo','express'],groups=['PS:Online','RATE:Calibration','BW:Detector']),
        ChainProp(name='HLT_noalg_L1J30p31ETA49_EMPTY', l1SeedThresholds=['FSNOSEED'], stream=['CosmicCalo'],groups=['PS:Online','RATE:Cosmic_Calo','RATE:Calibration','BW:Jet']+SupportLegGroup),
        ChainProp(name='HLT_noalg_L1J12_EMPTY', l1SeedThresholds=['FSNOSEED'], stream=['CosmicCalo','express'],groups=['PS:Online','RATE:Cosmic_Calo','RATE:Calibration','BW:Jet']+SupportLegGroup),
        ChainProp(name='HLT_noalg_L1EM7_EMPTY', l1SeedThresholds=['FSNOSEED'], stream=['CosmicCalo','express'],groups=['PS:Online','RATE:Cosmic_Calo','RATE:Calibration','BW:Jet']+SupportLegGroup),
        ChainProp(name='HLT_noalg_L1J30_EMPTY', l1SeedThresholds=['FSNOSEED'], stream=['CosmicCalo','express'],groups=['PS:Online','RATE:Cosmic_Calo','RATE:Calibration','BW:Jet']+SupportLegGroup),
        ChainProp(name='HLT_noalg_L1J12_FIRSTEMPTY', l1SeedThresholds=['FSNOSEED'], stream=['CosmicCalo','express'],groups=['PS:Online','RATE:Cosmic_Calo','RATE:Calibration','BW:Jet']+SupportLegGroup),
        ChainProp(name='HLT_noalg_L1J30_FIRSTEMPTY', l1SeedThresholds=['FSNOSEED'], stream=['CosmicCalo','express'],groups=['PS:Online','RATE:Cosmic_Calo','RATE:Calibration','BW:Jet']+SupportLegGroup),
        ChainProp(name='HLT_noalg_L1RD0_FIRSTEMPTY', l1SeedThresholds=['FSNOSEED'], stream=['CosmicCalo'], groups=['PS:Online','RATE:Calibration','BW:Detector']),
        ChainProp(name='HLT_noalg_L1TAU8_EMPTY', l1SeedThresholds=['FSNOSEED'], stream=['CosmicCalo','express'],groups=['PS:Online','RATE:Cosmic_Calo','RATE:Calibration','BW:Jet']+SupportLegGroup),
        ChainProp(name='HLT_noalg_L1RD0_BGRP7', l1SeedThresholds=['FSNOSEED'], stream=['CosmicCalo'], groups=['PS:Online','RATE:Calibration','BW:Detector']),

        ChainProp(name='HLT_noalg_LArPEBNoise_L1EM3_EMPTY',  l1SeedThresholds=['FSNOSEED'],  stream=['LArCellsEmpty'],groups=['PS:Online','RATE:Calibration','BW:Detector']+SupportLegGroup),
        ChainProp(name='HLT_noalg_LArPEBNoise_L1EM7_EMPTY',  l1SeedThresholds=['FSNOSEED'], stream=['LArCellsEmpty'],groups=['PS:Online','RATE:Calibration','BW:Detector']+SupportLegGroup),
        ChainProp(name='HLT_noalg_LArPEBNoise_L1J12_EMPTY',  l1SeedThresholds=['FSNOSEED'], stream=['LArCellsEmpty'],groups=['PS:Online','RATE:Calibration','BW:Detector']+SupportLegGroup),
        ChainProp(name='HLT_noalg_LArPEBNoise_L1TAU8_EMPTY', l1SeedThresholds=['FSNOSEED'], stream=['LArCellsEmpty'],groups=['PS:Online','RATE:Calibration','BW:Detector']+SupportLegGroup),
        ChainProp(name='HLT_noalg_LArPEBNoise_L1J30p31ETA49_EMPTY', l1SeedThresholds=['FSNOSEED'], stream=['LArCellsEmpty'],groups=['PS:Online','RATE:Calibration','BW:Detector']+SupportLegGroup),
        #                         
        ChainProp(name='HLT_noalg_LArPEBNoise_L1EM7_FIRSTEMPTY',  l1SeedThresholds=['FSNOSEED'],  stream=['LArCellsEmpty'],groups=['PS:Online','RATE:Calibration','BW:Detector']+SupportLegGroup),
        ChainProp(name='HLT_noalg_LArPEBNoise_L1J12_FIRSTEMPTY',  l1SeedThresholds=['FSNOSEED'],  stream=['LArCellsEmpty'],groups=['PS:Online','RATE:Calibration','BW:Detector']+SupportLegGroup),
        ChainProp(name='HLT_noalg_LArPEBNoise_L1J30_FIRSTEMPTY',  l1SeedThresholds=['FSNOSEED'],  stream=['LArCellsEmpty'],groups=['PS:Online','RATE:Calibration','BW:Detector']+SupportLegGroup),
        
        # TGC
        ChainProp(name='HLT_noalg_L1TGC_BURST',  l1SeedThresholds=['FSNOSEED'], stream=['TgcNoiseBurst'],groups=['PS:Online','RATE:Calibration','BW:Detector']),

#        ChainProp(name='HLT_noalg_L1RD1_BGRP10', l1SeedThresholds=['FSNOSEED'], stream=['CosmicCalo','express'],groups=['RATE:Calibration','BW:Detector']),

        # MBTS
        ChainProp(name='HLT_noalg_L1MBTS_A', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup),
        ChainProp(name='HLT_noalg_L1MBTS_C', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup),

        ChainProp(name='HLT_noalg_L1MBTS_1_EMPTY', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup), #ATR-21740
        ChainProp(name='HLT_noalg_L1RD2_EMPTY', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup), # ATR-21367
        ChainProp(name='HLT_noalg_L1ZB',        l1SeedThresholds=['FSNOSEED'], stream=['ZeroBias'],groups=['PS:Online']+ZeroBiasGroup+SupportLegGroup),# ATR-21367
        ChainProp(name='HLT_noalg_L1MBTS_2_EMPTY', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup), #ATR-21999
        ChainProp(name='HLT_noalg_L1MBTS_1_1_EMPTY', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup), #ATR-21999

#        ChainProp(name='HLT_noalg_L1MBTSA0', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup), #ATR-23216
#        ChainProp(name='HLT_noalg_L1MBTSA1', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup), #ATR-23216
#        ChainProp(name='HLT_noalg_L1MBTSA2', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup), #ATR-23216
#        ChainProp(name='HLT_noalg_L1MBTSA3', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup), #ATR-23216
#        ChainProp(name='HLT_noalg_L1MBTSA4', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup), #ATR-23216
#        ChainProp(name='HLT_noalg_L1MBTSA5', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup), #ATR-23216
#        ChainProp(name='HLT_noalg_L1MBTSA6', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup), #ATR-23216
#        ChainProp(name='HLT_noalg_L1MBTSA7', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup), #ATR-23216
#        ChainProp(name='HLT_noalg_L1MBTSA8', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup), #ATR-23216
#        ChainProp(name='HLT_noalg_L1MBTSA9', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup), #ATR-23216
#        ChainProp(name='HLT_noalg_L1MBTSA10', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup), #ATR-23216
#        ChainProp(name='HLT_noalg_L1MBTSA11', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup), #ATR-23216
#        ChainProp(name='HLT_noalg_L1MBTSA12', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup), #ATR-23216
#        ChainProp(name='HLT_noalg_L1MBTSA13', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup), #ATR-23216
#        ChainProp(name='HLT_noalg_L1MBTSA14', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup), #ATR-23216
#        ChainProp(name='HLT_noalg_L1MBTSA15', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup), #ATR-23216
#        
#        ChainProp(name='HLT_noalg_L1MBTSC0', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup), #ATR-23216
#        ChainProp(name='HLT_noalg_L1MBTSC1', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup), #ATR-23216
#        ChainProp(name='HLT_noalg_L1MBTSC2', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup), #ATR-23216
#        ChainProp(name='HLT_noalg_L1MBTSC3', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup), #ATR-23216
#        ChainProp(name='HLT_noalg_L1MBTSC4', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup), #ATR-23216
#        ChainProp(name='HLT_noalg_L1MBTSC5', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup), #ATR-23216
#        ChainProp(name='HLT_noalg_L1MBTSC6', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup), #ATR-23216
#        ChainProp(name='HLT_noalg_L1MBTSC7', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup), #ATR-23216
#        ChainProp(name='HLT_noalg_L1MBTSC8', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup), #ATR-23216
#        ChainProp(name='HLT_noalg_L1MBTSC9', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup), #ATR-23216
#        ChainProp(name='HLT_noalg_L1MBTSC10', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup), #ATR-23216
#        ChainProp(name='HLT_noalg_L1MBTSC11', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup), #ATR-23216
#        ChainProp(name='HLT_noalg_L1MBTSC12', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup), #ATR-23216
#        ChainProp(name='HLT_noalg_L1MBTSC13', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup), #ATR-23216
#        ChainProp(name='HLT_noalg_L1MBTSC14', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup), #ATR-23216
#        ChainProp(name='HLT_noalg_L1MBTSC15', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['PS:Online']+MinBiasGroup), #ATR-23216

#        ChainProp(name='HLT_noalg_L1MBTS_1', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=MinBiasGroup+['PS:Online']+LowMuGroup+SupportLegGroup),
#        ChainProp(name='HLT_noalg_L1MBTS_2', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=MinBiasGroup+['PS:Online']+LowMuGroup+SupportLegGroup),
#        ChainProp(name='HLT_noalg_L1MBTS_1_1', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=MinBiasGroup+['PS:Online']+LowMuGroup+SupportLegGroup),
#        ChainProp(name='HLT_noalg_L1MBTS_1_UNPAIRED_ISO', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=MinBiasGroup+['PS:Online']+LowMuGroup+SupportLegGroup),
#        ChainProp(name='HLT_noalg_L1MBTS_2_UNPAIRED_ISO', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=MinBiasGroup+['PS:Online']+LowMuGroup+SupportLegGroup),
#        ChainProp(name='HLT_noalg_L1MBTS_1_1_UNPAIRED_ISO', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=MinBiasGroup+['PS:Online']+LowMuGroup+SupportLegGroup),


        # ZDCCalib
        ChainProp(name='HLT_noalg_ZDCPEB_L1ZDC_A', l1SeedThresholds=['FSNOSEED'], stream=['ZDCCalib'], groups=MinBiasGroup+['PS:Online']+SupportGroup),
        ChainProp(name='HLT_noalg_ZDCPEB_L1ZDC_C', l1SeedThresholds=['FSNOSEED'], stream=['ZDCCalib'], groups=MinBiasGroup+['PS:Online']+SupportGroup),
        ChainProp(name='HLT_noalg_ZDCPEB_L1ZDC_AND', l1SeedThresholds=['FSNOSEED'], stream=['ZDCCalib'], groups=MinBiasGroup+['PS:Online']+SupportGroup),
 
    ]

    chainsP1['Monitor'] = [
        ChainProp(name='HLT_timeburner_L1All', l1SeedThresholds=['FSNOSEED'], stream=['DISCARD'], groups=['PS:Online','RATE:DISCARD','BW:DISCARD']),
    ]


    # check all chains have the 'PS:Online' group, so that they are not simulated
    for sig in chainsP1:
        for chain in chainsP1[sig]:
            if 'Main' in chain.stream:
                log.error("chain %s in PhysicsP1 menu with Main stream. Please move this to Physics menu file", chain.name)
                raise RuntimeError("Move %s chain to Physics menu file",chain.name)            
            if 'PS:Online' not in chain.groups:
                log.error("chain %s in PhysicsP1 menu needs to the group 'PS:Online'", chain.name)
                raise RuntimeError("Add the group 'PS:Online' to the chain %s",chain.name)                

    for sig in chainsP1:
        chains[sig] += chainsP1[sig]

def setupMenu():

    from AthenaCommon.Logging                 import logging
    log = logging.getLogger(__name__)
    log.info('setupMenu ...')

    chains = physics_menu.setupMenu()
    addP1Signatures(chains)
    return chains
