# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

#------------------------------------------------------------------------#
# MC_pp_run3_v1.py menu for the long shutdown development
#------------------------------------------------------------------------#

# This defines the input format of the chain and it's properties with the defaults set
# always required are: name, stream and groups
#['name', 'L1chainParts'=[], 'stream', 'groups', 'merging'=[], 'topoStartFrom'=False],
#from TriggerMenuMT.HLT.Menu.ChainDefInMenu import ChainProp

import TriggerMenuMT.HLT.Menu.Physics_pp_run3_v1 as physics_menu 
from TriggerMenuMT.HLT.Menu.ChainDefInMenu import ChainProp
from TriggerMenuMT.HLT.Menu.Physics_pp_run3_v1 import (SingleElectronGroup,
                                                       BphysicsGroup,
                                                       EOFBPhysL1MuGroup,
)

def setupMenu():

    from AthenaCommon.Logging import logging
    log = logging.getLogger( __name__ )
    log.info('[setupMenu] going to add the MC menu chains now')
    
    chains = physics_menu.setupMenu()

    chains['Egamma'] += [

        ChainProp(name='HLT_e5_etcut_L1EM3', groups=SingleElectronGroup),
        ChainProp(name='HLT_e5_etcut_L1eEM5', groups=SingleElectronGroup),
        ChainProp(name='HLT_e26_etcut_L1EM22VHI', groups=SingleElectronGroup),
        ChainProp(name='HLT_e26_etcut_L1eEM26M', groups=SingleElectronGroup),
        ChainProp(name='HLT_e60_etcut_L1EM22VHI', groups=SingleElectronGroup),
        ChainProp(name='HLT_e60_etcut_L1eEM26M', groups=SingleElectronGroup),
        ChainProp(name='HLT_e5_lhtight_gsf_L1EM3', groups=SingleElectronGroup),
        ChainProp(name='HLT_e5_lhtight_gsf_L1eEM5', groups=SingleElectronGroup),

    ]

    chains['Bphysics'] += [    

        #ATR-21566, chains for di-muon TLA, but with HLT selections to test rates. Here streaming into BphysDelayed (not in TLA stream)   
        ChainProp(name='HLT_2mu4_b7invmAB22vtx20_L1BPH-7M22-2MU3VF', l1SeedThresholds=['MU3VF'],stream=['BphysDelayed'], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_mu6_mu4_b7invmAB22vtx20_L1BPH-7M22-MU5VFMU3VF', l1SeedThresholds=['MU5VF','MU3VF'],stream=['BphysDelayed'], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_2mu4_b0dRAB207invmAB22vtx20_L1BPH-7M22-0DR20-2MU3V', l1SeedThresholds=['MU3V'],stream=['BphysDelayed'], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_2mu4_b0dRAB207invmAB22vtx20_L1BPH-7M22-0DR20-2MU3VF', l1SeedThresholds=['MU3VF'],stream=['BphysDelayed'], groups=BphysicsGroup+EOFBPhysL1MuGroup),
        ChainProp(name='HLT_2mu4_b0dRAB127invmAB22vtx20_L1BPH-7M22-0DR12-2MU3V', l1SeedThresholds=['MU3V'],stream=['BphysDelayed'], groups=BphysicsGroup+EOFBPhysL1MuGroup),

    ]

    return chains
