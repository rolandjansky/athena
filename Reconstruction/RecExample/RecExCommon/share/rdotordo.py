# steering file for RDO->RDO step
# see myTopOptions.py for more info
from RecExConfig.RecFlags import rec
rec.doCBNT.set_Value_and_Lock(False)
rec.doWriteRDO.set_Value_and_Lock(True)


include ("RecExCond/RecExCommon_flags.py")
rec.AutoConfiguration = ['everything']
import RecExConfig.AutoConfiguration as auto
auto.ConfigureFromListOfKeys(rec.AutoConfiguration())


# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

