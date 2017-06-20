#-- ------------------------------------------------------------
# Private Application Configuration options
#-- ------------------------------------------------------------
# Full job is a list of algorithms
from AthenaCommon.AlgSequence import AlgSequence
  job = AlgSequence()
# Add top algorithms to be run
        from TrigAFPHypo.TrigAFPHypoConf import TrigAFPJetAllTE
        job += TrigAFPHypo("TrigAFPHypo")
#-- ------------------------------------------------------------
# Set output level threshold (DEBUG, INFO, WARNING, ERROR, FATAL)
#-- ------------------------------------------------------------
               job.TrigAFPHypo.OutputLevel = INFO
#-- ------------------------------------------------------------
# Event related parameters
#-- ------------------------------------------------------------
# Number of events to be processed (default is 10)
#theApp .EvtMax = 1
#== ============================================================
