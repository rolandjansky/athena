# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import TriggerMenu.menu.Physics_HI_v5 as physics_menu

from TriggerJobOpts.TriggerFlags          import TriggerFlags
from copy                                 import deepcopy



from AthenaCommon.Logging import logging
log = logging.getLogger( 'MC_HI_v5.py' )


#---------------------------------------------------------------------
#---------------------------------------------------------------------
def setupMenu():

    physics_menu.setupMenu()


    # ---------------------------------------------------------------------------------------
    # INPUT FORMAT FOR CHAINS:
    # ['chainName',  'L1itemforchain', [L1 items for chainParts], [stream], [groups], EBstep], OPTIONAL: [mergingStrategy, offset,[merginOrder] ]], topoStartsFrom = False
    # ----------------------------------------------------------------------------------------

    TriggerFlags.TestSlice.signatures = TriggerFlags.TestSlice.signatures() + [
        ]

    TriggerFlags.AFPSlice.signatures = TriggerFlags.AFPSlice.signatures() + [
	]

    TriggerFlags.MuonSlice.signatures = TriggerFlags.MuonSlice.signatures() + [
        ]
    
    TriggerFlags.JetSlice.signatures = TriggerFlags.JetSlice.signatures() + [
        ]
    
    TriggerFlags.BjetSlice.signatures = TriggerFlags.BjetSlice.signatures() + [
        ]
    
    TriggerFlags.METSlice.signatures = TriggerFlags.METSlice.signatures() + [ 
        ]
    
    TriggerFlags.TauSlice.signatures = TriggerFlags.TauSlice.signatures() + [
        ]
    
    TriggerFlags.EgammaSlice.signatures = TriggerFlags.EgammaSlice.signatures() + [
        ]
    
    TriggerFlags.BphysicsSlice.signatures = TriggerFlags.BphysicsSlice.signatures() + [
        ]
    
    TriggerFlags.CombinedSlice.signatures = TriggerFlags.CombinedSlice.signatures() + [
        ]
    
    TriggerFlags.MinBiasSlice.signatures = TriggerFlags.MinBiasSlice.signatures() + [
        ]
    
    #    TriggerFlags.BeamspotSlice.signatures = TriggerFlags.BeamspotSlice.signatures() + [                                                                 
    #        ]
    
    TriggerFlags.CalibSlice.signatures   = TriggerFlags.CalibSlice.signatures() + []
    TriggerFlags.CosmicSlice.signatures  = TriggerFlags.CosmicSlice.signatures() + []
    TriggerFlags.StreamingSlice.signatures = TriggerFlags.StreamingSlice.signatures() + []
    TriggerFlags.MonitorSlice.signatures = TriggerFlags.MonitorSlice.signatures() + []
    #TriggerFlags.GenericSlice.signatures = TriggerFlags.GenericSlice.signatures() + []
    
    

######################################################
Prescales = physics_menu.Prescales

# ######################################################
# # DEFAULT MC prescales
# ######################################################
Prescales.L1Prescales = dict([(ctpid,1) for ctpid in Prescales.L1Prescales])  # setting all L1 prescales to 1
ps_exclusion_list=[
    'mb_sptrk_L1ZDC_XOR_TE5_VTE200',
    'mb_sptrk_L1ZDC_XOR_VTE200',
    'mb_sptrk_L1ZDC_XOR_VTE200_MBTS_1',
    'mb_perf_L1RD1_FILLED',
    'mb_sptrk_L1MBTS_1_1_VTE50',
    'timeburner',
    'mb_sp',
]    
chain_list=ps_exclusion_list
Prescales.HLTPrescales.update(dict(map(None,chain_list,len(chain_list)*[ [-1, 0, -1] ])))
# ######################################################

# ######################################################
# # No MC prescales
# ######################################################
# Prescales.L1Prescales_no_prescale  = deepcopy(Prescales.L1Prescales)
# Prescales.HLTPrescales_no_prescale = {}
# ######################################################

# ######################################################
# # LOOSE mc prescales
# ######################################################
# Prescales.L1Prescales_loose_mc_prescale  = deepcopy(Prescales.L1Prescales)
# Prescales.HLTPrescales_loose_mc_prescale = deepcopy(Prescales.HLTPrescales)
# Prescales.HLTPrescales_loose_mc_prescale.update({})
# ######################################################

# ######################################################
# # TIGHT mc prescales
# ######################################################
Prescales.L1Prescales_tight_mc_prescale  = deepcopy(Prescales.L1Prescales)
Prescales.HLTPrescales_tight_mc_prescale = deepcopy(Prescales.HLTPrescales)
Prescales.HLTPrescales_tight_mc_prescale.update({
        'mb_sp'                           : [    -1,    0,   -1],
        #'2e10_loose_ion'                  : [    -1,    0,   -1],
        #'e10_etcut_ion'                   : [    -1,    0,   -1],
        #'e10_etcut'                       : [    -1,    0,   -1],
        #'e12_etcut'                       : [    -1,    0,   -1],
        #'e12_etcut_ion'                   : [    -1,    0,   -1],
        #'e12_etcut_ion_L1EM10'            : [    -1,    0,   -1],
        })

# ######################################################
# Prescales.L1Prescales_upgrade_mc_prescale  = deepcopy(Prescales.L1Prescales)
# Prescales.HLTPrescales_upgrade_mc_prescale = deepcopy(Prescales.HLTPrescales_tight_mc_prescale)
# # Note: "upgrade" prescales are set with regular expressions at the end
# #       of the setupMenu() function above 
# ######################################################
