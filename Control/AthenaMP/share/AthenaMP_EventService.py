# Configure AthenaMP Strategy
from AthenaMP.AthenaMPFlags import jobproperties as jps
jps.AthenaMPFlags.Strategy="EventService"

# Configure Output Stream Sequencer
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaServices.AthenaServicesConf import OutputStreamSequencerSvc
outputStreamSequencerSvc = OutputStreamSequencerSvc()
outputStreamSequencerSvc.SequenceIncidentName = "NextEventRange"
svcMgr += outputStreamSequencerSvc
