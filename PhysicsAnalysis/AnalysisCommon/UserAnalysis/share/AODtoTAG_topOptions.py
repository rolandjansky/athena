####################
#DetDescrVersion="ATLAS-XXX-..."

PoolAODInput=["AOD.pool.root"]
PoolTAGOutput="TAG.pool.root"

if not 'EvtMax' in dir():
    EvtMax=10


doHist=False
doCBNT=False
readAOD=True
doWriteESD=False
doWriteAOD=False
doAOD=False
doESD=False
doWriteTAG=True


######################## main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")
##############################

