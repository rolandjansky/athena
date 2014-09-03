#--------------------------------------------------------------
# jobOptions for reading Digit from POOL
#--------------------------------------------------------------
# Load POOL support
theApp.setup( POOL )

# Define input
EventSelector.InputCollection        = "MuonPool.root"
# Converters:
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )

include( "MuonEventAthenaPool/MuonEventAthenaPool_joboptions.py" )

