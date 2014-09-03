# steering file for AOD->AOD step
# see myTopOptions.py for more info
from RecExConfig.RecFlags import rec
rec.doCBNT.set_Value_and_Lock(False)
rec.doWriteTAG.set_Value_and_Lock(False) # uncomment if do not write TAG
rec.readAOD.set_Value_and_Lock(True)
rec.doWriteAOD.set_Value_and_Lock(True)
#PoolAODInput=["AOD.pool.root"]

if not 'PoolAODOutput' in dir():
    PoolAODOutput="copy_AOD.pool.root"

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

