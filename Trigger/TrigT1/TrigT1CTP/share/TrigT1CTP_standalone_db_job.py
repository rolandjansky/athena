# standalone steering file for testing configuration (process 0 events)

MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel  = INFO
MessageSvc.defaultLimit = 10000000

include( "TrigT1CTP/TrigT1CTPJobOptions.py" )

LVL1ConfigSvc.ConfigSource = "Oracle"
LVL1ConfigSvc.DBServer     = "devdb"
LVL1ConfigSvc.OutputLevel = VERBOSE

topSequence.CTPSimulation.OutputLevel = VERBOSE

theApp.EvtMax = 0
