# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#------------------------------------------------------------------------#
#------------------------------------------------------------------------#
from TriggerMenuMT.HLTMenuConfig.Menu.ChainDefInMenu import ChainProp


def setupMenu():

    from TriggerJobOpts.TriggerFlags          import TriggerFlags
    from AthenaCommon.Logging                 import logging

    # IMPORTANT: Needs to be commented in again!
    #PhysicsStream = "Main"

    #---------------------------------------------------------------------
    # INPUT FORMAT FOR CHAINS:
    # ['chainName', [L1 thresholds for chainParts], [stream], [groups]], 
    # OPTIONAL: [mergingStrategy, offset,[merginOrder] ]], topoStartsFrom = False
    #---------------------------------------------------------------------
    #---------------------------------------------------------------------
    # if it's needed to temporary remove almost all the chains from the menu
    # be aware that it is necessary to leave at least one chain in the muon slice
    # otherwise athenaHLT will seg-fault 
    #---------------------------------------------------------------------

    log = logging.getLogger( __name__ )
    log.info('Executing menu....')

    TriggerFlags.TestSlice.signatures = [
        # muons
        ChainProp(name='HLT_TestChain8_muv1step_L1MU6', stream=['Main'], groups=['RATE:Test','BW:Other'] ),
        ChainProp(name='HLT_TestChain8_muv1_L1MU10', stream=['Main'], groups=['RATE:Test','BW:Other'] ),
        ChainProp(name='HLT_TestChain20_muv1_L1MU10',stream=['Main'], groups=['RATE:Test','BW:Other'] ),
        ChainProp(name='HLT_TestChain10_muv2_L1MU10',stream=['Main'], groups=['RATE:Test','BW:Other'] ),
        ChainProp(name='HLT_TestChain6_muEmpty2_L1MU6',  stream=['Main'], groups=['RATE:Test','BW:Other'] ),

        # egamma
        ChainProp(name='HLT_TestChain5_ev1_L1EM3', stream=['Main'], groups=['RATE:Test','BW:Other'] ),
        ChainProp(name='HLT_TestChain8_ev1_L1EM5', stream=['Main'], groups=['RATE:Test','BW:Other'] ),
        ChainProp(name='HLT_TestChain5_ev2_L1EM7', stream=['Main'], groups=['RATE:Test','BW:Other'] ),
        ChainProp(name='HLT_TestChain5_ev3_L1EM7', stream=['Main'], groups=['RATE:Test','BW:Other'] ),
        ChainProp(name='HLT_TestChain5_gv1_L1EM7', stream=['Main'], groups=['RATE:Test','BW:Other'] ),

        # combined
        ChainProp(name='HLT_TestChain6_muv1_TestChain10_ev1_L1MU6_EM5', stream=['Main'], groups=['RATE:Test','BW:Other'] ), #serial         
        ChainProp(name='HLT_TestChain6_muv2_TestChain8_ev2_L1MU6_EM5',  stream=['Main'], groups=['RATE:Test','BW:Other'] ),
        ChainProp(name='HLT_TestChain5_ev1_TestChain8_ev1_L12EM3',      stream=['Main'], groups=['RATE:Test','BW:Other']),
#        ChainProp(name='HLT_TestChain5_ev1_TestChain8_ev1_2TestChain6_muv1_L1EM3_L1EM5_L12MU6', stream=['Main'], groups=['RATE:Test','BW:Other']   ),
        ChainProp(name='HLT_2TestChain6_muv1_L12MU6',                   stream=['Main'], groups=['RATE:Test','BW:Other']   ),
        ChainProp(name='HLT_3TestChain6_muv1_L12MU6',                   stream=['Main'], groups=['RATE:Test','BW:Other']   ),
        ChainProp(name='HLT_TestChain6_muv1_TestChain10_muv1_L12MU6',   stream=['Main'], groups=['RATE:Test','BW:Other']  ),        
        ChainProp(name='HLT_2TestChain6_muEmpty1_L12MU6',               stream=['Main'], groups=['RATE:Test','BW:Other']), #may differ from manual
        ChainProp(name='HLT_TestChain6_muv1_TestChain5_ev1dr_L1MU6_EM5',   stream=['Main'], groups=['RATE:Test','BW:Other'] ), 
        ChainProp(name='HLT_2TestChain4_muv1dr_L12MU6', stream=['Main'], groups=['RATE:Test','BW:Other'] ),

        # FSNOSEED not implemented in emulation
        ChainProp(name='HLT_TestChain10_muEmpty1_TestChain6_muEmpty1_L12MU6',  l1SeedThresholds=['MU6','MU6'],  stream=['Main'], groups=['RATE:Test','BW:Other']) 
        
        ]


    
    TriggerFlags.EgammaSlice.signatures =  [
    # ElectronChains----------
#        ChainProp(name='HLT_e3_etcut_L1EM3', stream=['Main'], groups=['RATE:Test','BW:Other'])
        ]


    TriggerFlags.MuonSlice.signatures = []
    TriggerFlags.CombinedSlice.signatures = []
    TriggerFlags.JetSlice.signatures = []
    TriggerFlags.BjetSlice.signatures = [] 
    TriggerFlags.METSlice.signatures = []
    TriggerFlags.TauSlice.signatures = []
    TriggerFlags.BphysicsSlice.signatures  = []
    TriggerFlags.HeavyIonSlice.signatures  = []
    TriggerFlags.BeamspotSlice.signatures  = []   
    TriggerFlags.MinBiasSlice.signatures   = []    
    TriggerFlags.CalibSlice.signatures     = []
    TriggerFlags.CosmicSlice.signatures    = []
    TriggerFlags.StreamingSlice.signatures = [] 
    TriggerFlags.MonitorSlice.signatures   = []

    # Random Seeded EB chains which select at the HLT based on L1 TBP bits
    TriggerFlags.EnhancedBiasSlice.signatures = [ ]


