# read AOD
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )
EventSelector = Service( "EventSelector" )

# The AOD input file
EventSelector.InputCollections = [ "AOD.pool.root" ]
EventSelector.BackNavigation = TRUE

# POOL converters
include( "ParticleBuilderOptions/ESD_PoolCnv_jobOptions.py" )
include( "ParticleBuilderOptions/AOD_PoolCnv_jobOptions.py" )

# Number of Events to process
theApp.EvtMax = -1

# initialize AnalysisCore
include ("PyAnalysisCore/InitPyAnalysisCore.py")
