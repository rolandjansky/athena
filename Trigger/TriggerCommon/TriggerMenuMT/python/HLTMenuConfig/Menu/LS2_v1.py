# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

#------------------------------------------------------------------------#
# LS2_v1.py menu for the long shutdown development
#------------------------------------------------------------------------#

# This defines the input format of the chain and it's properties with the defaults set
# always required are: name, stream and groups
#['name', 'L1chainParts'=[], 'stream', 'groups', 'merging'=[], 'topoStartFrom'=False],
from TriggerMenuMT.HLTMenuConfig.Menu.ChainDefInMenu import ChainProp

def setupMenu():

    from TriggerJobOpts.TriggerFlags          import TriggerFlags
    from AthenaCommon.Logging                 import logging
    log = logging.getLogger( 'TriggerMenuMT.HLTMenuConfig.Menu.LS2_v1.py' )

    PhysicsStream="Main"
    SingleMuonGroup = ['RATE:SingleMuon', 'BW:Muon']
    MultiMuonGroup = ['RATE:MultiMuon', 'BW:Muon']
    SingleElectronGroup = ['RATE:SingleElectron', 'BW:Electron']
    MultiElectronGroup = ['RATE:MultiElectron', 'BW:Electron']
    SinglePhotonGroup = ['RATE:SinglePhoton', 'BW:Photon']
    #MultiPhotonGroup = ['RATE:MultiPhoton', 'BW:Photon']
    SingleMETGroup = ['RATE:MET', 'BW:MET']
    #MultiMETGroup = ['RATE:MultiMET', 'BW:MultiMET']
    SingleJetGroup = ['RATE:SingleJet', 'BW:Jet']
    MultiJetGroup = ['RATE:MultiJet', 'BW:Jet']
    SingleBjetGroup = ['RATE:SingleBJet', 'BW:BJet']
    #MultiBjetGroup = ['RATE:MultiBJet', 'BW:BJet']
    SingleTauGroup = ['RATE:SingleTau', 'BW:Tau']
    #MultiTauGroup = ['RATE:MultiTau', 'BW:Tau']
    BphysicsGroup = ['RATE:Bphysics', 'BW:Bphysics']

    TriggerFlags.Slices_all_setOff()

    TriggerFlags.TestSlice.signatures = []

    TriggerFlags.MuonSlice.signatures = [
        ChainProp(name='HLT_mu6fast_L1MU6', groups=SingleMuonGroup),
        ChainProp(name='HLT_mu6Comb_L1MU6', groups=SingleMuonGroup), 
        ChainProp(name='HLT_mu6_L1MU6', groups=SingleMuonGroup),

        ChainProp(name='HLT_mu20_ivar_L1MU6', groups=SingleMuonGroup),
        ChainProp(name='HLT_mu6_ivarmedium_L1MU6', groups=SingleMuonGroup),
        ChainProp(name='HLT_mu6noL1_L1MU6', groups=SingleMuonGroup),

        ChainProp(name='HLT_mu6_msonly_L1MU6', groups=SingleMuonGroup),

        ChainProp(name='HLT_2mu6Comb_L12MU6', groups=MultiMuonGroup),
        ChainProp(name='HLT_2mu6_L12MU6', groups=MultiMuonGroup),
        ChainProp(name='HLT_mu6_mu4_L12MU4', groups=MultiMuonGroup),

     ]

    TriggerFlags.EgammaSlice.signatures = [
        # ElectronChains----------
        ChainProp(name='HLT_e3_etcut1step_L1EM3', groups=SingleElectronGroup),
        ChainProp(name='HLT_e3_etcut_L1EM3', groups=SingleElectronGroup),
        ChainProp(name='HLT_e5_etcut_L1EM3', groups=SingleElectronGroup),
        ChainProp(name='HLT_e7_etcut_L1EM3', stream=[PhysicsStream, 'express'], groups=SingleElectronGroup),
        # currently disabled, seems to be a problem with the precision calo step
        #ChainProp(name='HLT_2e3_etcut_L12EM3', stream=[PhysicsStream], groups=MultiElectronGroup),
        #ChainProp(name='HLT_e3_etcut_e7_etcut_L12EM3', stream=[PhysicsStream], groups=MultiElectronGroup),
        # enabling only one step
        #ChainProp(name='HLT_2e3_etcut1step_L12EM3', stream=[PhysicsStream], groups=MultiElectronGroup),
        #ChainProp(name='HLT_e3_etcut1step_e7_etcut1step_L12EM3', stream=[PhysicsStream], groups=MultiElectronGroup),

        #chain with asymmetric number of steps not yet working
        #ChainProp(name='HLT_e3_etcut1step_e7_etcut_L12EM3', stream=[PhysicsStream], groups=MultiElectronGroup), 

        # PhotonChains------------
        ChainProp(name='HLT_g5_etcut_L1EM3', groups=SinglePhotonGroup),  
    ]

    TriggerFlags.METSlice.signatures = [
        ChainProp(name='HLT_xe30_cell_L1XE10', groups=SingleMETGroup),
        ChainProp(name='HLT_xe65_cell_L1XE50', groups=SingleMETGroup),
        #ChainProp(name='HLT_xe30_mht_L1XE10', groups=SingleMETGroup),
        ChainProp(name='HLT_xe30_tcpufit_L1XE10', groups=SingleMETGroup),

        # MultiMET Chain
        #ChainProp(name='HLT_xe30_cell_xe30_tcpufit_L1XE10', groups=MultiMETGroup),
    ]

    TriggerFlags.JetSlice.signatures = [
        ChainProp(name='HLT_j85_L1J20', groups=SingleJetGroup),
        ChainProp(name='HLT_j45_L1J20', groups=SingleJetGroup),
        ChainProp(name='HLT_j420_L1J20', groups=SingleJetGroup),

        #ChainProp(name='HLT_j225_gsc420_boffperf_split_L1J20', groups=SingleJetGroup),
        ChainProp(name='HLT_j260_320eta490_L1J20', groups=SingleJetGroup),

        ChainProp(name='HLT_j460_a10_lcw_subjes_L1J20', groups=SingleJetGroup),
        ChainProp(name='HLT_j460_a10r_L1J20', groups=SingleJetGroup),
        ChainProp(name='HLT_j420_a10t_lcw_jes_30smcINF_L1J20', groups=SingleJetGroup),
        ChainProp(name='HLT_2j330_a10t_lcw_jes_30smcINF_L1J20', groups=SingleJetGroup),
        
        ChainProp(name='HLT_3j200_L1J20', groups=MultiJetGroup),
        ChainProp(name='HLT_j0_vbenfSEP30etSEP34mass35SEP50fbet_L1J20', groups=SingleJetGroup),

        ChainProp(name='HLT_5j70_0eta240_L1J20', groups=MultiJetGroup), # this chain is supposed to be seeded off L1_4J15 in principle, needs CF fix

    ]
    TriggerFlags.BjetSlice.signatures = [
        ChainProp(name="HLT_j35_gsc45_boffperf_split_L1J20", groups=SingleBjetGroup),
        ChainProp(name="HLT_j35_gsc45_bmv2c1070_split_L1J20", groups=SingleBjetGroup),
        #ChainProp(name="HLT_j35_gsc45_bmv2c1070_L1J20", groups=SingleBjetGroup),
    ] 

    TriggerFlags.TauSlice.signatures = [
        ChainProp(name="HLT_tau0_perf_ptonly_L1TAU12", groups=SingleTauGroup),
        ChainProp(name="HLT_tau25_medium1_tracktwo_L1TAU12IM", groups=SingleTauGroup),
        ChainProp(name="HLT_tau35_mediumRNN_tracktwoMVA_L1TAU12IM", groups=SingleTauGroup)
    ]
    TriggerFlags.BphysicsSlice.signatures = [
        ChainProp(name='HLT_2mu4_bDimu_L12MU4',     groups=BphysicsGroup),
        ChainProp(name='HLT_2mu6_bJpsimumu_L12MU6', groups=BphysicsGroup),
        ChainProp(name='HLT_2mu4_bBmumu_L12MU4',    groups=BphysicsGroup),
        ChainProp(name='HLT_2mu4_bUpsimumu_L12MU4', groups=BphysicsGroup),
        ChainProp(name='HLT_2mu4_bJpsimumu_L12MU4', groups=BphysicsGroup)
    ]
    TriggerFlags.CombinedSlice.signatures = [ 
        ChainProp(name='HLT_e3_etcut1step_mu6fast_L1EM8I_MU10', l1SeedThresholds=['L1_EM8I', 'L1_MU10'], stream=[PhysicsStream], groups=MultiElectronGroup),    #L1 item thresholds in wrong order (EM first, then MU)    
        #ChainProp(name='HLT_mu8_e8_etcut_L1MU6_EM7', l1SeedThresholds=['L1_MU6', 'L1_EM7'], stream=[PhysicsStream], groups=MultiElectronGroup),    #L1 item thresholds in wrong order (EM first, then MU)    
        #ChainProp(name='HLT_e8_etcut1step_j85_L1EM3_J20', l1SeedThresholds=['L1_EM3', 'L1_J20'], stream=[PhysicsStream], groups=MultiElectronGroup),  
   ]
    TriggerFlags.HeavyIonSlice.signatures  = []
    TriggerFlags.BeamspotSlice.signatures  = []   
    TriggerFlags.MinBiasSlice.signatures   = []    
    TriggerFlags.CalibSlice.signatures     = []
    TriggerFlags.CosmicSlice.signatures    = []
    TriggerFlags.StreamingSlice.signatures = [] 
    TriggerFlags.MonitorSlice.signatures   = []

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
    

class Prescales(object):
    #   Item name             | Prescale
    #----------------------------------------------------------
    L1Prescales = {}

    #   Signature name   | [ HLTprescale, HLTpass-through, rerun]
    #   - Prescale values should be a positive integer (default=1)
    #   - If the current pass_through value is non-zero,
    #     the value given here will be used as pass_through rate
    #     Assuming that pass through chains are configured so
    #     in the slice files and won't change. Also prescale
    #     and pass_through will not be used together.
    #   - If only the first value is specified,
    #     the default value of pass-through (=0) will be used
    #----------------------------------------------------------
    HLTPrescales = {
        }

    L1Prescales_cosmics  = {}
    HLTPrescales_cosmics = {}
    chain_list=[]

