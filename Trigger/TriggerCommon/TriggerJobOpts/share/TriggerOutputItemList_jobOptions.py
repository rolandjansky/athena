# This is TriggerOutputItemListESD_joboptions.py
# It is included from RecoOutputItemList_jobOptions.py
# when doWriteESD=True and doTrigger=True

def TriggerOutputItemListESD():
        from RecExConfig.RecFlags import rec
        from AthenaCommon.Logging import logging
        log = logging.getLogger( 'TriggerOutputItemList_joboptions' )
        log.error("this file is deprecated by the Object Key Store and should no longer be used")
        if rec.doWriteESD():
            print "TriggerESDList: "
            print TriggerESDList

# empty list to avoid breaking RecExPers/RecoOutputESDList_jobOptions.py
TriggerESDList = [] 
TriggerOutputItemListESD()
