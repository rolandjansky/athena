#################################################
# Script for the PixelDigitization RTT.
# It also includes the clusterization algorithm.
# Histos are produced by PixelMonitor package.
#################################################
#
#
OutputLevel = INFO
#
# Get flags
#
from AthenaCommon.AthenaCommonFlags import jobproperties
jobproperties.AthenaCommonFlags.EvtMax=50
jobproperties.AthenaCommonFlags.PoolHitsInput=["root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc10/mc10_7TeV.105200.T1_McAtNlo_Jimmy.simul.HITS.e598_s933_tid168076_00/HITS.168076._008421.pool.root.1"]
jobproperties.AthenaCommonFlags.PoolRDOOutput="DigitizationOutput.pool.root"

#
# Flags that are defined in python are best set here
# switch off TRT, Calorimeters and muons
#
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
#DetFlags.SCT_setOff()
DetFlags.TRT_setOff()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()

#
# include Digitization
#
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
    #job.PixelDigitization.AuditInitialize = True
    #job.PixelDigitization.AuditExecute    = True
    #job.PixelDigitization.AuditFinalize   = True
    #
    # set whether or not to use cond DB.
    #
    #job.PixelDigitization.UseCalibCondDB  = True
    #job.PixelDigitization.UsePixMapCondDB = True
    #job.PixelDigitization.UsePixCondSum   = True
    #
    # set various enable flags
    #
    #job.PixelDigitization.EnableHits      = True
    #job.PixelDigitization.EnableNoise     = True

    #
    # create RDOs for special pixel map monitoring
    #
    #job.PixelDigitization.RDOforSPM       = False
    
    #
    # simulate special pixels - only used if UsePixMapCondDB = False
    #
    #job.PixelDigitization.SpmNoiseProb   = 0.00001
    #job.PixelDigitization.SpmDisableProb = 0.00002
    #job.PixelDigitization.SpmNoBumpProb  = 0.00050
    #job.PixelDigitization.SpmBadTOTProb  = 0.00050

    # simulated IOV - setIOV[once,all,step,rnd]
    #job.PixelDigitization.setIOVonce()

    #
    # Probabilities for random noise/in-efficiency.
    # The below probabilities are independent of special pixels.
    # They simulate random 'unknown' effects.
    #
    #job.PixelDigitization.RndNoiseProb   = 5e-8
    #job.PixelDigitization.SpmNoiseOccu   = 1.E-5
    #job.PixelDigitization.RndDisableProb = 1e-3

    #
    # Bow simulation
    #
    #job.PixelDigitization.SimulateBow = 0
    #job.PixelDigitization.DistoFilename = "PixelSurveyDistorsions.txt"
    #job.PixelDigitization.DistoTag = "Datafile"
    
    print job.PixelDigitization
else:
    logger.warning("PixelDigitization configurable not found in AlgSequence() - assuming oldstyle setup is used.")
#
# include clusterization
# (need to set up services not already configured for digitization) 
#
from IOVDbSvc.CondDB import conddb
if not conddb.folderRequested("/PIXEL/PixReco"):
    conddb.addFolder("PIXEL_OFL", "/PIXEL/PixReco", className="DetCondCFloat")

from AthenaCommon.AlgSequence import AthSequencer
condSeq = AthSequencer("AthCondSeq")
if not hasattr(condSeq, 'PixelOfflineCalibCondAlg'):
    from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelOfflineCalibCondAlg
    condSeq += PixelOfflineCalibCondAlg(name="PixelOfflineCalibCondAlg", ReadKey="/PIXEL/PixReco")
    PixelOfflineCalibCondAlg.InputSource = 2

from InDetPrepRawDataFormation.InDetPrepRawDataFormationConf import InDet__PixelClusterization

pixelClusterization = InDet__PixelClusterization("PixelClusterization")

from RegionSelector.RegSelToolConfig import makeRegSelTool_Pixel
pixelClusterization.RegSelTool = makeRegSelTool_Pixel()

job += pixelClusterization

print job.PixelClusterization



from SiSpacePointFormation.SiSpacePointFormationConf import InDet__SiTrackerSpacePointFinder
job += InDet__SiTrackerSpacePointFinder("PixelSpacePoints",
                                        ProcessSCTs = False)
print job.PixelSpacePoints

#
# include pixel monitoring package
#
## Root file definition
if not hasattr(ServiceMgr, 'THistSvc'):
    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += THistSvc()

ServiceMgr.THistSvc.Output += [ "GLOBAL DATAFILE='TestMon.root' OPT='RECREATE'"]


## add an AthenaMonManager algorithm to the list of algorithms to be ran
## AthenaMonManager is the Algorithm that manages many classes inheriting
## from ManagedMonitorToolBase

from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
monMan = AthenaMonManager( "PixelMonManager" )

from PixelMonitoring.PixelMonitoringConf import *
PixelMainsMon=PixelMainMon()
#ToolSvc += PixelMainsMon
monMan.AthenaMonTools += [ PixelMainsMon ]
print PixelMainsMon

## FileKey must match that given to THistSvc
monMan.FileKey = "GLOBAL"

## Set global monitoring parameters: see the AthenaMonManager class
## in the Control/AthenaMonitoring package
monMan.ManualDataTypeSetup = True
monMan.DataType            = "montecarlo"
monMan.Environment         = "user"
monMan.ManualRunLBSetup    = True
monMan.Run                 = 1
monMan.LumiBlock           = 1

## Set pixel monitoring parameters
##Flags for data container types
PixelMainsMon.doRDO = True
PixelMainsMon.doErrors = True
PixelMainsMon.doSpacePoint = True 
PixelMainsMon.doCluster = True 
PixelMainsMon.doTrack = False 

##Flags for environment types
PixelMainsMon.doOffline = True 

##Names of storegate containers
PixelMainsMon.RDOName = "PixelRDOs"
PixelMainsMon.RODErrorName = "pixel_error_summary"
PixelMainsMon.SpacePointName = "PixelSpacePoints"
PixelMainsMon.ClusterName = "PixelClusters"
PixelMainsMon.TrackName = "Tracks"

job += monMan
