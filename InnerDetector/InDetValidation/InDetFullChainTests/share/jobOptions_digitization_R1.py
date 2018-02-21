#--------------------------------------------------------------
# Template jobOptions: ID only simulation for run 1 ATLAS
#  - Digitization
#--------------------------------------------------------------

from AthenaCommon.AthenaCommonFlags import jobproperties
jobproperties.AthenaCommonFlags.EvtMax=-1
jobproperties.AthenaCommonFlags.PoolHitsInput=["singleMuon_R1_Hits.pool.root"]
jobproperties.AthenaCommonFlags.PoolRDOOutput="singleMuon_R1_RDO.pool.root"

from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOn()
DetFlags.LVL1_setOff()

from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion='ATLAS-R1-2012-03-00-00'

## Random Number Seed offsets
## should be altered between jobs in order to change noise etc.
from Digitization.DigitizationFlags import jobproperties
jobproperties.Digitization.rndmSeedOffset1=11
jobproperties.Digitization.rndmSeedOffset2=22

## Main Digitization configuration
include ( "Digitization/Digitization.py" )


## Re-configure MessageSvc
MessageSvc = Service( "MessageSvc" )
#increase the number of letter reserved to the alg/tool name from 18 to 30
MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M"
# to change the default limit on number of message per alg
MessageSvc.defaultLimit = 9999999  # all messages
# Set output level threshold among DEBUG, INFO, WARNING, ERROR, FATAL
MessageSvc.OutputLevel = INFO
