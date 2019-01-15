# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

#------------------------------------------------------------------------#
#------------------------------------------------------------------------#
def setupMenu():

    from TriggerJobOpts.TriggerFlags          import TriggerFlags
    from AthenaCommon.Logging                 import logging
    log = logging.getLogger( 'TriggerMenu.menu.Physics_pp_v8_lowmu.py' )

    from TriggerMenu.TriggerConfigLVL1 import TriggerConfigLVL1 as tcl1
    if tcl1.current:
        log.info("L1 items: %s " % tcl1.current.menu.items.itemNames())
    else:
        log.info("ERROR L1 menu has not yet been defined")

    PhysicsStream="Main"

    # stream, BW and RATE tags for Bphysics items that appear in Muon and Bphysics slice.signatures
    BPhysicsStream     = "BphysLS"
    #BMultiMuonStream   = "Main"  
    RATE_BphysTag      = 'RATE:Bphysics'
    RATE_BMultiMuonTag = 'RATE:BMultiMuon'  # can become RATE:BMultiMuon' with one letter change 
    BW_BphysTag        = 'BW:Bphys'
    #RATE_DYTag         = 'RATE:Muon'   
    #BW_DYTag           = 'BW:Muon'   

    #---------------------------------------------------------------------
    # INPUT FORMAT FOR CHAINS:
    # ['chainName',  'L1itemforchain', [L1 items for chainParts], [stream], [groups], EBstep], OPTIONAL: [mergingStrategy, offset,[merginOrder] ]], topoStartsFrom = False
    #---------------------------------------------------------------------

    #---------------------------------------------------------------------
    # if it's needed to temporary remove almost all the chains from the menu
    # be aware that it is necessary to leave at least one chain in the muon slice
    # otherwise athenaHLT will seg-fault 
    #---------------------------------------------------------------------

    TriggerFlags.Slices_all_setOff()

    TriggerFlags.TestSlice.signatures = []

    TriggerFlags.AFPSlice.signatures = []

    TriggerFlags.MuonSlice.signatures = [
        #Low-mu run triggers
        ['mu14',                   'L1_MU10',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['2mu4',                 'L1_2MU4', [], [PhysicsStream,'express'], [RATE_BMultiMuonTag, BW_BphysTag], -1],
			 ]

    TriggerFlags.JetSlice.signatures = [   
        #Low-mu run triggers
        ['j0_perf_ds1_L1J20',      'L1_J20',  [], ['DataScouting_05_Jets'], ['RATE:Jets_DS', 'BW:Jet'], -1],
        ['j175',                        'L1_J50',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j110_320eta490',              'L1_J30.31ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j260_a10_lcw_subjes_L1J75',       'L1_J75', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
			 ]

    TriggerFlags.BjetSlice.signatures = []

    TriggerFlags.METSlice.signatures = [
        ['xe35', 'L1_XE35',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe35_tc_lcw', 'L1_XE35',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
			 ]
    
    TriggerFlags.TauSlice.signatures = []

    TriggerFlags.EgammaSlice.signatures = [
        #Low-mu run triggers
        ['e15_lhloose_nod0_L1EM12',  'L1_EM12', [], [PhysicsStream], ['RATE:SingleElectron','BW:Egamma'], -1],
        ['g120_loose',                    'L1_EM24VHI', [], [PhysicsStream], ['RATE:SinglePhoton','BW:Egamma'],-1], 
        ['g35_loose_g25_loose',                      'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1],
        	 ]

    TriggerFlags.BphysicsSlice.signatures = [
        ['2mu4_bDimu',                    'L1_2MU4', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1], 
        ['2mu4_bDimu_novtx_noos',         'L1_2MU4', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],  
        ['2mu6_bJpsimumu',                'L1_2MU6', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
			 ]

    TriggerFlags.CombinedSlice.signatures = [
        #ATR-17860 L1Topo low Et di-photon triggers
        ['2g3_loose_L1DPHI-2EM3_VTE10', 'L1_DPHI-2EM3_VTE10', ['L1_2EM3'], [PhysicsStream],['RATE:SinglePhoton','BW:Egamma'],-1],
        #ATR-17836 W T&P for low-mu
        ['e15_etcut_trkcut_xe30noL1', 'L1_EM12', ['L1_EM12',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1, ['serial',-1,["e15_etcut_trkcut","xe30noL1"]]],
			 ]

    TriggerFlags.HeavyIonSlice.signatures = []
    TriggerFlags.BeamspotSlice.signatures = []   
    TriggerFlags.MinBiasSlice.signatures = []
    TriggerFlags.CalibSlice.signatures   = []
    TriggerFlags.CosmicSlice.signatures  = []
    TriggerFlags.StreamingSlice.signatures = []
    TriggerFlags.MonitorSlice.signatures = []
    TriggerFlags.EnhancedBiasSlice.signatures = []


#    ###############################################################
#    #################################################################
#    signatureList=[]
#    for prop in dir(TriggerFlags):
#        if prop[-5:]=='Slice':
#            sliceName=prop
#            slice=getattr(TriggerFlags,sliceName)
#            if slice.signatures():
#                signatureList.extend(slice.signatures())
#            else:
#                log.debug('SKIPPING '+str(sliceName))
#    mySigList=[]
#    for allInfo in signatureList:
#        mySigList.append(allInfo[0])
#    mydict={}
#    for chain in mySigList:
#        mydict[chain]=[-1,0,0]
#    mydict.update(Prescales.HLTPrescales_cosmics)
#    mydict.update(Prescales.HLTPrescales_unprescaled_only_physics_prescale)
#    mydict.update(Prescales.L1Prescales_unprescaled_only_physics_prescale)
#    from copy import deepcopy
#    Prescales.HLTPrescales_cosmics = deepcopy(mydict)
#    Prescales.L1Prescales_unprescaled_only_physics_prescale  = deepcopy(mydict)
#    Prescales.HLTPrescales_unprescaled_only_physics_prescale = deepcopy(mydict)


class Prescales:
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

