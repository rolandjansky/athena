# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

#------------------------------------------------------------------------#
#------------------------------------------------------------------------#
def setupMenu():

    from TriggerJobOpts.TriggerFlags          import TriggerFlags
    from AthenaCommon.Logging                 import logging
    log = logging.getLogger( 'TriggerMenuMT.HLTMenuConfig.Menu.LS2_emu_v1.py' )


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

    TriggerFlags.Slices_all_setOff()

    TriggerFlags.TestSlice.signatures = []

    TriggerFlags.MuonSlice.signatures = [
        #['HLT_mu20_L1MU10',   [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon']],
    ]
    TriggerFlags.EgammaSlice.signatures = [
        #['HLT_e7_etcut_L1EM3',      ['L1_EM3', ['EM3']],  [PhysicsStream], ['RATE:SingleElectron', 'BW:Electron']],
        
    ]
    TriggerFlags.CombinedSlice.signatures = [
        #['e8_mu8_L1EM6_MU6',	   [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon']],
    ]
    TriggerFlags.JetSlice.signatures = [ ]
    TriggerFlags.BjetSlice.signatures = [] 
    TriggerFlags.METSlice.signatures = []
    TriggerFlags.TauSlice.signatures = []
    TriggerFlags.BphysicsSlice.signatures = [ ]
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

    L1Prescales_cosmics  = {}
    HLTPrescales_cosmics = {}
    chain_list=[]

