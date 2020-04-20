# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

#------------------------------------------------------------------------#
# MC_pp_run3_v1.py menu for the long shutdown development
#------------------------------------------------------------------------#

# This defines the input format of the chain and it's properties with the defaults set
# always required are: name, stream and groups
#['name', 'L1chainParts'=[], 'stream', 'groups', 'merging'=[], 'topoStartFrom'=False],
#from TriggerMenuMT.HLTMenuConfig.Menu.ChainDefInMenu import ChainProp

import TriggerMenuMT.HLTMenuConfig.Menu.Physics_pp_run3_v1 as physics_menu 
from TriggerMenuMT.HLTMenuConfig.Menu.MenuPrescaleConfig import addSliceChainsToPrescales


def setupMenu():

    from TriggerJobOpts.TriggerFlags          import TriggerFlags
    from AthenaCommon.Logging import logging
    log = logging.getLogger( __name__ )
    log.info('setupMenu ...')

    physics_menu.setupMenu()

    TriggerFlags.TestSlice.signatures = TriggerFlags.TestSlice.signatures() + []

    TriggerFlags.MuonSlice.signatures = TriggerFlags.MuonSlice.signatures() + [
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
    TriggerFlags.BeamspotSlice.signatures  = TriggerFlags.BeamspotSlice.signatures() + []   
    TriggerFlags.MinBiasSlice.signatures   = TriggerFlags.MinBiasSlice.signatures() + [] 
    TriggerFlags.CalibSlice.signatures     = TriggerFlags.CalibSlice.signatures() + []
    TriggerFlags.CosmicSlice.signatures    = TriggerFlags.CosmicSlice.signatures() + []
    TriggerFlags.StreamingSlice.signatures = TriggerFlags.StreamingSlice.signatures() + [
    ]
    TriggerFlags.MonitorSlice.signatures   = TriggerFlags.MonitorSlice.signatures() + [
    ]

    # Random Seeded EB chains which select at the HLT based on L1 TBP bits
    TriggerFlags.EnhancedBiasSlice.signatures = TriggerFlags.EnhancedBiasSlice.signatures() + [ ]

    addSliceChainsToPrescales(TriggerFlags, Prescales.HLTPrescales_cosmics)

Prescales = physics_menu.Prescales
