###########################################
# A short script to test PixelDigitization
###########################################
#
# Set auditors to use.
# Chrono:  time used by algos
# Name:    print info upon entry/exit of init/exec/fina routines of the algo
# MemStat: memory usage
#
theApp.Dlls += [ "GaudiAud" ]
theAuditorSvc.Auditors  += [ "ChronoAuditor"]
theAuditorSvc.Auditors  += [ "NameAuditor" ]
theAuditorSvc.Auditors  += [ "MemStatAuditor" ]
MemStatAuditor = theAuditorSvc.auditor( "MemStatAuditor" )
theApp.AuditAlgorithms=True 
MemStatAuditor.OutputLevel = INFO
#
# Pixel digitization using a configurable class.
#
from AthenaCommon.Logging import logging
logger = logging.getLogger("PixelDigitization")

#
# Get flags
#
from AthenaCommon.AthenaCommonFlags import jobproperties
jobproperties.AthenaCommonFlags.EvtMax=5
#
# define input hits and output digits
#
#jobproperties.AthenaCommonFlags.PoolHitsInput=["rfio:/castor/cern.ch/user/s/svahsen/digitization/RTT/calib1_csc11.005200.T1_McAtNlo_Jimmy.simul.HITS.v12003104_tid004131._00069.pool.root.10"]
jobproperties.AthenaCommonFlags.PoolHitsInput=["/afs/cern.ch/user/t/tegen/scratch0/calib1_csc11.005200.T1_McAtNlo_Jimmy.simul.HITS.v12003104_tid004131._00069.pool.root.10"]
jobproperties.AthenaCommonFlags.PoolRDOOutput="DigitizationOutput.pool.root"

#
# Flags that are defined in python are best set here
# switch off ID and muons
#
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.SCT_setOn()
DetFlags.TRT_setOff()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOn()
DetFlags.LVL1_setOff()


from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion='ATLAS-CSC-01-02-00'
#GeoModelSvc = Service( "GeoModelSvc" )
#GeoModelSvc.IgnoreTagDifference = True

include("Digitization/Digitization.py")
#
# PixelDigitization configurable is added to the job sequence in
# PixelDigitization_jobOptions.py.
# It can be accessed by job.PixelDigitization as below.
#
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
if 'PixelDigitization' in dir(job):
    #
    # set audit flags
    #
    job.PixelDigitization.AuditInitialize = True
    job.PixelDigitization.AuditExecute    = True
    job.PixelDigitization.AuditFinalize   = True
    #
    # set whether or not to use cond DB.
    #
    job.PixelDigitization.UseCalibCondDB  = True
    job.PixelDigitization.UsePixMapCondDB = True
    job.PixelDigitization.UsePixCondSum   = True
    #
    # set various enable flags
    #
    job.PixelDigitization.EnableHits      = True
    job.PixelDigitization.EnableNoise     = True

    #
    # create RDOs for special pixel map monitoring
    #
    job.PixelDigitization.RDOforSPM       = True

    #
    # simulate special pixels - only used if UsePixMapCondDB = False
    #
    job.PixelDigitization.SpmNoiseProb   = 0.00001
    job.PixelDigitization.SpmDisableProb = 0.00002
    job.PixelDigitization.SpmNoBumpProb  = 0.00050
    job.PixelDigitization.SpmBadTOTProb  = 0.00050
    
    # simulated IOV - setIOV[once,all,step,rnd]
    job.PixelDigitization.setIOVonce()

    #
    # Probabilities for random noise/in-efficiency.
    # The below probabilities are independent of special pixels.
    # They simulate random 'unknown' effects.
    #
    job.PixelDigitization.RndNoiseProb   = 5e-8
    job.PixelDigitization.RndDisableProb = 0.0

else:
    logger.warning("PixelDigitization configurable not found in AlgSequence() - assuming oldstyle setup is used.")

#
MessageSvc.OutputLevel = INFO
#


