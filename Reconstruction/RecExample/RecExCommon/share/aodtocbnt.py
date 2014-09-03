# steering file for AOD->CBNT step
# see myTopOptions.py for more info


# PoolAODInput=["theaodfile"]

if not 'rec.RootNtupleOutput' in dir():
    rec.RootNtupleOutput="ntuple_fromAOD.root"

rec.doCBNT=True
rec.readAOD=True
rec.doWriteESD=False 
rec.doWriteAOD=False 
rec.doAOD=False 
rec.doWriteTAG=False 


# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

