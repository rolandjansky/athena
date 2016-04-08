####################
#DetDescrVersion="ATLAS-XXX-..."

include ( "EventTagAlgs/EventTagFlags.py" )
EventTagFlags.set_NonEventOff()

if not 'EvtMax' in dir():
    EvtMax=100000

try:
  PoolAODInput = InColl
except: pass

try:
  PoolTAGOutput = OutColl
except: pass

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

