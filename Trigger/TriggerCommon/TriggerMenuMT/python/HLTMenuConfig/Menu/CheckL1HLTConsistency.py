# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)

#this function checks each threshold within each chain to make sure that it is defined in the L1Menu
def checkL1HLTConsistency():
    from TrigConfIO.L1TriggerConfigAccess   import L1MenuAccess
    from TrigConfigSvc.TrigConfigSvcCfg     import getL1MenuFileName
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    lvl1name    = getL1MenuFileName(ConfigFlags)
    lvl1access  = L1MenuAccess(lvl1name)
    lvl1thtypes = lvl1access.thresholdTypes()
    lvl1items   = lvl1access.items(includeKeys=["name"])
    lvl1items_full   = lvl1access.items() 
    from TriggerMenuMT.HLTMenuConfig.Menu.TriggerConfigHLT import TriggerConfigHLT

    allUsedItems = []
    allUnusedItems = []

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
        legacyItemNoCalo = True
        foundPhase1CaloSeed = False
        foundLegacyCaloSeed = False
        for item in l1item_vec:
            if item=='':
                continue
            if 'legacy' in lvl1items_full[item]:
                if lvl1items_full[item]['legacy']:
                    legacyItem = True
            else:
                items = item[2:].split('_')
                for it in items:
                    if it[:1] in ['e','c','j','c']:
                       legacyItemNoCalo = False
        for th in l1thr_vec:
            if th=='FSNOSEED':
                continue
            if th[:1] in ['e','c','j','c']:
                foundPhase1CaloSeed = True
            if th[:1] in ['E','T','J','X']:
                foundLegacyCaloSeed = True
        if ( (not legacyItem) and (not legacyItemNoCalo) and foundLegacyCaloSeed) or (legacyItem and foundPhase1CaloSeed):
            log.error('[checkL1HLTConsistency] chain %s contains a mix of legacy and phase1 calo thresholds! L1 items: [%s] (legacy item = %s; if yes, without calo inputs? %s ), L1 seeds: [%s] (foundPhase1CaloSeed = %s, foundLegacyCaloSeed = %s)', chain["chainName"], ",".join(l1item_vec), legacyItem, legacyItemNoCalo, ",".join(l1thr_vec), foundPhase1CaloSeed, foundLegacyCaloSeed )
            raise Exception("Please fix the chain.")

    # check for unused L1 items
    for item in lvl1items:
        if item not in allUsedItems:
            allUnusedItems.append(item)
    if len(allUnusedItems)==0:
        log.info('[checkL1HLTConsistency] All items in L1 menu are used')
    else:
        log.info('[checkL1HLTConsistency] %i items in L1 menu are not used: %s', len(allUnusedItems), ",".join(allUnusedItems))
    log.info('[checkL1HLTConsistency] checkL1HLTConsistency completed succesfully')