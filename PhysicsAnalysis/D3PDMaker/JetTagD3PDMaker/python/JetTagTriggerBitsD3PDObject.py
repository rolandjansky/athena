# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
## @file JetTagD3PDMaker/python/JetTagTriggerBitsD3PDObject.py
## @brief D3PD object for trigger bits needed by btagging
## @author Georges Aad
## @date Juin, 2011
##


from TriggerD3PDMaker.defineTriggerBits import defineTriggerBits
from D3PDMakerCoreComps.D3PDObject import make_Void_D3PDObject


JetTagTriggerBitsD3PDObject = \
      make_Void_D3PDObject(  "", "JetTagTriggerBitsD3PDObject",
                             default_name = 'JetTagTriggerBitsFiller' )
 

### add btag triggers
# already added if btag trig info is included
# but defineTriggerBits will handle the duplicates correctly

defineTriggerBits(JetTagTriggerBitsD3PDObject, 0, ["EF_b.*", "EF_2b.*", "EF_3b.*",
                                                   "EF_mu4_j10_a4tc_EFFS", "EF_mu4_L1J10_matched",
                                                   "EF_mu4_L1J20_matched",
                                                   "EF_mu4_L1J30_matched", "EF_mu4_L1J50_matched",
                                                   "EF_mu4_L1J75_matched", "EF_4j30_a4tc_EFFS",
                                                   "EF_mu4_j100_a4tc_EFFS_L1matched",
                                                   "EF_mu4_j10_a4_EFFS",
                                                   "EF_mu4_j10_a4tc_EFFS_matched"
                                                   ])

defineTriggerBits(JetTagTriggerBitsD3PDObject, 0, ["L2_b.*", "L2_2b.*", "L2_3b.*",
                                            "L2_mu4_j10_a4tc_EFFS", "L2_mu4_L1J10_matched",
                                            "L2_mu4_L1J20_matched",
                                            "L2_mu4_L1J30_matched", "L2_mu4_L1J50_matched",
                                            "L2_mu4_L1J75_matched", "L2_4j30_a4tc_EFFS",
                                            "L2_mu4_j100_a4tc_EFFS_L1matched"])


defineTriggerBits(JetTagTriggerBitsD3PDObject, 0, ["L1_MU4.*","L2_mu4T.*","EF_mu4.*"])
   

def getJetTagTriggerBitsD3PDObject(triggerList):

    defineTriggerBits(JetTagTriggerBitsD3PDObject,0,triggerList)

    return JetTagTriggerBitsD3PDObject(0)
