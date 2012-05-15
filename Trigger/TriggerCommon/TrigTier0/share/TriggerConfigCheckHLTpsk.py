include.block("TriggerTier0/TriggerConfigCheckHLTpsk.py")

from AthenaCommon.Logging import logging
mlog = logging.getLogger( 'TriggerConfigCheckHLTpsk' ) ## get the logger

runNumbers=[]
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
if len(athenaCommonFlags.BSRDOInput()) > 0 :
    from RecExConfig.InputFilePeeker import inputFileSummary
    if inputFileSummary.has_key('run_number'): # online monitoring does not provide a run_number in the inputFileSummary (hence the rest of this program will be skipped)
        runNumbers=inputFileSummary['run_number']

if len(runNumbers)>0:

    #from RecExConfig.RecAlgsFlags import recAlgs
    #from RecExConfig.RecFlags import rec
    from TriggerJobOpts.TriggerFlags import TriggerFlags

    ### Loop over the input files and find if a problematic one is there.
    ### If so, turn off the trigger.
    from PyCool import cool
    from CoolConvUtilities.AtlCoolLib import indirectOpen
    import PyUtils.AthFile as AthFile

    # get connection to COOL and find the HLT ps key
    coolDbConn = indirectOpen("COOLONL_TRIGGER/COMP200",oracle=True)
    hltfolder=coolDbConn.getFolder('/TRIGGER/HLT/HltConfigKeys')
    lvl1folder=coolDbConn.getFolder('/TRIGGER/LVL1/Lvl1ConfigKey')
    chansel = cool.ChannelSelection(0,0,cool.ChannelSelection.sinceBeforeChannel)

    needToTurnOffHLT = False
    needToTurnOffLVL1 = False
    for RunNumber in runNumbers:
        # translate to begin and end IOV
        iovmin=(RunNumber << 32)+0
        iovmax=((RunNumber+1) << 32)-1
        # read info from COOL
        hltobjs = hltfolder.browseObjects( iovmin, iovmax, chansel)
        if hltobjs.goToNext():    
            obj=hltobjs.currentRef()
            ch = obj.channelId()
            hltpsk = int(obj.payloadValue("HltPrescaleConfigurationKey")) # By default is a string
            mlog.info("HLT prescale key for run %d is %d "%(RunNumber,hltpsk))
            if hltpsk == 0:
                needToTurnOffHLT = True
        else:
            needToTurnOffHLT = True
        # same thing for lvl1    
        lvl1objs = lvl1folder.browseObjects( iovmin, iovmax, chansel)
        if lvl1objs.goToNext():
            obj=lvl1objs.currentRef()
            ch = obj.channelId()
            lvl1psk = int(obj.payloadValue("Lvl1PrescaleConfigurationKey"))
            mlog.info("LVL1 prescale key for run %d is %d "%(RunNumber,lvl1psk))
            if lvl1psk == 0:
                needToTurnOffLVL1 = True
        else:
            needToTurnOffLVL1 = True
          
    if needToTurnOffHLT and needToTurnOffLVL1:
        mlog.warning("ERROR At least one run (%r) does not contain any trigger information in cool" % runNumbers)
        mlog.warning("turning off trigger [rec.doTrigger=False, and TriggerFlags.dataTakingConditions='NoTrigger']")
        TriggerFlags.dataTakingConditions='NoTrigger'
        from RecExConfig.RecFlags import rec
        from RecExConfig.RecAlgsFlags import recAlgs
        rec.doTrigger=False
        recAlgs.doTrigger=False
        if not 'DQMonFlags' in dir():
            print "TrigTier0/TriggerConfigCheckHLTpsk.py: DQMonFlags not yet imported - I import them now"
            from AthenaMonitoring.DQMonFlags import DQMonFlags
        DQMonFlags.doHLTMon.set_Value_and_Lock(False)
        DQMonFlags.useTrigger.set_Value_and_Lock(False)
        DQMonFlags.doLVL1CaloMon.set_Value_and_Lock(False)
        DQMonFlags.doCTPMon.set_Value_and_Lock(False)

    elif needToTurnOffHLT:
        mlog.warning("ERROR At least one run (%r) does not contain HLT information" % runNumbers)
        mlog.warning("turning off hlt [rec.doTrigger=True, recAlgs.doTrigger=True, and TriggerFlags.dataTakingConditions='Lvl1Only'].")
        TriggerFlags.dataTakingConditions='Lvl1Only'
        if not 'DQMonFlags' in dir():
            print "TrigTier0/TriggerConfigCheckHLTpsk.py: DQMonFlags not yet imported - I import them now"
            from AthenaMonitoring.DQMonFlags import DQMonFlags
        DQMonFlags.doHLTMon.set_Value_and_Lock(False)
        DQMonFlags.useTrigger.set_Value_and_Lock(False)

    elif needToTurnOffLVL1:
        mlog.warning("ERROR At least one run (%r) does not contain LVL1 information" % runNumbers)
        mlog.warning("turning off lvl1 [rec.doTrigger=True, recAlgs.doTrigger=True, and TriggerFlags.dataTakingConditions='HltOnly'].")
        TriggerFlags.dataTakingConditions='HltOnly'
        if not 'DQMonFlags' in dir():
            print "TrigTier0/TriggerConfigCheckHLTpsk.py: DQMonFlags not yet imported - I import them now"
            from AthenaMonitoring.DQMonFlags import DQMonFlags
#        DQMonFlags.doHLTMon.set_Value_and_Lock(False)
        DQMonFlags.useTrigger.set_Value_and_Lock(False)
        DQMonFlags.doLVL1CaloMon.set_Value_and_Lock(False)
        DQMonFlags.doCTPMon.set_Value_and_Lock(False)


    coolDbConn.closeDatabase()

