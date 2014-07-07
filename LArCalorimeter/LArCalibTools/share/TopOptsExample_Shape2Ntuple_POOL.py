#####################################################################
#
# Dont' forget to insert your POOL file in the local XML catalog!
#
#####################################################################

PoolFileDir  = "/tmp"
PoolFileName = PoolFileDir+"/"+"LArShapePhys_29142_EB-EMBA_5samples.pool.root"

# Output file location and name

RootFileName = "/tmp/LArShape2Ntuple_POOL.root"

#####################################################################

include("LArCalibTools/LArShape2Ntuple_POOL_jobOptions.py")

#####################################################################

from AthenaCommon.AppMgr import theAuditorSvc
from AthenaCommon.ConfigurableDb import getConfigurable
theAuditorSvc += getConfigurable("MemStatAuditor")(OutputLevel = WARNING)
theAuditorSvc += getConfigurable("ChronoAuditor")()
theAuditorSvc += getConfigurable("NameAuditor")()

#####################################################################
