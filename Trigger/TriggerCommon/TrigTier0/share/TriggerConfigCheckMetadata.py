include.block("TrigTier0/TriggerConfigCheckMetadata.py")

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags  import globalflags
from AthenaCommon.Logging import logging
from TriggerJobOpts.TriggerFlags import TriggerFlags

log = logging.getLogger( "TriggerConfigCheckMetadata.py" )

if len(athenaCommonFlags.PoolESDInput())>0 or len(athenaCommonFlags.PoolAODInput())>0 :
    from RecExConfig.InputFilePeeker import inputFileSummary
    if not 'DQMonFlags' in dir():
        print "DataQualityMon_RecExCommon_Flags_jobOptions.py: DQMonFlags not yet imported - I import them now"
        from AthenaMonitoring.DQMonFlags import DQMonFlags
        
    if inputFileSummary.has_key('metadata'):
        hasLVL1 = inputFileSummary['metadata'].has_key('/TRIGGER/LVL1/Lvl1ConfigKey')
        hasHLT  = inputFileSummary['metadata'].has_key('/TRIGGER/HLT/HltConfigKeys')
        

        if globalflags.DataSource()=='data':
            if hasLVL1 and not hasHLT:
                log.warning("ERROR This pool file does not contain HLT trigger information ")
                log.info("Disabling HLT monitoring, trigger aware DQMonitoring and trigger ntuples.")
                TriggerFlags.dataTakingConditions.set_Value_and_Lock('Lvl1Only')
                DQMonFlags.doHLTMon.set_Value_and_Lock(False) # now that some hltmon moved to ESDtoAOD
                DQMonFlags.useTrigger.set_Value_and_Lock(False)
                TriggerFlags.NtupleProductionFlags.ProductionLocation.set_Value_and_Lock('')

            if hasHLT and not hasLVL1:
                log.warning("ERROR This pool file does not contain LVL1 trigger information ")
                log.info("Disabling trigger ntuples and trigger aware DQMonitoring but not HLT monitoring.")
                TriggerFlags.dataTakingConditions.set_Value_and_Lock('HltOnly')
                TriggerFlags.NtupleProductionFlags.ProductionLocation.set_Value_and_Lock('')
                DQMonFlags.useTrigger.set_Value_and_Lock(False)
                DQMonFlags.doLVL1CaloMon.set_Value_and_Lock(False)
                DQMonFlags.doCTPMon.set_Value_and_Lock(False)
                try:
                    from MuonDQAMonFlags.MuonDQAProperFlags import MuonDQADetFlags
                    MuonDQADetFlags.doTGCL1Mon.set_Value_and_Lock(False)
                    MuonDQADetFlags.doTGCMon.set_Value_and_Lock(False)                    
                    log.warning("Turning off TgcLv1RawMonitoring because lvl1 info is missing")
                except Exception:
                    treatException("Could not import MuonDQADetFlags")

            if not hasHLT and not hasLVL1:
                log.warning("ERROR This pool file does not contain any trigger information ")
                log.info("Disabling HLT monitoring and trigger aware DQMonitoring and trigger ntuples.")
                TriggerFlags.dataTakingConditions.set_Value_and_Lock('NoTrigger')
                rec.doTrigger.set_Value_and_Lock(False)
                DQMonFlags.doHLTMon.set_Value_and_Lock(False)
                DQMonFlags.useTrigger.set_Value_and_Lock(False)
                DQMonFlags.doLVL1CaloMon.set_Value_and_Lock(False)
                DQMonFlags.doCTPMon.set_Value_and_Lock(False)
                TriggerFlags.NtupleProductionFlags.ProductionLocation.set_Value_and_Lock('')
                try:
                    from MuonDQAMonFlags.MuonDQAProperFlags import MuonDQADetFlags
                    MuonDQADetFlags.doTGCL1Mon.set_Value_and_Lock(False)
                    MuonDQADetFlags.doTGCMon.set_Value_and_Lock(False)                    
                    log.warning("Turning off TgcLv1RawMonitoring  because all trigger info is missing")
                except Exception:
                    treatException("Could not import MuonDQADetFlags")

        else:
            if hasLVL1 and not hasHLT:
                log.info("This pool file does not contain HLT trigger information ")
                TriggerFlags.dataTakingConditions.set_Value_and_Lock('Lvl1Only')

            if hasHLT and not hasLVL1:
                log.info("This pool file does not contain LVL1 trigger information ")
                TriggerFlags.dataTakingConditions.set_Value_and_Lock('HltOnly')

            if not hasHLT and not hasLVL1:
                log.info("This pool file does not contain any trigger information, setting rec.doTrigger to False")
                TriggerFlags.dataTakingConditions.set_Value_and_Lock('NoTrigger')
                rec.doTrigger.set_Value_and_Lock(False)
            
    else:
        log.warning("Either inputFileSummary does not have key 'metadata' or something strange is happening.")
else:
    log.warning("Wrong flags setting for pool input, try calling TriggerConfigGetter with 'ReadPool' or 'WritePool' as argument.")
