# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

#------------------------------------------------------------------------#
# Physics_pp_run3_v1.py menu for the long shutdown development
#------------------------------------------------------------------------#

# This defines the input format of the chain and it's properties with the defaults set
# always required are: name, stream and groups
#['name', 'L1chainParts'=[], 'stream', 'groups', 'merging'=[], 'topoStartFrom'=False],
from TriggerMenuMT.HLTMenuConfig.Menu.ChainDefInMenu import ChainProp

PhysicsStream="Main"
SingleMuonGroup = ['RATE:SingleMuon', 'BW:Muon']
MultiMuonGroup = ['RATE:MultiMuon', 'BW:Muon']
SingleElectronGroup = ['RATE:SingleElectron', 'BW:Electron']
MultiElectronGroup = ['RATE:MultiElectron', 'BW:Electron']
SinglePhotonGroup = ['RATE:SinglePhoton', 'BW:Photon']
MultiPhotonGroup = ['RATE:MultiPhoton', 'BW:Photon']
SingleMETGroup = ['RATE:MET', 'BW:MET']
MultiMETGroup = ['RATE:MultiMET', 'BW:MultiMET']
SingleJetGroup = ['RATE:SingleJet', 'BW:Jet']
MultiJetGroup = ['RATE:MultiJet', 'BW:Jet']
SingleBjetGroup = ['RATE:SingleBJet', 'BW:BJet']
#MultiBjetGroup = ['RATE:MultiBJet', 'BW:BJet']
SingleTauGroup = ['RATE:SingleTau', 'BW:Tau']
#MultiTauGroup = ['RATE:MultiTau', 'BW:Tau']
BphysicsGroup = ['RATE:Bphysics', 'BW:Bphysics']
MinBiasGroup = ['RATE:MinBias', 'BW:MinBias']
EgammaStreamersGroup = ['RATE:SeededStreamers', 'BW:Egamma']

