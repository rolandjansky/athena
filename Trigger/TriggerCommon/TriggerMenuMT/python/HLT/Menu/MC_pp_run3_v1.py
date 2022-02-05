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


    return chains
