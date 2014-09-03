#--------------------------------------------------------------
# jobOptions for reading RDO from POOL
#--------------------------------------------------------------
# Load POOL support
#theApp.setup( POOL )

# Define input
EventSelector.InputCollections        = ["DigitizationOutput.pool.root"]
# Converters:
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )

include( "MuonEventAthenaPool/MuonEventAthenaPool_jobOptions.py" )


