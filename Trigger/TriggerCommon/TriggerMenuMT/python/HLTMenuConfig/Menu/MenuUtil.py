# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from TriggerJobOpts.TriggerFlags import TriggerFlags
from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)

def checkGroups(triggerPythonConfig):
    """ Make sure the groups used in Physics and MC menu exists
    """
    menu_name = TriggerFlags.triggerMenuSetup()
    log.info( "Menu: " + menu_name)
    
    from TriggerMenuMT.HLTMenuConfig.Menu.GroupInfo       import getAllAllowedGroups
    allgroup=getAllAllowedGroups(menu_name)

    for chain in triggerPythonConfig.theHLTChains:
        if len(chain.groups) == 0:
            log.error( "group undefined for chain: %s", chain.chain_name )
        for i in chain.groups:
            if 'BW' in i:
                if i.split('BW:')[1] not in allgroup:
                    log.error( "BW Group %d for chain %s not allowed.", i, chain.chain_name)

                  
        
def checkTriggerGroupAssignment(triggerPythonConfig):
    """ Checks menu consistency

    Checks trigger-groups assignment in Physics_ and MC_ menus.
    Excludes those chains which either have no group assigned or have a wrong group attached in Physics_
    which allows only primary, supporting and calibration triggers. 
    """
    menu_name = TriggerFlags.triggerMenuSetup()
    log.info( "Menu: " + menu_name)
    GroupItems = []
    CheckGroups=False

    if menu_name.startswith('Physics_lumi'):
        CheckGroups=True
        GroupItems = ['Primary', 'Supporting', 'Calibration', 'Monitoring']
    elif menu_name.startswith('MC_lumi'):
        CheckGroups=True
        GroupItems = ['Primary', 'Supporting', 'Calibration', 'Monitoring', 'Backup', 'Commissioning', 'Test']

    for chain in triggerPythonConfig.theL2HLTChains + triggerPythonConfig.theEFHLTChains:
        if len(chain.groups) == 0:
            log.error( "IN CHAIN: GROUP undefined for chain: %s", chain.chain_name )
        else:
            GroupAssigned=False
            
            for group in GroupItems:
                if group in chain.groups:
                    GroupAssigned=True
            if GroupAssigned is False and CheckGroups is True:
                log.warning( "IN CHAIN: GROUP (primary, supporting, ... etc) undefined for chain: %s", chain.chain_name )

#def checkStreamConsistency(triggerPythonConfig,streamConfig):
def checkStreamConsistency(triggerPythonConfig):
    """ Checks streamer consistency
    Checks that all chains are assigned to existing streams
    """
    menu_name = TriggerFlags.triggerMenuSetup()
    log.info( "Menu: " + menu_name)

    from TriggerMenuMT.HLTMenuConfig.Menu.StreamInfo       import getAllStreams

    allStreams = getAllStreams()

    ##Check that all the streams associated to the chains exists
    ##Chech that there is no duplication of ROB_ID for the ds streams used in a specific menu
    already_used_robs={}
    for chain in triggerPythonConfig.theHLTChains:
        if len(chain.stream_tag) == 0:
            log.error( "IN CHAIN: STREAMTAG undefined for chain: %s", chain.chain_name )
        else:
            for stream in chain.stream_tag:
                if stream[0] not in allStreams:
                    log.error(' Chain: ' + chain.chain_name + ' has the wrong streamer ' + stream[0])
                else:
                    ##check data scouting streaming name
                    if "DataScouting" in stream[0]:
                        rob_id= stream[0].split("_")[1]                        
                        if rob_id  in already_used_robs and stream[0] is not already_used_robs[rob_id]:
                            log.error( "Duplicated ROB in stream " + stream[0] + 
                                       "(ROB number " + str(stream[0].split("_")[1]) + " already used in stream " +  already_used_robs[stream[0].split("_")[1]] + ")")
                            already_used_robs[rob_id]=stream[0]
                        else:                 
                            already_used_robs[rob_id]=stream[0] 

    
    ##Chech that there is no duplication of ROB_ID for the ds streams defined in the all TM
    already_used_robs={}
    for stream in allStreams:
        if "DataScouting" in stream:
            rob_id= stream.split("_")[1]                        
            if rob_id  in already_used_robs:
                log.error( "Duplicated ROB in stream " + stream + 
                           "(ROB number " + str(rob_id) + " already used in stream " +  already_used_robs[rob_id] + ")")
                already_used_robs[rob_id]=stream
            else:
                already_used_robs[rob_id]=stream
                

def allSignatures():
    sigs = []
    slices = (
        TriggerFlags.MuonSlice,
        TriggerFlags.JetSlice,
        TriggerFlags.METSlice,
        TriggerFlags.TauSlice,
        TriggerFlags.EgammaSlice,
        TriggerFlags.MinBiasSlice,
        TriggerFlags.CombinedSlice,
        )
    for a in slices:
        if a.signatures.statusOn: 
            sigs += a.signatures()
    return sigs


def findL1TT(chain, triggerPythonConfig):
    l2_chain = chain
    if chain.level == 'EF':
        l2_chain_name = chain.lower_chain_name
        l2_chain = triggerPythonConfig.getHLTChain(l2_chain_name)
    if l2_chain is None:
        return -2
    l1_item_name = l2_chain.lower_chain_name
    l1_item = triggerPythonConfig.getLvl1Item(l1_item_name)
    if l1_item is None:
        return -1
    return l1_item.trigger_type

