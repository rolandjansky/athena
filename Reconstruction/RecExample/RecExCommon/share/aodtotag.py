####################
#DetDescrVersion="ATLAS-XXX-..."
#PoolAODInput=["myAOD.pool.root"]
#PoolTAGOutput="TAG.pool.root"

rec.doCBNT=False
rec.readAOD=True
rec.doWriteESD=False
rec.doWriteAOD=False
rec.doAOD=False
rec.doESD=False
rec.doWriteTAG=True

### uncomment these to switch off Trigger in the TAG
#include ('EventTagAlgs/EventTagFlags.py')
#EventTagFlags.set_TriggerOff()

include ("RecExCond/RecExCommon_flags.py")
rec.AutoConfiguration = ['everything']
import RecExConfig.AutoConfiguration as auto
auto.ConfigureFromListOfKeys(rec.AutoConfiguration())

######################## main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")
##############################

