# steering file for ESD->CBNT step
# see myTopOptions.py for more info

# jp.AthenaCommonFlags.PoolESDInput=["theesdfile"]

if not 'rec.RootNtupleOutput' in dir():
    rec.RootNtupleOutput="ntuple_fromESD.root"

rec.doCBNT=True
rec.readESD=True
rec.doWriteESD=False 
rec.doWriteAOD=False 
rec.doAOD=False 
rec.doWriteTAG=False 

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

