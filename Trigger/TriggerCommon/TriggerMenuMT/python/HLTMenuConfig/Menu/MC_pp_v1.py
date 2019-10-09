# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

#------------------------------------------------------------------------#
# MC_pp_v1.py menu for the long shutdown development
#------------------------------------------------------------------------#

# This defines the input format of the chain and it's properties with the defaults set
# always required are: name, stream and groups
#['name', 'L1chainParts'=[], 'stream', 'groups', 'merging'=[], 'topoStartFrom'=False],
#from TriggerMenuMT.HLTMenuConfig.Menu.ChainDefInMenu import ChainProp

import TriggerMenuMT.HLTMenuConfig.Menu.LS2_v1 as physics_menu 


def setupMenu():

    from TriggerJobOpts.TriggerFlags          import TriggerFlags
    from AthenaCommon.Logging                 import logging
    log = logging.getLogger( 'TriggerMenuMT.HLTMenuConfig.Menu.MC_pp_v1.py' )

    physics_menu.setupMenu()

    #PhysicsStream="Main"
    #SingleMuonGroup = ['RATE:SingleMuon', 'BW:Muon']
    #MultiMuonGroup = ['RATE:MultiMuon', 'BW:Muon']
    #SingleElectronGroup = ['RATE:SingleElectron', 'BW:Electron']
    #MultiElectronGroup = ['RATE:MultiElectron', 'BW:Electron']
    #SinglePhotonGroup = ['RATE:SinglePhoton', 'BW:Photon']
    #MultiPhotonGroup = ['RATE:MultiPhoton', 'BW:Photon']
    #SingleMETGroup = ['RATE:MET', 'BW:MET']
    #MultiMETGroup = ['RATE:MultiMET', 'BW:MultiMET']
    #SingleJetGroup = ['RATE:SingleJet', 'BW:Jet']
    #MultiJetGroup = ['RATE:MultiJet', 'BW:Jet']
    #SingleBjetGroup = ['RATE:SingleBJet', 'BW:BJet']
    ##MultiBjetGroup = ['RATE:MultiBJet', 'BW:BJet']
    #SingleTauGroup = ['RATE:SingleTau', 'BW:Tau']
    ##MultiTauGroup = ['RATE:MultiTau', 'BW:Tau']
    #BphysicsGroup = ['RATE:Bphysics', 'BW:Bphysics']
    #MinBiasGroup = ['RATE:MinBias', 'BW:MinBias']
    #EgammaStreamersGroup = ['RATE:SeededStreamers', 'BW:Egamma']

    TriggerFlags.Slices_all_setOff()

    TriggerFlags.TestSlice.signatures = []

    TriggerFlags.MuonSlice.signatures = [
     ]

    TriggerFlags.EgammaSlice.signatures = [
    ]

    TriggerFlags.METSlice.signatures = [
    ]

    TriggerFlags.JetSlice.signatures = [
    ]

    TriggerFlags.BjetSlice.signatures = [
    ] 

    TriggerFlags.TauSlice.signatures = [
    ]
    TriggerFlags.BphysicsSlice.signatures = [
    ]
    TriggerFlags.CombinedSlice.signatures = [ 
   ]
    TriggerFlags.HeavyIonSlice.signatures  = []
    TriggerFlags.BeamspotSlice.signatures  = []   
    TriggerFlags.MinBiasSlice.signatures   = []    
    TriggerFlags.CalibSlice.signatures     = []
    TriggerFlags.CosmicSlice.signatures    = []
    TriggerFlags.StreamingSlice.signatures = [
    ]
    TriggerFlags.MonitorSlice.signatures   = [
    ]

    # Random Seeded EB chains which select at the HLT based on L1 TBP bits
    TriggerFlags.EnhancedBiasSlice.signatures = [ ]

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
