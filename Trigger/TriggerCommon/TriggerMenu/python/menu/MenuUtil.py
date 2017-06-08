# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TriggerJobOpts.TriggerFlags import TriggerFlags
from AthenaCommon.Logging        import logging


log = logging.getLogger('TriggerMenu.menu.MenuUtil.py')



def getStreamTagForRerunChains(triggerPythonConfig, HLTPrescale):
    list=[]
    for item, prescales in HLTPrescale.iteritems():
        # prescales is a list of 3 integers [HLT_prescale, HLT_pass_through, rerun_prescale]
        if item not in triggerPythonConfig.allChains.keys():
            log.debug('Signature %s not registered to TriggerPythonConfig' % item)
            continue
        n = len(prescales)
        hltchain = None
        for ch in triggerPythonConfig.allChains[item]:
            if ch.level == 'HLT': hltchain = ch
            if n > 3  and hltchain:
                if hltchain.prescale != "0":
                    log.warning("chain "+ hltchain.chain_name + " in rerun mode with special strema tag does not have the correct HLT PS [=0] ")
                if hltchain.rerun_prescale !=  "1":
                    log.error("chain "+ hltchain.chain_name + " has special stream tag but it's not in rerun mode")
                list.append( "%s:%s" %(str(hltchain.chain_name),str(prescales[3])) )
            

    return list

                


def applyHLTPrescale(triggerPythonConfig, HLTPrescale):
    for item, prescales in HLTPrescale.iteritems():
        # prescales is a list of 3 integers [HLT_prescale, HLT_pass_through, rerun_prescale]
        if item not in triggerPythonConfig.allChains.keys():
            if triggerPythonConfig.signaturesOverwritten:
                log.warning('Attempt to set prescales for nonexisting chain: %s' % item)
                continue
            else:
                log.error('Attempt to set prescales for nonexisting chain: %s' % item)
                continue
        n = len(prescales)
        hltchain = None
        for ch in triggerPythonConfig.allChains[item]:
            if ch.level == 'HLT': hltchain = ch
        if n > 0  and hltchain:
            hltchain.prescale = str(prescales[0])
        if n > 1  and hltchain:
            hltchain.pass_through = str(prescales[1])
        if n > 2  and hltchain:
            hltchain.rerun_prescale = str(prescales[2])
       
        log.info('Applied HLTPS to the item '+item+': PS'+ hltchain.prescale+" PT"+hltchain.pass_through+" RerunPS"+hltchain.rerun_prescale)

def checkGroups(triggerPythonConfig):
    """ Make sure the groups used in Physics and MC menu exists
    """
    menu_name = TriggerFlags.triggerMenuSetup()
    log.info( "Menu: " + menu_name)
    
    from TriggerMenu.menu.GroupInfo       import getAllAllowedGroups
    allgroup=getAllAllowedGroups(menu_name)

    for chain in triggerPythonConfig.theHLTChains:
        if len(chain.groups) == 0:
            log.error( "group undefined for chain: " + str(chain.chain_name) )
        for i in chain.groups:
            if 'BW' in i:
                if i.split('BW:')[1] not in allgroup:
                    log.error( "BW Group " + str(i)+ " for chain "+ str(chain.chain_name) + " not allowed." )

                  
        
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
            log.error( "IN CHAIN: GROUP undefined for chain: " + str(chain.chain_name) )
        else:
            GroupAssigned=False
            
            for group in GroupItems:
                if group in chain.groups:
                    GroupAssigned=True
            if not GroupAssigned and CheckGroups:
                log.warning( "IN CHAIN: GROUP (primary, supporting, ... etc) undefined for chain: " + str(chain.chain_name) + ".")
                #log.warning( "IN CHAIN: GROUP undefined for chain: " + str(chain.chain_name) + ". The chain will be excluded from the menu!")
                #This part will become active soon
                #if chain in triggerPythonConfig.theL2HLTChains : triggerPythonConfig.theL2HLTChains.remove(chain)
                #if chain in triggerPythonConfig.theEFHLTChains: triggerPythonConfig.theEFHLTChains.remove(chain)

#def checkStreamConsistency(triggerPythonConfig,streamConfig):
def checkStreamConsistency(triggerPythonConfig):
    """ Checks streamer consistency
    Checks that all chains are assigned to existing streams
    """
    menu_name = TriggerFlags.triggerMenuSetup()
    log.info( "Menu: " + menu_name)

    from TriggerMenu.menu.StreamInfo       import getAllStreams

    allStreams = getAllStreams()
#    allStreams=['UPC',
#                'TRTMonitoring',
#                'WarmStart',      
#                'ZeroBias',       
#                'CosmicCalo',     
#                'ZeroBiasOverlay',
#                'CosmicMuons',
#                'CosmicCalo',
#                'IDCosmic',
#                'IDMonitoring',
#                'IDTracks',
#                'Muon_Calibration',
#                'Standby',   
#                'Background',
#                'MinBias',
#                'MinBiasOverlay',
#                'ALFA',
#                'Bphysics',
#                'physics',
#                'express',
#                'Tile',
#                'L1Muon',
#                'L1MinBias',
#                'EnhancedBias',
#                'LArCells',
#                'LArCellsEmpty',
#                'DISCARD',
#                'EFCostMonitoring',
#                'HLTPassthrough',
#                'beamspot',
#                'PixelBeam',
#                'PixelNoise',
#                'VdM',
#                'L2CostMonitoring',
#                'SCTNoise',
#                'ALFACalib',
#                'L1Calo',     
#                'L1CaloCalib',     
#                ]

    #allStreams.extend(streamConfig)

    ##Check that all the streams associated to the chains exists
    ##Chech that there is no duplication of ROB_ID for the ds streams used in a specific menu
    already_used_robs={}
    for chain in triggerPythonConfig.theHLTChains:
        if len(chain.stream_tag) == 0:
            log.error( "IN CHAIN: STREAMTAG undefined for chain: " + str(chain.chain_name) )
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
                

def resetAllPrescales(triggerPythonConfig):
    for sig in triggerPythonConfig.allChains.values():
        for chain in sig:
            if float(chain.prescale) > 0.:
                chain.prescale = '1'
            if float(chain.pass_through) > 0.:
                chain.pass_through = '1'
            if float(chain.rerun_prescale) > 0.:   
                chain.rerun_prescale = '1'
    for item in triggerPythonConfig.allItems.values():
        if float(item.prescale) > 0.:
            item.prescale = '1'
        
def allSignatures():
    sigs = []
    slices = (
        TriggerFlags.MuonSlice,
        TriggerFlags.JetSlice,
        TriggerFlags.METSlice,
        TriggerFlags.TauSlice,
        TriggerFlags.EgammaSlice,
        TriggerFlags.AFPSlice,
        TriggerFlags.MinBiasSlice,
        TriggerFlags.CombinedSlice,
        )
    for a in slices:
        if a.signatures.statusOn: sigs += a.signatures()
    return sigs


def findL1TT(chain, triggerPythonConfig):
    l2_chain = chain
    if chain.level == 'EF':
        l2_chain_name = chain.lower_chain_name
        l2_chain = triggerPythonConfig.getHLTChain(l2_chain_name)
    if l2_chain == None:
        return -2
    l1_item_name = l2_chain.lower_chain_name
    l1_item = triggerPythonConfig.getLvl1Item(l1_item_name)
    if l1_item==None:
        return -1
    return l1_item.trigger_type

