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


# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

