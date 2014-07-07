#####################################################################
#
# Dont' forget to insert your POOL file in the local XML catalog!
#
#####################################################################

PoolFileDir  = "/afs/cern.ch/user/l/larcalib/w0/data/WorkingDirectory/0088263_0088265_0088267_Barrel-EMB_HIGH_26_15.0.0_1/poolFiles"
PoolFileName = PoolFileDir+"/"+"LArCaliWave_0088265_EB-EMBA_StripsXtalkCorr.pool.root"
WaveType     = "Cali"

## swicth these to 'True' if you want the Wave derived quantities 
## (Baseline, TmaxAmp, MaxAmp, Width, T0, PosLobe) and Jitter info in the TTree
SaveDerivedInfo = True
SaveJitter      = True # works only with CaliWave

## Output file location and name
RootFileName = "/tmp/LArWaves2Ntuple_POOL.root"

#####################################################################

include("LArCalibTools/LArWaves2Ntuple_POOL_jobOptions.py")

#####################################################################

from AthenaCommon.AppMgr import theAuditorSvc
from AthenaCommon.ConfigurableDb import getConfigurable
theAuditorSvc += getConfigurable("MemStatAuditor")(OutputLevel = WARNING)
theAuditorSvc += getConfigurable("ChronoAuditor")()
theAuditorSvc += getConfigurable("NameAuditor")()

#####################################################################
