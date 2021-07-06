#!/usr/bin/env python

def main():
    from TriggerMenuMT.TriggerAPI.TriggerAPI import TriggerAPI
    from TriggerMenuMT.TriggerAPI.TriggerEnums import TriggerPeriod, TriggerType
    from AthenaCommon.Logging import logging
    log = logging.getLogger("checkMenuPrimaries")
    log.setLevel(logging.INFO)

    TriggerAPI.setRelease("current")
    inconsistent =  TriggerAPI.checkPeriodConsistency(TriggerPeriod.future2e34, TriggerType.ALL)
    if inconsistent:
        log.error("Some items are tighter than primaries but are lacking the 'Primary' tag, please fix")
        log.error(inconsistent)
    else: log.info("All primaries and backups have the 'Primary' tag, cool")
    return 0

if __name__=="__main__": main()
