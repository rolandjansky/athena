# This is TriggerOutputItemListAOD_joboptions.py
# It is included from ParticleBuilderOptions/share/AOD_OutputList_jobOptions.py
# when doWriteAOD=True and doTrigger=True

# add LVL1 and HLT output objects for AOD

def TriggerOutputItemListAOD():
    from RecExConfig.RecFlags import rec
    from AthenaCommon.Logging import logging
    log = logging.getLogger( 'TriggerOutputItemListAOD_joboptions' )
    log.error("this file is deprecated by the Object Key Store and should no longer be used")
    TriggerAODList = []
    if rec.doWriteAOD():
        print "TriggerAODList: "
        print TriggerAODList

TriggerOutputItemListAOD()

