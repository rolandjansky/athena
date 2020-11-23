#do not save AOD cells
rec.doAODCaloCells =False

# turning off some monitoring
if rec.doMonitoring() :
    # 20.10.2015 failing (pp containers), turn off
    from AthenaMonitoring.DQMonFlags import DQMonFlags
    DQMonFlags.doMissingEtMon.set_Value_and_Lock(False)
    DQMonFlags.doTauMon.set_Value_and_Lock(False)
    # 20.01.2015 failing (pp containers), turn off
    from TrigHLTMonitoring.HLTMonFlags import HLTMonFlags
    HLTMonFlags.doTau.set_Value_and_Lock(False)
