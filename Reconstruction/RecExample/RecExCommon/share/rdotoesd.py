# steering file for RDO->ESD step
# see myTopOptions.py for more info
#rec.doCBNT=False

# jp.AthenaCommonFlags.PoolRDOInput=["therdofile"]
# jp.AthenaCommonFlags.PoolESDOutput="theoutput.ESD.pool.root"
if not 'rec.doWriteESD' in dir():
    rec.doWriteESD=True

doWriteRDO=False
rec.doWriteAOD=False 
rec.doAOD=False 
rec.doWriteTAG=False 


# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

