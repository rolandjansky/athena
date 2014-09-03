# steering file for TAG->CBNT step

# set input file (triggering autoconfiguration)
jp.AthenaCommonFlags.FilesInput=["TAG.pool.root"]
# logical selection (just an example)
#jp.AthenaCommonFlags.PoolInputQuery="(EventNumber == 2 || EventNumber == 4)  && RunNumber==105200 "

#to be checked : force navigation to ESD  jp.Rec.readESD.set_Value_and_Lock(True)

# write out another AOD
jp.Rec.doWriteAOD.set_Value_and_Lock(True)
jp.AthenaCommonFlags.PoolAODOutput="fromTAG_AOD.pool.root"


# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")
#force printout each event
try:
    ServiceMgr.AthenaEventLoopMgr.EventPrintoutInterval = 1
except Exception:
    pass
