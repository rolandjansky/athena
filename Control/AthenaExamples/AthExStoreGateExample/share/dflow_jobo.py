###############################################################
#
# Job options file
#
#==============================================================

#--------------------------------------------------------------
# ATLAS default Application Configuration options
#--------------------------------------------------------------

# Use McEventSelector so we can run with AthenaMP
import AthenaCommon.AtlasUnixGeneratorJob

# Full job is a list of algorithms
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# Add top algorithms to be run
job += CfgMgr.AthEx__DFlowAlg1("dflow_alg1")

# Set event info key in StoreGateSvc (EventStore)
job.dflow_alg1.EvtInfo = "McEventInfo"

# Set store name and event info key
#job.dflow_alg1.EvtInfo = "AnotherStore/McEventInfo"

job += CfgMgr.AthEx__DFlowAlg2("dflow_alg2")
job += CfgMgr.AthEx__DFlowAlg3("dflow_alg3")

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = 10


#==============================================================
#
# End of job options file
#
###############################################################