def setupMenu():

    from TriggerJobOpts.TriggerFlags          import TriggerFlags
    from AthenaCommon.Logging                 import logging
    log = logging.getLogger( 'TriggerMenuMT.HLTMenuConfig.Menu.Physics_pp_run3_v1.py' )
    log.info('setupMenu ...')


    TriggerFlags.Slices_all_setOff()

    TriggerFlags.TestSlice.signatures = []

    TriggerFlags.MuonSlice.signatures = [
        #ATR-20049
        ChainProp(name='HLT_mu60_0eta105_msonly_L1MU20', groups=SingleMuonGroup),
        ChainProp(name='HLT_mu80_msonly_3layersEC_L1MU20', groups=SingleMuonGroup),
        ChainProp(name='HLT_3mu6_L13MU6', l1SeedThresholds=['MU6'],   groups=MultiMuonGroup),
        ChainProp(name='HLT_3mu6_msonly_L13MU6', l1SeedThresholds=['MU6'],   groups=MultiMuonGroup),
        ChainProp(name='HLT_4mu4_L14MU4', l1SeedThresholds=['MU4'],   groups=MultiMuonGroup),
        ChainProp(name='HLT_mu26_ivarmedium_L1MU20', groups=SingleMuonGroup),
        ChainProp(name='HLT_mu50_L1MU20', groups=SingleMuonGroup),
        ChainProp(name='HLT_2mu14_L12MU10', groups=MultiMuonGroup),
        ChainProp(name='HLT_2mu6Comb_L12MU6', l1SeedThresholds=['MU6'],   groups=MultiMuonGroup),
        ChainProp(name='HLT_2mu6_L12MU6',     l1SeedThresholds=['MU6'],   groups=MultiMuonGroup),
     ]

    TriggerFlags.EgammaSlice.signatures = [
        # ElectronChains----------
        ChainProp(name='HLT_e26_etcut_L1EM22VHI', groups=SingleElectronGroup),
        ChainProp(name='HLT_2e17_etcut_L12EM15VH', stream=[PhysicsStream], groups=MultiElectronGroup),
        ChainProp(name='HLT_g140_etcut_L1EM24VHI', groups=SinglePhotonGroup),  
        ChainProp(name='HLT_2g35_etcut_L12EM20VH', groups=MultiPhotonGroup),  
    ]

    TriggerFlags.METSlice.signatures = [
        ChainProp(name='HLT_xe65_cell_L1XE50', groups=SingleMETGroup),
        # MultiMET Chain
    ]

    TriggerFlags.JetSlice.signatures = [
        # ATR-20049
        ChainProp(name='HLT_j420_L1J100', groups=SingleJetGroup),
        ChainProp(name='HLT_j260_320eta490_L1J75_31ETA49', groups=SingleJetGroup),
        ChainProp(name='HLT_j460_a10r_L1J100',  groups=SingleJetGroup),
        ChainProp(name='HLT_j460_a10_lcw_subjes_L1J100', groups=SingleJetGroup),
        ChainProp(name='HLT_j460_a10t_lcw_jes_L1J100', groups=SingleJetGroup),
        ChainProp(name='HLT_5j70_0eta240_L14J20', groups=MultiJetGroup), # this chain is supposed to be seeded off L1_4J15 in principle, needs CF fix
        ChainProp(name='HLT_3j200_L1J100', groups=MultiJetGroup),

        ChainProp(name='HLT_j80_j60_L1J15', l1SeedThresholds=['']*2, groups=MultiJetGroup),
        ChainProp(name='HLT_j80_0eta240_2j60_320eta490_j0_dijetSEP80j1etSEP0j1eta240SEP80j2etSEP0j2eta240SEP700djmass_L1J20', l1SeedThresholds=['']*3, groups=MultiJetGroup),
    ]

    TriggerFlags.BjetSlice.signatures = [
        #ATR-20049
        ChainProp(name="HLT_j175_gsc225_bmv2c1040_split_L1J100", groups=SingleBjetGroup),
        ChainProp(name="HLT_j225_gsc275_bmv2c1060_split_L1J100", groups=SingleBjetGroup),
        ChainProp(name="HLT_j225_gsc300_bmv2c1070_split_L1J100", groups=SingleBjetGroup),
        ChainProp(name="HLT_j225_gsc360_bmv2c1077_split_L1J100", groups=SingleBjetGroup),
    ] 

    TriggerFlags.TauSlice.signatures = [
        #ATR-20049
        ChainProp(name="HLT_tau160_mediumRNN_tracktwoMVA_L1TAU100", groups=SingleTauGroup),
    ]
    TriggerFlags.BphysicsSlice.signatures = [
        #ATR-20049
        ChainProp(name='HLT_2mu10_bJpsimumu_L12MU10',     groups=BphysicsGroup),
        ChainProp(name='HLT_2mu10_bUpsimumu_L12MU10',     groups=BphysicsGroup),
    ]
    TriggerFlags.CombinedSlice.signatures = [ 
        ChainProp(name='HLT_e3_etcut1step_mu6fast_L1EM8I_MU10', l1SeedThresholds=['EM8I', 'MU10'], stream=[PhysicsStream], groups=MultiElectronGroup),    #L1 item thresholds in wrong order (EM first, then MU)    
   ]
    TriggerFlags.HeavyIonSlice.signatures  = []
    TriggerFlags.BeamspotSlice.signatures  = [
        ChainProp(name='HLT_beamspot_allTE_trkfast_L1J15',  l1SeedThresholds=[''], stream=['BeamSpot'], groups=['RATE:BeamSpot',  'BW:BeamSpot']),
        #ChainProp(name='HLT_beamspot_activeTE_trkfast_L1J15',  l1SeedThresholds=[''], stream=['BeamSpot'], groups=['RATE:BeamSpot',  'BW:BeamSpot']),
        #ChainProp(name='HLT_beamspot_trkFS_trkfast_L1J15',  l1SeedThresholds=[''], stream=['BeamSpot'], groups=['RATE:BeamSpot',  'BW:BeamSpot']),
    ]
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

