# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)
import re

# Function to define map between L1 algorithm name and board (Topo2, Topo3 or LegacyTopo, items coming from other boards will not be listed in the map)
def getL1TopoAlgToBoardMap(lvl1access):

    lvl1items   = lvl1access.items(includeKeys=["name"])
    lvl1items_full   = lvl1access.items() 

    l1topo_alg_to_board = {}
    connectors = ['Topo2El','Topo3El','LegacyTopo0']

    for l1item in lvl1items:
        l1item_def_list = re.split("[&|]+",str(lvl1items_full[l1item]['definition']))
        for l1item_def_aux in l1item_def_list:
            l1item_def = re.sub("[[((()))]]+","",re.split("[[]+",re.sub("[()]+","",l1item_def_aux))[0]).strip()
            for connect in connectors:
                for v in lvl1access.connector(connect)['triggerlines']:
                    v_name = v['name']
                    if l1item_def==v_name:
                        l1topo_alg_to_board[l1item_def] = connect
                   
    log.debug("L1Topo alg to board map: %s",l1topo_alg_to_board)
    return l1topo_alg_to_board

def getL1TopoAlgToItemMap(lvl1access):

    lvl1items   = lvl1access.items(includeKeys=["name"])
    lvl1items_full   = lvl1access.items() 

    l1topo_alg_to_item = {}

    for l1item in lvl1items:
        l1item_def_list = re.split("[&|]+",str(lvl1items_full[l1item]['definition']))
        for l1item_def_aux in l1item_def_list:
            l1item_def = re.sub("[[((()))]]+","",re.split("[[]+",re.sub("[()]+","",l1item_def_aux))[0]).strip()
            if 'TOPO' in l1item_def:
                l1topo_alg_to_item[l1item] = l1item_def
                break
            else:
                l1topo_alg_to_item[l1item] = l1item_def

    log.debug("L1Topo alg to item map: %s",l1topo_alg_to_item)
    return l1topo_alg_to_item
    
#this function checks each threshold within each chain to make sure that it is defined in the L1Menu
def checkL1HLTConsistency(flags):
    from TrigConfIO.L1TriggerConfigAccess   import L1MenuAccess
    from TrigConfigSvc.TrigConfigSvcCfg     import getL1MenuFileName
    lvl1name    = getL1MenuFileName(flags)
    lvl1access  = L1MenuAccess(lvl1name)
    lvl1thtypes = lvl1access.thresholdTypes()
    lvl1items   = lvl1access.items(includeKeys=["name"])
    lvl1items_full   = lvl1access.items() 
    from TriggerMenuMT.HLT.Config.Utility.HLTMenuConfig import HLTMenuConfig

    allUsedItems = []
    allUnusedItems = []
    chainsWithWrongLabel = {}

    for chain in HLTMenuConfig.dictsList():
        log.debug('[checkL1HLTConsistency] Checking the l1thresholds in the chain %s', chain["chainName"])
