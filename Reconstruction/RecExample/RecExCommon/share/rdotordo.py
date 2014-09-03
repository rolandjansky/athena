# steering file for RDO->RDO step
# see myTopOptions.py for more info
from RecExConfig.RecFlags import rec
rec.doCBNT.set_Value_and_Lock(False)
rec.doWriteRDO.set_Value_and_Lock(True)


# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

