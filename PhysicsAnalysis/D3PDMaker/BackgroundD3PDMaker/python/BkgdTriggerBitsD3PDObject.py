# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
## @file BackgroundD3PDMaker/python/BkgdTriggerBitsD3PDObject.py
## @brief D3PD object for background stream trigger bits 
## @author Mark Tibbetts
## @date May 2012
##


from TriggerD3PDMaker.defineTriggerBits import defineTriggerBits
from D3PDMakerCoreComps.D3PDObject import make_Void_D3PDObject


BkgdTriggerBitsD3PDObject = make_Void_D3PDObject(  "", "BkgdTriggerBitsD3PDObject",
                                                   default_name = 'BkgdTriggerBitsFiller' )


# defineTriggerBits will handle the duplicates correctly

defineTriggerBits( BkgdTriggerBitsD3PDObject, 0, ["EF_Background.*",
                                                  "EF_mbSpTrk_unpaired_iso","EF_mbSpBg_unpaired_iso"])

defineTriggerBits( BkgdTriggerBitsD3PDObject, 0, ["L2_Background.*",
                                                  "L2_mbSpTrk_unpaired_iso","L2_mbSpBg_unpaired_iso"])

defineTriggerBits( BkgdTriggerBitsD3PDObject, 0, ["L1_RD0_UNPAIRED_ISO","L1_RD0_EMPTY",
                                                  "L1_BCM_AC_CA_BGRP0","L1_BCM_AC_CA_UNPAIRED_ISO",
                                                  "L1_BCM_Wide_EMPTY",
                                                  "L1_BCM_Wide_UNPAIRED_ISO","L1_BCM_Wide_UNPAIRED_NONISO",
                                                  "L1_EM3_UNPAIRED_ISO","L1_FJ30_UNPAIRED_ISO",
                                                  "L1_J10_UNPAIRED_ISO","L1_J10_UNPAIRED_NONISO",
                                                  "L1_LUCID_A_C_EMPTY",
                                                  "L1_LUCID_A_C_UNPAIRED_ISO","L1_LUCID_A_C_UNPAIRED_NONISO",
                                                  "L1_LUCID_COMM_EMPTY","L1_LUCID_COMM_UNPAIRED_ISO",
                                                  "L1_LUCID_EMPTY","L1_LUCID_UNPAIRED_ISO",
                                                  "L1_MU4_UNPAIRED_ISO"
                                                  ])


def getBkgdTriggerBitsD3PDObject(triggerList):

    defineTriggerBits(BkgdTriggerBitsD3PDObject,0,triggerList);

    return BkgdTriggerBitsD3PDObject(0)
