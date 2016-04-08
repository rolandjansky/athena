include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )
include( "G4SimAthenaPOOL/G4SimAthenaPOOL_joboptions.py" )

include( "PartPropSvc/PartPropSvc.py" )
EventSelector = Service( "EventSelector" )
EventSelector.InputCollection = "rfio:/castor/cern.ch/user/c/costanzo/T1/rome.004100.digit.T1_McAtNLO_top._05002.pool.root.1"; #CHANGEME!!

theApp.Dlls  += [ "MCTruthAlgs"]
theApp.TopAlg = ["TrackRecordFilter"]

MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = 2

EvtMax=10
MessageSvc.defaultLimit = 9999999  # all messages
