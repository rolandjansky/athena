# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#------------------------------------------------------------------------#
# Cosmic_run3_v1.py menu cosmics running with MT framework
#------------------------------------------------------------------------#

# This defines the input format of the chain and it's properties with the defaults set
# always required are: name, stream and groups
#['name', 'L1chainParts'=[], 'stream', 'groups', 'merging'=[], 'topoStartFrom'=False],
from TriggerMenuMT.HLTMenuConfig.Menu.ChainDefInMenu import ChainProp
from TriggerMenuMT.HLTMenuConfig.Menu.PhysicsP1_pp_run3_v1 import addP1Signatures
from TriggerMenuMT.HLTMenuConfig.Menu.Physics_pp_run3_v1 import (
    SinglePhotonGroup,
    SingleJetGroup,
    SingleBjetGroup,
    SingleMuonGroup,
    MultiMuonGroup,
    EgammaMuonGroup,
    PrimaryLegGroup,
    PrimaryL1MuGroup,
)

def setupMenu():
    from TriggerJobOpts.TriggerFlags import TriggerFlags

    TriggerFlags.Slices_all_setOff()

    TriggerFlags.TestSlice.signatures = []

    TriggerFlags.MuonSlice.signatures = [
        ChainProp(name='HLT_mu4_cosmic_L1MU4_EMPTY', l1SeedThresholds=['MU4'], stream=['CosmicMuons'], groups=['RATE:Cosmic_Muon','BW:Muon']),
        ChainProp(name='HLT_mu4_cosmic_L1MU4', l1SeedThresholds=['MU4'], stream=['CosmicMuons'], groups=['RATE:Cosmic_Muon','BW:Muon']),
        ChainProp(name='HLT_mu4_msonly_cosmic_L1MU4_EMPTY', l1SeedThresholds=['MU4'], stream=['CosmicMuons'], groups=['RATE:Cosmic_Muon','BW:Muon']),

        ChainProp(name='HLT_3mu6_msonly_L1MU4_EMPTY', l1SeedThresholds=['MU4'], stream=['Late'], groups=PrimaryL1MuGroup+MultiMuonGroup),
        ChainProp(name='HLT_3mu6_msonly_L1MU6_EMPTY', l1SeedThresholds=['MU6'], stream=['Late'], groups=PrimaryL1MuGroup+MultiMuonGroup),

        ChainProp(name='HLT_mu60_0eta105_msonly_L1MU20', l1SeedThresholds=['MU20'], stream=['Main'], groups=PrimaryL1MuGroup+SingleMuonGroup),
        ChainProp(name='HLT_mu60_msonly_3layersEC_L1MU20', l1SeedThresholds=['MU20'], stream=['Main'], groups=PrimaryL1MuGroup+SingleMuonGroup),
        ChainProp(name='HLT_mu80_msonly_3layersEC_L1MU20', l1SeedThresholds=['MU20'], stream=['Main'], groups=PrimaryL1MuGroup+SingleMuonGroup),
     ]

    TriggerFlags.EgammaSlice.signatures = [
        ChainProp(name='HLT_g3_etcut_LArPEB_L1EM3',stream=['LArCells'], groups=['RATE:SinglePhoton', 'BW:Egamma']),
        ChainProp(name='HLT_g3_loose_LArPEBHLT_L1EM3',	   stream=['LArCells'], groups=['PS:Online']+SinglePhotonGroup),
        ChainProp(name='HLT_g12_loose_LArPEBHLT_L1EM10VH', stream=['LArCells'], groups=['PS:Online']+SinglePhotonGroup),
        ChainProp(name='HLT_g20_loose_LArPEBHLT_L1EM15',   stream=['LArCells'], groups=['PS:Online']+SinglePhotonGroup),
        ChainProp(name='HLT_g40_loose_LArPEBHLT_L1EM20VHI',stream=['LArCells'], groups=['PS:Online']+SinglePhotonGroup),
        ChainProp(name='HLT_g60_loose_LArPEBHLT_L1EM20VHI',stream=['LArCells'], groups=['PS:Online']+SinglePhotonGroup),
        ChainProp(name='HLT_g80_loose_LArPEBHLT_L1EM20VHI',stream=['LArCells'], groups=['PS:Online']+SinglePhotonGroup),

        ChainProp(name='HLT_e5_etcut_L1EM3',stream=['Main'], groups=['RATE:SingleElectron', 'BW:Egamma']),
    ]

    TriggerFlags.METSlice.signatures = []
    TriggerFlags.JetSlice.signatures = [
        ChainProp(name='HLT_j40_LArPEBHLT_L1J20', l1SeedThresholds=['FSNOSEED'],			stream=['LArCells'], groups=['PS:Online']+SingleJetGroup),
        ChainProp(name='HLT_j165_LArPEBHLT_L1J100', l1SeedThresholds=['FSNOSEED'],			stream=['LArCells'], groups=['PS:Online']+SingleJetGroup),
        ChainProp(name='HLT_j75_320eta490_LArPEBHLT_L1J30p31ETA49', l1SeedThresholds=['FSNOSEED'],	stream=['LArCells'], groups=['PS:Online']+SingleJetGroup),
        ChainProp(name='HLT_j140_320eta490_LArPEBHLT_L1J75p31ETA49', l1SeedThresholds=['FSNOSEED'],	stream=['LArCells'], groups=['PS:Online']+SingleJetGroup),
        ChainProp(name='HLT_j25_LArPEBHLT_L1J15',   l1SeedThresholds=['FSNOSEED'],			stream=['LArCells'], groups=['PS:Online']+SingleJetGroup),
        ChainProp(name='HLT_j25_320eta490_LArPEBHLT_L1J15p31ETA49',   l1SeedThresholds=['FSNOSEED'], stream=['LArCells'], groups=['PS:Online']+SingleJetGroup),

        ChainProp(name='HLT_j15_L1J12_EMPTY',   l1SeedThresholds=['FSNOSEED'], stream=['Main'], groups=PrimaryLegGroup+SingleJetGroup),
        ChainProp(name='HLT_j0_HT0_L1J12_EMPTY',   l1SeedThresholds=['FSNOSEED'], stream=['Main'], groups=PrimaryLegGroup+SingleJetGroup),
        
    ]
    TriggerFlags.BjetSlice.signatures = [
        ChainProp(name='HLT_j0_ftf_boffperf_L1MU10',   l1SeedThresholds=['FSNOSEED'], stream=['Main'], groups=PrimaryLegGroup+SingleBjetGroup),
        ChainProp(name='HLT_j0_ftf_boffperf_L1RD0_EMPTY',   l1SeedThresholds=['FSNOSEED'], stream=['Main'], groups=PrimaryLegGroup+SingleBjetGroup),
        ChainProp(name='HLT_j0_ftf_boffperf_L1J12_EMPTY',   l1SeedThresholds=['FSNOSEED'], stream=['Main'], groups=PrimaryLegGroup+SingleBjetGroup),
    ]
    TriggerFlags.TauSlice.signatures = []
    TriggerFlags.BphysicsSlice.signatures = []
    TriggerFlags.CombinedSlice.signatures = [
        ChainProp(name='HLT_g15_loose_2mu10_msonly_L1MU4_EMPTY', l1SeedThresholds=['EM8VH','MU4'], stream=['Main'], groups=PrimaryLegGroup+EgammaMuonGroup),
        ChainProp(name='HLT_g15_loose_2mu10_msonly_L1MU6_EMPTY', l1SeedThresholds=['EM8VH','MU6'], stream=['Main'], groups=PrimaryLegGroup+EgammaMuonGroup),
    ]
    TriggerFlags.HeavyIonSlice.signatures  = []
    TriggerFlags.BeamspotSlice.signatures  = []
    TriggerFlags.MinBiasSlice.signatures   = [
        ChainProp(name='HLT_mb_sptrk_costr_L1RD0_FILLED', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['Rate:MinBias','BW:MinBias']),
        ChainProp(name='HLT_mb_sptrk_costr_L1RD0_EMPTY', l1SeedThresholds=['FSNOSEED'], stream=['MinBias'], groups=['Rate:Cosmic_MinBias','BW:MinBias']),
    ]
    TriggerFlags.CalibSlice.signatures     = []
    TriggerFlags.CosmicSlice.signatures    = []
    TriggerFlags.StreamingSlice.signatures = [

        ChainProp(name='HLT_noalg_L1TRT_EMPTY', l1SeedThresholds=['FSNOSEED'], stream=['IDCosmic','express'],groups=['RATE:SeededStreamers','BW:Other']),
        ChainProp(name='HLT_noalg_L1TRT_FILLED', l1SeedThresholds=['FSNOSEED'], stream=['IDCosmic'],groups=['RATE:SeededStreamers','BW:Other']),

        ChainProp(name='HLT_noalg_L1MU4_EMPTY', l1SeedThresholds=['FSNOSEED'], stream=['CosmicMuons','express'],groups=['RATE:Cosmic_Muon','BW:Muon']),
        ChainProp(name='HLT_noalg_L1MU11_EMPTY', l1SeedThresholds=['FSNOSEED'], stream=['CosmicMuons','express'],groups=['RATE:Cosmic_Muon','BW:Muon']),


]
    TriggerFlags.MonitorSlice.signatures   = []

    # Random Seeded EB chains which select at the HLT based on L1 TBP bits
    TriggerFlags.EnhancedBiasSlice.signatures = [ ]

    # Add all standard monitoring chains from addP1Signatures function
    addP1Signatures()


