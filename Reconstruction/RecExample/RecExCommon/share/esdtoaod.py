# steering file for ESD->AOD step
# see myTopOptions.py for more info

# jp.AthenaCommonFlags.PoolESDInput=["theesdfile"]

# PoolAODOutput="theoutput.AOD.pool.root"
if not 'rec.RootNtupleOutput' in dir():
    rec.RootNtupleOutput="ntuple_fromESDwhileAOD.root"

rec.readESD=True
rec.doWriteESD=False 
rec.doWriteAOD=True 
rec.doAOD=True 
rec.doWriteTAG=False 

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")
