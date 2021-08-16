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
    from TriggerMenuMT.HLTMenuConfig.Menu.TriggerConfigHLT import TriggerConfigHLT
    for chain in TriggerConfigHLT.dictsList():
        log.debug('[checkL1HLTConsistency] Checking the l1thresholds in the chain %s', chain["chainName"])
        #don't check the noalg chains (they don't do anything in the HLT anyway)
        if 'HLT_noalg_' in chain["chainName"]:
            continue

        #check that the L1item is listed in the L1Menu
        l1item = chain['L1item']
        if l1item not in lvl1items:
            if l1item != "": 
                log.error('[checkL1HLTConsistency] chain %s: L1item: %s, not found in the items list of the L1Menu %s', chain["chainName"], chain["L1item"], lvl1name)
                raise Exception("Please fix the menu or the chain.")
            else:
                log.info('[checkL1HLTConsistency] chain %s in L1Menu %s: L1item not set...', chain["chainName"], lvl1name)

        # Find L1 Threshold information for current chain
        for p in chain['chainParts']:
            #now check that the thresholds of the chain are listed in the L1Menu.thresholds field
            th = p['L1threshold'][5:] if p['L1threshold'].startswith("PROBE") else p['L1threshold']
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
    log.info('[checkL1HLTConsistency] checkL1HLTConsistency completed succesfully')
