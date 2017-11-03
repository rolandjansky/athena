# steering file for RDO->ESD step without trigger
# see myTopOptions.py for more info

#rec.doCBNT=False
rec.doTrigger=False
doWriteRDO=False
if not 'rec.doWriteESD' in dir():
    rec.doWriteESD=True
rec.doWriteAOD=False
rec.doAOD=False 
rec.doWriteTAG=False 

include ("RecExCond/RecExCommon_flags.py")
rec.AutoConfiguration = ['everything']
import RecExConfig.AutoConfiguration as auto
auto.ConfigureFromListOfKeys(rec.AutoConfiguration())


# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

