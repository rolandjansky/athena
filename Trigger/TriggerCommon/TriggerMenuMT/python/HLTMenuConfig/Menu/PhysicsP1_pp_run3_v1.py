# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

#------------------------------------------------------------------------#
# PhysicsP1_pp_run3_v1.py menu for the long shutdown development
#------------------------------------------------------------------------#

# This defines the input format of the chain and it's properties with the defaults set
# always required are: name, stream and groups
#['name', 'L1chainParts'=[], 'stream', 'groups', 'merging'=[], 'topoStartFrom'=False],
#from TriggerMenuMT.HLTMenuConfig.Menu.ChainDefInMenu import ChainProp

from TriggerMenuMT.HLTMenuConfig.Menu.ChainDefInMenu import ChainProp

import TriggerMenuMT.HLTMenuConfig.Menu.Physics_pp_run3_v1 as physics_menu 

from TriggerMenuMT.HLTMenuConfig.Menu.Physics_pp_run3_v1 import PhysicsStream,SingleMuonGroup,SinglePhotonGroup,MinBiasGroup


def addP1Signatures():

    from TriggerJobOpts.TriggerFlags          import TriggerFlags
    from AthenaCommon.Logging                 import logging
    
    log = logging.getLogger( 'TriggerMenuMT.HLTMenuConfig.Menu.PhysicsP1_pp_run3_v1.py addP1Signatures()' )
    log.info('addP1Signatures ...')

    TriggerFlags.TestSlice.signatures = TriggerFlags.TestSlice.signatures() + []

    TriggerFlags.MuonSlice.signatures = TriggerFlags.MuonSlice.signatures() + [
          ChainProp(name='HLT_mu50_RPCPEBSecondaryReadout_L1MU20', stream=['RPCSecondaryReadout'], groups=SingleMuonGroup), # TODO: Move to Detector slice
     ]

    TriggerFlags.EgammaSlice.signatures = TriggerFlags.EgammaSlice.signatures() + [
          ChainProp(name='HLT_g20_etcut_LArPEB_L1EM15',stream=['LArCells'], groups=SinglePhotonGroup),
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
          ChainProp(name='HLT_noalg_L1RD0_EMPTY',  l1SeedThresholds=[''], stream=[PhysicsStream, 'BeamSpot'], groups=MinBiasGroup),  # FIXME: BeamSpot stream added just for testing, to be removed
          ChainProp(name='HLT_noalg_L1RD0_FILLED', l1SeedThresholds=[''], stream=[PhysicsStream, 'BeamSpot'], groups=MinBiasGroup),  # FIXME: BeamSpot stream added just for testing, to be removed
    ]
    TriggerFlags.MonitorSlice.signatures   = TriggerFlags.MonitorSlice.signatures() + [
          ChainProp(name='HLT_costmonitor_CostMonDS_L1All',        l1SeedThresholds=[''], stream=['CostMonitoring'], groups=['RATE:Monitoring','BW:Other']),
    ]

    # Random Seeded EB chains which select at the HLT based on L1 TBP bits
    TriggerFlags.EnhancedBiasSlice.signatures = TriggerFlags.EnhancedBiasSlice.signatures() + [ ]


def setupMenu():

    from TriggerJobOpts.TriggerFlags          import TriggerFlags
    from AthenaCommon.Logging                 import logging
    log = logging.getLogger( 'TriggerMenuMT.HLTMenuConfig.Menu.PhysicsP1_pp_run3_v1.py' )
    log.info('setupMenu ...')

    physics_menu.setupMenu()
    addP1Signatures()

    signatureList=[]
    for prop in dir(TriggerFlags):
        if prop[-5:]=='Slice':
            sliceName=prop
            slice=getattr(TriggerFlags,sliceName)
            if slice.signatures():
                signatureList.extend(slice.signatures())
            else:
                log.debug('SKIPPING '+str(sliceName))
    mySigList=[]
    for allInfo in signatureList:
        mySigList.append(allInfo[0])
    mydict={}
    for chain in mySigList:
        mydict[chain]=[-1,0,0]
    mydict.update(Prescales.HLTPrescales_cosmics)
    from copy import deepcopy
    Prescales.HLTPrescales_cosmics = deepcopy(mydict)
    

Prescales = physics_menu.Prescales
