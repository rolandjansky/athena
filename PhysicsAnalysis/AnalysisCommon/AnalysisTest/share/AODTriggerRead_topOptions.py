# read AOD
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )
EventSelector = Service( "EventSelector" )

# The AOD input file
EventSelector.InputCollections = [ "AOD_Trigger.root" ]

# POOL converters
include( "AnalysisTriggerEventAthenaPool/AnalysisTriggerEventAthenaPool_joboptions.py" )

# DLLs
theApp.Dlls   += [ "AnalysisTest" ]

# Algs
theApp.TopAlg += [ "ReadTrigger" ]

# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = INFO

# message level for ReadXYZ
ReadTrigger = Algorithm( "ReadTrigger" )
ReadTrigger.OutputLevel = DEBUG

# Number of Events to process
theApp.EvtMax = 1000000

# Root Ntuple output
theApp.Dlls += [ "RootHistCnv" ]
theApp.HistogramPersistency = "ROOT"
NTupleSvc = Service( "NTupleSvc" )
NTupleSvc.Output = [ "FILE1 DATAFILE='AODTest.root' OPT='NEW'" ]
HistogramPersistencySvc = Service( "HistogramPersistencySvc" )
HistogramPersistencySvc.OutputFile = "hist.root";

