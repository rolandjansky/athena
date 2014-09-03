####################
#DetDescrVersion="ATLAS-XXX-..."


#only fill event number 
include ( "EventTagAlgs/EventTagFlags.py" )
EventTagFlags.set_NonEventOff()

if not 'jp.AthenaCommonFlags.EvtMax' in dir():
    jp.AthenaCommonFlags.EvtMax=-1

try:
  jp.AthenaCommonFlags.PoolRDOInput = InColl
except: pass

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
try:
  athenaCommonFlags.PoolTAGOutput=OutColl
except: 
  athenaCommonFlags.PoolTAGOutput="SimpleTAG.pool.root"
  
doTruth=False
rec.doCBNT=False
# readAOD=True
rec.doWriteESD=False
rec.doWriteAOD=False
rec.doAOD=False
doESD=False
from RecExConfig.RecFlags import rec
#avoid rec.doWriteTAG to be switched of because there is no AOD
rec.doWriteTAG=True
rec.doWriteTAG._locked=True

# switch off the geometry which is not needed
include ("RecExCond/RecExCommon_flags.py")
DetFlags.ID_setOff()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
include ("RecExCommon/RecExCommon_topOptions.py")
##############################

