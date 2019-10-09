# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

#------------------------------------------------------------------------#
# Dev_pp_v1.py menu for the long shutdown development
#------------------------------------------------------------------------#

# This defines the input format of the chain and it's properties with the defaults set
# always required are: name, stream and groups
#['name', 'L1chainParts'=[], 'stream', 'groups', 'merging'=[], 'topoStartFrom'=False],

from TriggerMenuMT.HLTMenuConfig.Menu.ChainDefInMenu import ChainProp

import TriggerMenuMT.HLTMenuConfig.Menu.MC_pp_v1 as mc_menu

def setupMenu():

    mc_menu.setupMenu()

    from TriggerJobOpts.TriggerFlags          import TriggerFlags
    from AthenaCommon.Logging                 import logging
    log = logging.getLogger( 'TriggerMenuMT.HLTMenuConfig.Menu.LS2_pp_v1.py' )

    PhysicsStream="Main"
    SingleMuonGroup = ['RATE:SingleMuon', 'BW:Muon']
    MultiMuonGroup = ['RATE:MultiMuon', 'BW:Muon']
    SingleElectronGroup = ['RATE:SingleElectron', 'BW:Electron']
    #MultiElectronGroup = ['RATE:MultiElectron', 'BW:Electron']
    SinglePhotonGroup = ['RATE:SinglePhoton', 'BW:Photon']
    #MultiPhotonGroup = ['RATE:MultiPhoton', 'BW:Photon']
    SingleMETGroup = ['RATE:MET', 'BW:MET']
    MultiMETGroup = ['RATE:MultiMET', 'BW:MultiMET']
    SingleJetGroup = ['RATE:SingleJet', 'BW:Jet']
    MultiJetGroup = ['RATE:MultiJet', 'BW:Jet']
    SingleBjetGroup = ['RATE:SingleBJet', 'BW:BJet']
    #MultiBjetGroup = ['RATE:MultiBJet', 'BW:BJet']
    SingleTauGroup = ['RATE:SingleTau', 'BW:Tau']
    #MultiTauGroup = ['RATE:MultiTau', 'BW:Tau']
    #BphysicsGroup = ['RATE:Bphysics', 'BW:Bphysics']
    MinBiasGroup = ['RATE:MinBias', 'BW:MinBias']
    EgammaStreamersGroup = ['RATE:SeededStreamers', 'BW:Egamma']

    TriggerFlags.Slices_all_setOff()

    TriggerFlags.TestSlice.signatures = []

    TriggerFlags.MuonSlice.signatures = [
        #ART-19985
        ChainProp(name='HLT_mu6_idperf_L1MU6', groups=SingleMuonGroup),
        ChainProp(name='HLT_mu24_idperf_L1MU20', groups=SingleMuonGroup),

        #ATR-20049

        ChainProp(name='HLT_mu6fast_L1MU6', groups=SingleMuonGroup),
        ChainProp(name='HLT_mu6Comb_L1MU6', groups=SingleMuonGroup), 
        ChainProp(name='HLT_mu6_L1MU6',     groups=SingleMuonGroup),

        ChainProp(name='HLT_mu20_ivar_L1MU6',      groups=SingleMuonGroup),
        ChainProp(name='HLT_mu6_ivarmedium_L1MU6', groups=SingleMuonGroup),
        ChainProp(name='HLT_mu6_mu6noL1_L1MU6', l1SeedThresholds=['MU6',''], groups=MultiMuonGroup),
        ChainProp(name='HLT_mu6_msonly_L1MU6',     groups=SingleMuonGroup),

        ChainProp(name='HLT_2mu6_10invm70_L1MU6', groups=SingleMuonGroup),

        # ATR-20049
        ChainProp(name='HLT_mu6_mu4_L12MU4',  l1SeedThresholds=['MU4']*2, groups=MultiMuonGroup),

     ]

    TriggerFlags.EgammaSlice.signatures = [
        # ElectronChains----------
        #ChainProp(name='HLT_e3_etcut1step_L1EM3', groups=SingleElectronGroup),
        ChainProp(name='HLT_e3_etcut_L1EM3', groups=SingleElectronGroup),
        ChainProp(name='HLT_e5_etcut_L1EM3', groups=SingleElectronGroup),
        ChainProp(name='HLT_e7_etcut_L1EM3', stream=[PhysicsStream, 'express'], groups=SingleElectronGroup),
        # enabling only one step
        #ChainProp(name='HLT_2e3_etcut1step_L12EM3', stream=[PhysicsStream], groups=MultiElectronGroup),

        # PhotonChains------------
        ChainProp(name='HLT_g5_etcut_L1EM3', groups=SinglePhotonGroup),  

        # ATR-19360
        ChainProp(name='HLT_g5_etcut_LArPEB_L1EM3',stream=['LArCells'], groups=SinglePhotonGroup),
    ]

    TriggerFlags.METSlice.signatures = [
        ChainProp(name='HLT_xe30_cell_L1XE10', groups=SingleMETGroup),
        ChainProp(name='HLT_xe30_mht_L1XE10', groups=SingleMETGroup),
        ChainProp(name='HLT_xe30_tcpufit_L1XE10', groups=SingleMETGroup),

        # MultiMET Chain
        ChainProp(name='HLT_xe30_cell_xe30_tcpufit_L1XE10',l1SeedThresholds=['XE10']*2, groups=MultiMETGroup), #must be FS seeded
    ]

    TriggerFlags.JetSlice.signatures = [
        ChainProp(name='HLT_j85_L1J20', groups=SingleJetGroup),
        ChainProp(name='HLT_j45_L1J15', groups=SingleJetGroup),
        ChainProp(name='HLT_j420_L1J20', groups=SingleJetGroup),

        ChainProp(name='HLT_j260_320eta490_L1J20', groups=SingleJetGroup),

        ChainProp(name='HLT_j460_a10_lcw_subjes_L1J20', groups=SingleJetGroup),
        ChainProp(name='HLT_j460_a10r_L1J20', groups=SingleJetGroup),
        
        ChainProp(name='HLT_3j200_L1J20', groups=MultiJetGroup),
        ChainProp(name='HLT_j0_vbenfSEP30etSEP34mass35SEP50fbet_L1J20', groups=SingleJetGroup),

    ]

    TriggerFlags.BjetSlice.signatures = [
        #ATR-20049
        ChainProp(name="HLT_j35_gsc45_bmv2c1070_split_L1J20", groups=SingleBjetGroup),
        #ChainProp(name="HLT_j35_gsc45_bmv2c1070_L1J20", groups=SingleBjetGroup),
    ] 

    TriggerFlags.TauSlice.signatures = [
        # ATR-19985
        #ChainProp(name="HLT_tau25_idperf_tracktwo_L1TAU12IM", groups=SingleTauGroup),
        #ChainProp(name="HLT_tau25_idperf_tracktwoMVA_L1TAU12IM", groups=SingleTauGroup),
        #ATR-20049
        #ChainProp(name="HLT_tau0_perf_ptonly_L1TAU12", groups=SingleTauGroup),
        ChainProp(name="HLT_tau25_medium1_tracktwo_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau35_mediumRNN_tracktwoMVA_L1TAU12IM", groups=SingleTauGroup),

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
        ChainProp(name='HLT_noalg_L1RD0_EMPTY',  l1SeedThresholds=[''], stream=[PhysicsStream, 'BeamSpot'], groups=MinBiasGroup),  # FIXME: BeamSpot stream added just for testing, to be removed
        ChainProp(name='HLT_noalg_L1RD0_FILLED', l1SeedThresholds=[''], stream=[PhysicsStream, 'BeamSpot'], groups=MinBiasGroup),  # FIXME: BeamSpot stream added just for testing, to be removed
        ChainProp(name='HLT_noalg_L1EM3',        l1SeedThresholds=[''], stream=[PhysicsStream], groups=EgammaStreamersGroup),
    ]
    TriggerFlags.MonitorSlice.signatures   = [
        ChainProp(name='HLT_costmonitor_L1TE5',        l1SeedThresholds=[''], stream=['CostMonitoring'], groups=['RATE:Monitoring','BW:Other']),
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
    

Prescales = mc_menu.Prescales
