# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)

#this function checks each threshold within each chain to make sure that it is defined in the L1Menu
def checkL1HLTConsistency(flags):
    from TrigConfIO.L1TriggerConfigAccess   import L1MenuAccess
    from TrigConfigSvc.TrigConfigSvcCfg     import getL1MenuFileName
    lvl1name    = getL1MenuFileName(flags)
    lvl1access  = L1MenuAccess(lvl1name)
    lvl1thtypes = lvl1access.thresholdTypes()
    lvl1items   = lvl1access.items(includeKeys=["name"])
    lvl1items_full   = lvl1access.items() 
    from TriggerMenuMT.HLT.Config.Utility.TriggerConfigHLT import TriggerConfigHLT

    allUsedItems = []
    allUnusedItems = []
    chainsWithWrongLabel = {}

    for chain in TriggerConfigHLT.dictsList():
        log.debug('[checkL1HLTConsistency] Checking the l1thresholds in the chain %s', chain["chainName"])
#        #don't check the noalg chains (they don't do anything in the HLT anyway)
#        if 'HLT_noalg_' in chain["chainName"]:
#            continue

        #check that the L1item is listed in the L1Menu
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
            if ('TAU' in th) and ('e' not in th) and ('j' not in th): 
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
        if wrongLabel or not rightLabel:
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
