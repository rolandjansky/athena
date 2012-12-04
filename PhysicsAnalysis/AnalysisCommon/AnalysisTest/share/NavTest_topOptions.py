# read AOD
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )

EventSelector = Service( "EventSelector" )

# The AOD input file
EventSelector.InputCollections = [ "AOD.pool.root" ]

# enable Back Navigation
EventSelector.BackNavigation = True

# POOL converters
include( "LArAthenaPool/LArAthenaPool_joboptions.py" )
include( "TrkEventAthenaPool/TrkEventAthenaPool_joboptions.py" )
include( "GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPool_joboptions.py")
include( "RecAthenaPool/RecAthenaPool_joboptions.py" )
include( "ParticleEventAthenaPool/ParticleEventAthenaPool_joboptions.py" )

# DLLs
theApp.Dlls   += [ "AnalysisTest" ]

# Algs
theApp.TopAlg += [ "NavTest" ]

# number of events to be processed
theApp.EvtMax = 1000000
