# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

#------------------------------------------------------------------------#
# Dev_HI_run3_v1.py menu for the long shutdown development
#------------------------------------------------------------------------#

# This defines the input format of the chain and it's properties with the defaults set
# always required are: name, stream and groups
#['name', 'L1chainParts'=[], 'stream', 'groups', 'merging'=[], 'topoStartFrom'=False],


from TriggerMenuMT.HLTMenuConfig.Menu.ChainDefInMenu import ChainProp

from TriggerMenuMT.HLTMenuConfig.Menu.Physics_pp_run3_v1 import SingleMuonGroup,SinglePhotonGroup,MinBiasGroup,MultiMuonGroup,SingleElectronGroup,MultiElectronGroup,PrimaryLegGroup,PrimaryL1MuGroup,SupportLegGroup,SupportGroup

import TriggerMenuMT.HLTMenuConfig.Menu.PhysicsP1_HI_run3_v1 as HIp1_menu

from TriggerMenuMT.HLTMenuConfig.Menu.PhysicsP1_HI_run3_v1 import HardProbesStream,MinBiasStream



def setupMenu():

    HIp1_menu.setupMenu()

    from TriggerJobOpts.TriggerFlags          import TriggerFlags
    from AthenaCommon.Logging                 import logging
    log = logging.getLogger( __name__ )
    log.info('setupMenu ...')


    TriggerFlags.TestSlice.signatures = TriggerFlags.TestSlice.signatures() + []
    
    TriggerFlags.MuonSlice.signatures = TriggerFlags.MuonSlice.signatures() + [
        #-- 1 mu
        ChainProp(name='HLT_mu8_L1MU5VF', stream=[HardProbesStream], groups=SingleMuonGroup+PrimaryL1MuGroup),
            
        #-- 2 mu
        ChainProp(name='HLT_2mu4_L12MU3V', stream=[HardProbesStream], groups=MultiMuonGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_mu4_mu4noL1_L1MU3V', stream=[HardProbesStream], l1SeedThresholds=['MU3V','FSNOSEED'], groups=MultiMuonGroup+PrimaryL1MuGroup),

     ]

    TriggerFlags.EgammaSlice.signatures = TriggerFlags.EgammaSlice.signatures() + [
        # ElectronChains----------
        #--------- e etcut
        ChainProp(name='HLT_e15_etcut_ion_L1EM12', stream=[HardProbesStream] ,groups=SingleElectronGroup+SupportLegGroup),
        ChainProp(name='HLT_e20_etcut_ion_L1EM15', stream=[HardProbesStream] ,groups=SingleElectronGroup+SupportLegGroup),
        
        #--------- e lh
        ChainProp(name='HLT_e15_lhloose_ion_L1EM12', stream=[HardProbesStream], groups=SingleElectronGroup+PrimaryLegGroup),
        ChainProp(name='HLT_e20_lhloose_ion_L1EM15', stream=[HardProbesStream], groups=SingleElectronGroup+PrimaryLegGroup),
        ChainProp(name='HLT_e15_lhmedium_ion_L1EM12', stream=[HardProbesStream], groups=SingleElectronGroup+PrimaryLegGroup),
        ChainProp(name='HLT_e20_lhmedium_ion_L1EM15', stream=[HardProbesStream], groups=SingleElectronGroup+PrimaryLegGroup),
        
        #--------- e isEM
        ChainProp(name='HLT_e15_loose_ion_L1EM12', stream=[HardProbesStream], groups=SingleElectronGroup+PrimaryLegGroup),
        ChainProp(name='HLT_e20_loose_ion_L1EM15', stream=[HardProbesStream], groups=SingleElectronGroup+PrimaryLegGroup),
        ChainProp(name='HLT_e15_medium_ion_L1EM12', stream=[HardProbesStream], groups=SingleElectronGroup+PrimaryLegGroup),
        ChainProp(name='HLT_e20_medium_ion_L1EM15', stream=[HardProbesStream], groups=SingleElectronGroup+PrimaryLegGroup),
        
        #---------- 2e
        ChainProp(name='HLT_2e20_loose_ion_L12EM15', stream=[HardProbesStream], groups=MultiElectronGroup+PrimaryLegGroup),
        
        # PhotonChains----------
        #----------- g etcut
        ChainProp(name='HLT_g18_etcut_L1EM12', stream=[HardProbesStream], groups=SinglePhotonGroup+SupportLegGroup),
        
        #----------- g isEM
        ChainProp(name='HLT_g20_loose_L1EM12', stream=[HardProbesStream], groups=SinglePhotonGroup+SupportLegGroup),
    ]

    TriggerFlags.METSlice.signatures = TriggerFlags.METSlice.signatures() + [
    ]

    TriggerFlags.JetSlice.signatures = TriggerFlags.JetSlice.signatures() + [
    ]

    TriggerFlags.BjetSlice.signatures = TriggerFlags.BjetSlice.signatures() + [
    ]

    TriggerFlags.TauSlice.signatures = TriggerFlags.TauSlice.signatures() + [
    ]
    TriggerFlags.BphysicsSlice.signatures = TriggerFlags.BphysicsSlice.signatures() + [
    ]
    TriggerFlags.CombinedSlice.signatures = TriggerFlags.CombinedSlice.signatures() + [
   ]
    TriggerFlags.HeavyIonSlice.signatures  = TriggerFlags.HeavyIonSlice.signatures() + []
    TriggerFlags.BeamspotSlice.signatures  = TriggerFlags.BeamspotSlice.signatures() + []
    TriggerFlags.MinBiasSlice.signatures   = TriggerFlags.MinBiasSlice.signatures() + [
        ChainProp(name='HLT_mb_sp_L1RD0_FILLED', l1SeedThresholds=['FSNOSEED'], stream=[MinBiasStream], groups=MinBiasGroup+SupportGroup),

    ]
    TriggerFlags.CalibSlice.signatures     = TriggerFlags.CalibSlice.signatures() + []
    TriggerFlags.CosmicSlice.signatures    = TriggerFlags.CosmicSlice.signatures() + []
    TriggerFlags.StreamingSlice.signatures = TriggerFlags.StreamingSlice.signatures() + [
    ]
    TriggerFlags.MonitorSlice.signatures   = TriggerFlags.MonitorSlice.signatures() + [
    ]

    # Random Seeded EB chains which select at the HLT based on L1 TBP bits
    TriggerFlags.EnhancedBiasSlice.signatures = TriggerFlags.EnhancedBiasSlice.signatures() + [ ]