#        #don't check the noalg chains (they don't do anything in the HLT anyway)
#        if 'HLT_noalg_' in chain["chainName"]:
#            continue

        l1topo_alg_to_board = getL1TopoAlgToBoardMap(lvl1access)
        l1topo_alg_to_item = getL1TopoAlgToItemMap(lvl1access)

        l1item_vec = chain['L1item'].split(',')
        for l1item in l1item_vec:
            if l1item == "":
                log.debug('[checkL1HLTConsistency] chain %s in L1Menu %s: L1item not set...', chain["chainName"], lvl1name)
                continue
            if l1item not in lvl1items:
                log.error('[checkL1HLTConsistency] chain %s: L1item: %s, not found in the items list of the L1Menu %s', chain["chainName"], chain["L1item"], lvl1name)
                raise Exception("Please fix the menu or the chain.")
            else:
                if l1item not in allUsedItems:
                    allUsedItems.append(l1item)

        # Find L1 Threshold information for current chain
        l1thr_vec = []
        for p in chain['chainParts']:
            #now check that the thresholds of the chain are listed in the L1Menu.thresholds field
            th = p['L1threshold'][5:] if p['L1threshold'].startswith("PROBE") else p['L1threshold']
            l1thr_vec.append(th)
            if ('TAU' in th) and th[0] not in ['e','j','c']: #legacy TAU
                th = th.replace('TAU','HA')
            thFoundInL1Menu = False
            l1type          = "NOTFOUND"
            if th == 'FSNOSEED':
                log.debug('[checkL1HLTConsistency] \t %s in L1thresholds', th)
                continue
            for thtype in lvl1thtypes:
                l1thresholds = lvl1access.thresholds(thtype)
                if th in l1thresholds:
                    thFoundInL1Menu = True
                    l1type          = thtype
                    break
            if  thFoundInL1Menu:
                log.debug('[checkL1HLTConsistency] \t %s in L1HLTConsistency[%s]', th, l1type)
            else:
                log.error('[checkL1HLTConsistency] chain %s: L1Threshold %s not found in the L1thresholds of the L1Menu %s', chain["chainName"], th, lvl1name)
                raise Exception("Please fix the menu or the chain.")

        # check that L1 seeds and L1 item are all either legacy or phase1
        legacyItem = False
        noCaloItemL1 = True
        noCaloItemHLT = True 
        foundPhase1CaloSeed = False
        foundLegacyCaloSeed = False
        for item in l1item_vec:
            if item=='':
                continue
            if 'legacy' in lvl1items_full[item]:
                if lvl1items_full[item]['legacy']:
                    legacyItem = True
                    noCaloItemL1 = False
            else:
                items = item[2:].split('_')
                for it in items:
                    if any(substring in it for substring in ['e','c','j','g']): # find phase1 calo inputs in L1Topo decision triggers
                       noCaloItemL1 = False
        
            wrongLabelTopo = False
            if l1topo_alg_to_item[item] in l1topo_alg_to_board:
                if not any('Topo' in g for g in chain['groups']):
                    log.debug("Chain has item %s but has no topo group",item)
                    wrongLabelTopo = True

            # Possibilities for wrong assignment:
            # - Item comes from topo board but has no topo group
            # - Chain has topo label but L1 item does not come from topo board
            # - Chain has wrong topo label (Topo2 when it should be Topo3 for example)
            for group in chain['groups']:
                if 'Topo' in group:
                    if l1topo_alg_to_item[item] in l1topo_alg_to_board:
                        if group in l1topo_alg_to_board[l1topo_alg_to_item[item]]:
                            log.debug("Chain correctly assigned topo board")
                        else:
                            wrongLabelTopo = True
                    elif l1topo_alg_to_item[item] not in l1topo_alg_to_board:
                        log.debug("Does not come from topo board but has Topo group")
                        wrongLabelTopo = True


        for th in l1thr_vec:
            if th=='FSNOSEED':
                continue
            if th[:1] in ['e','c','j','g']:
                foundPhase1CaloSeed = True
                noCaloItemHLT = False
            if th[:1] in ['E','T','J','X']:
                foundLegacyCaloSeed = True
                noCaloItemHLT = False 

        mixedCaloSeed = False
        if legacyItem and foundPhase1CaloSeed: # chain with L1 legacy item, cannot have phase1 seed
            mixedCaloSeed = True
        if not legacyItem:
            if not noCaloItemL1: # chain with L1 phase1 item, cannot have legacy seed
                if foundLegacyCaloSeed:
                    mixedCaloSeed = True
            else:
                if foundLegacyCaloSeed and foundPhase1CaloSeed: # chain without L1 calo items, cannot have both legacy and phase1 seeds
                    mixedCaloSeed = True 
        if mixedCaloSeed:
            log.error('[checkL1HLTConsistency] chain %s contains a mix of legacy and phase1 calo thresholds! L1 items: [%s] (legacy item = %s), L1 seeds: [%s] (foundPhase1CaloSeed = %s, foundLegacyCaloSeed = %s)', chain["chainName"], ",".join(l1item_vec), legacyItem,  ",".join(l1thr_vec), foundPhase1CaloSeed, foundLegacyCaloSeed )
            raise Exception("Please fix the chain.")

        # check legacy and phase1 chains have the correct groups 
        wrongLabel = False
        rightLabel = False
        log.debug("Checking chain %s: noCaloItemL1 %s, noCaloItemHLT %s , legacyItem %s", chain['chainName'], noCaloItemL1, noCaloItemHLT, legacyItem )
        
        log.debug(chain['groups'])        
        for group in chain['groups']:
            log.debug("Checking group %s: find 'Legacy' %s, find 'PhaseI' %s", group, str(group.find('Legacy')), str(group.find('PhaseI')) )            
                
            if noCaloItemL1 and noCaloItemHLT:
                if group.find('Legacy')>-1 or group.find('PhaseI')>-1 :
                   wrongLabel = True # chains without calo inputs should not have legacy or phase1 label 
                if group.find('Legacy')<0 and group.find('PhaseI')<0 :
                   rightLabel = True
            else: 
                if legacyItem: # chain with L1 legacy seed
                   if group.find('Legacy')>-1 :
                      rightLabel = True 
                   if group.find('PhaseI')>-1 :
                      wrongLabel = True 
                else:
                   if not noCaloItemL1: # chain with L1 phase1 item
                      if group.find('PhaseI')>-1 :
                         rightLabel = True 
                      if group.find('Legacy')>-1 :
                         wrongLabel = True 
                   if noCaloItemL1 and not noCaloItemHLT: # chain with HLT legacy or phase1 seed
                      if foundPhase1CaloSeed and group.find('Legacy')>-1 :
                         wrongLabel = True
                      if foundPhase1CaloSeed and group.find('PhaseI')>-1 :
                         rightLabel = True
                      if foundLegacyCaloSeed and group.find('PhaseI')>-1 :
                         wrongLabel = True
                      if foundLegacyCaloSeed and group.find('Legacy')>-1 :
                         rightLabel = True
        if wrongLabelTopo or wrongLabel or not rightLabel:
            chainsWithWrongLabel.update({chain['chainName']: chain['groups']})

    if len(chainsWithWrongLabel) and ('Physics' in lvl1name): # apply this check only for the Physics menu, for now
        log.error('These chains have the wrong groups:')
        for key in chainsWithWrongLabel:
            log.error('%s: %s', key, ",".join(chainsWithWrongLabel[key]))
        raise Exception("Please fix these chains.")

    # check for unused L1 items
    for item in lvl1items:
        if item not in allUsedItems:
            allUnusedItems.append(item)
    if len(allUnusedItems)==0:
        log.info('[checkL1HLTConsistency] All items in L1 menu are used')
    else:
        log.info('[checkL1HLTConsistency] %i items in L1 menu are not used: %s', len(allUnusedItems), ",".join(allUnusedItems))
    log.info('[checkL1HLTConsistency] checkL1HLTConsistency completed succesfully')
