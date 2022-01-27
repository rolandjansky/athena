# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#------------------------------------------------------------------------#
# Dev_HI_run3_v1.py menu for the long shutdown development
#------------------------------------------------------------------------#

# This defines the input format of the chain and it's properties with the defaults set
# always required are: name, stream and groups
#['name', 'L1chainParts'=[], 'stream', 'groups', 'merging'=[], 'topoStartFrom'=False],


from TriggerMenuMT.HLT.Menu.ChainDefInMenu import ChainProp

from TriggerMenuMT.HLT.Menu.Physics_pp_run3_v1 import SingleMuonGroup,MinBiasGroup,MultiMuonGroup,SinglePhotonGroup,SingleElectronGroup,MultiElectronGroup,PrimaryLegGroup,PrimaryL1MuGroup,SupportLegGroup,SupportGroup,SingleJetGroup

import TriggerMenuMT.HLT.Menu.PhysicsP1_HI_run3_v1 as HIp1_menu

from TriggerMenuMT.HLT.Menu.PhysicsP1_HI_run3_v1 import HardProbesStream,MinBiasStream



def setupMenu():

    chains = HIp1_menu.setupMenu()

    from AthenaCommon.Logging                 import logging
    log = logging.getLogger( __name__ )
    log.info('setupMenu ...')

    chains['Muon'] += [
        #-- 1 mu
        ChainProp(name='HLT_mu8_L1MU5VF',  stream=[HardProbesStream], groups=SingleMuonGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_mu10_L1MU5VF', stream=[HardProbesStream], groups=SingleMuonGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_mu10_L1MU8F',  stream=[HardProbesStream], groups=SingleMuonGroup+PrimaryL1MuGroup),
            
        #-- 2 mu
        ChainProp(name='HLT_2mu4_L12MU3V', stream=[HardProbesStream], groups=MultiMuonGroup+PrimaryL1MuGroup),
        ChainProp(name='HLT_mu4_mu4noL1_L1MU3V', stream=[HardProbesStream], l1SeedThresholds=['MU3V','FSNOSEED'], groups=MultiMuonGroup+PrimaryL1MuGroup),

     ]

    chains['Egamma'] += [
        # ElectronChains----------
        #--------- e etcut
        ChainProp(name='HLT_e15_etcut_ion_L1EM12', stream=[HardProbesStream] ,groups=SingleElectronGroup+SupportLegGroup),
        ChainProp(name='HLT_e20_etcut_ion_L1EM16', stream=[HardProbesStream] ,groups=SingleElectronGroup+SupportLegGroup),
        
        #--------- e lh
        ChainProp(name='HLT_e15_lhloose_ion_L1EM12', stream=[HardProbesStream], groups=SingleElectronGroup+PrimaryLegGroup),
        ChainProp(name='HLT_e20_lhloose_ion_L1EM16', stream=[HardProbesStream], groups=SingleElectronGroup+PrimaryLegGroup),
        ChainProp(name='HLT_e15_lhmedium_ion_L1EM12', stream=[HardProbesStream], groups=SingleElectronGroup+PrimaryLegGroup),
        ChainProp(name='HLT_e20_lhmedium_ion_L1EM16', stream=[HardProbesStream], groups=SingleElectronGroup+PrimaryLegGroup),
        
        #--------- e isEM
        ChainProp(name='HLT_e15_loose_ion_L1EM12', stream=[HardProbesStream], groups=SingleElectronGroup+PrimaryLegGroup),
        ChainProp(name='HLT_e20_loose_ion_L1EM16', stream=[HardProbesStream], groups=SingleElectronGroup+PrimaryLegGroup),
        ChainProp(name='HLT_e15_medium_ion_L1EM12', stream=[HardProbesStream], groups=SingleElectronGroup+PrimaryLegGroup),
        ChainProp(name='HLT_e20_medium_ion_L1EM16', stream=[HardProbesStream], groups=SingleElectronGroup+PrimaryLegGroup),
        
        #---------- 2e
        ChainProp(name='HLT_2e20_loose_ion_L12EM16', stream=[HardProbesStream], groups=MultiElectronGroup+PrimaryLegGroup),
        
        # PhotonChains----------
        #----------- g etcut
        ChainProp(name='HLT_g15_etcut_ion_L1EM12', stream=[HardProbesStream], groups=SinglePhotonGroup+SupportLegGroup),
        ChainProp(name='HLT_g20_etcut_ion_L1EM12', stream=[HardProbesStream], groups=SinglePhotonGroup+SupportLegGroup),
        
        #----------- g isEM
        ChainProp(name='HLT_g15_loose_ion_L1EM12', stream=[HardProbesStream], groups=SinglePhotonGroup+PrimaryLegGroup),
        ChainProp(name='HLT_g20_loose_ion_L1EM12', stream=[HardProbesStream], groups=SinglePhotonGroup+PrimaryLegGroup),
    ]

    chains['Jet'] += [
        ChainProp(name='HLT_j40_ion_L1RD0_FILLED', l1SeedThresholds=['FSNOSEED'], stream=[HardProbesStream], groups=SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j50_ion_L1RD0_FILLED', l1SeedThresholds=['FSNOSEED'], stream=[HardProbesStream], groups=SingleJetGroup+SupportLegGroup),
        ChainProp(name='HLT_j60_ion_L1J15', l1SeedThresholds=['FSNOSEED'], stream=[HardProbesStream], groups=SingleJetGroup+PrimaryLegGroup),
        ChainProp(name='HLT_j75_ion_L1J20', l1SeedThresholds=['FSNOSEED'], stream=[HardProbesStream], groups=SingleJetGroup+PrimaryLegGroup),
        ChainProp(name='HLT_j75_ion_L1J30', l1SeedThresholds=['FSNOSEED'], stream=[HardProbesStream], groups=SingleJetGroup+PrimaryLegGroup),
        ChainProp(name='HLT_j85_ion_L1J15', l1SeedThresholds=['FSNOSEED'], stream=[HardProbesStream], groups=SingleJetGroup+PrimaryLegGroup),
        ChainProp(name='HLT_j85_ion_L1J30', l1SeedThresholds=['FSNOSEED'], stream=[HardProbesStream], groups=SingleJetGroup+PrimaryLegGroup),
    ]

    chains['MinBias'] += [
        ChainProp(name='HLT_mb_sp_L1RD0_FILLED', l1SeedThresholds=['FSNOSEED'], stream=[MinBiasStream], groups=MinBiasGroup+SupportGroup),
    ]

    return chains
