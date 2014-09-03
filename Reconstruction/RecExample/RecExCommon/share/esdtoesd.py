# steering file for ESD->ESD step
# see myTopOptions.py for more info
from RecExConfig.RecFlags import rec
rec.doCBNT.set_Value_and_Lock(False)
rec.readESD.set_Value_and_Lock(True)
rec.doWriteESD.set_Value_and_Lock(True)
rec.doAOD.set_Value_and_Lock(False)
rec.doWriteAOD.set_Value_and_Lock(False)
rec.doWriteTAG.set_Value_and_Lock(False)
# doCaloTopoCluster=True
#jp.AthenaCommonFlags.PoolESDInput=["ESD.pool.root"]
if not 'jp.AthenaCommonFlags.PoolESDOutput' in dir():
    jp.AthenaCommonFlags.PoolESDOutput="copy_ESD.pool.root"
# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

