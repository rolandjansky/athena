#!/usr/bin/env python

def main():
    from TriggerMenu.api.TriggerAPI import TriggerAPI
    from TriggerMenu.api.TriggerEnums import TriggerPeriod, TriggerType
    from AthenaCommon.Logging import logging
    log = logging.getLogger("checkMenuPrimaries")
    log.setLevel(logging.INFO)
    
    p18 =  TriggerAPI.checkPeriodConsistency(TriggerPeriod.future1p8e34, TriggerType.ALL)
    p20 =  TriggerAPI.checkPeriodConsistency(TriggerPeriod.future2e34, TriggerType.ALL)
    print p18.difference(p20)
    print p20.difference(p18)

    #inconsistent =  TriggerAPI.checkPeriodConsistency(TriggerPeriod.future2e34, TriggerType.ALL)
    #if inconsistent:
    #    log.error("Some items are tighter than primaries but are lacking the 'Primary:20000' tag, please fix")
    #    log.error(inconsistent)
    #else: log.info("All primaries and backups have the 'Primary:20000' tag, cool")
    return 0

if __name__=="__main__": main()
