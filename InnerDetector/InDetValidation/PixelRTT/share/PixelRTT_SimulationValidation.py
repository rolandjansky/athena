#################################################
# Script for the PixelSimulation RTT.
# It runs digitization 
# pixel clusters.
#################################################
#
#
OutputLevel = INFO
#
# Get flags
#
from AthenaCommon.AthenaCommonFlags import jobproperties
jobproperties.AthenaCommonFlags.EvtMax=50
# HITS input file
# -----> ttbar sample
jobproperties.AthenaCommonFlags.PoolHitsInput=["root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc10/mc10_7TeV.105200.T1_McAtNlo_Jimmy.simul.HITS.e598_s933_tid168076_00/HITS.168076._008421.pool.root.1"]# -----> or get list of files from files.txt
#collection=[]
#if os.path.isfile("files.txt"):
#    for line in open("files.txt"):
#        collection.append("rfio:"+ line.strip())
#jobproperties.AthenaCommonFlags.PoolHitsInput=collection

jobproperties.AthenaCommonFlags.PoolRDOOutput="RDO.pool.root"

#
# Detector Flags (Pixel only)
#
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.SCT_setOff()
DetFlags.TRT_setOff()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOn()
DetFlags.LVL1_setOff()

#
# Include Digitization
#
#from Digitization.DigitizationFlags import jobproperties
#jobproperties.Digitization.overrideMetadata=True

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
    #job.PixelDigitization.UseCalibCondDB  = False 	# default True
    #job.PixelDigitization.UsePixMapCondDB = False 	# default True
    #job.PixelDigitization.UsePixCondSum   = False 	# default True
    
    #
    # set various enable flags
    #	
    #job.PixelDigitization.EnableHits      = False 	# default True
    job.PixelDigitization.EnableNoise     = False  	# default True
    #job.PixelDigitization.EnableSpecialPixels = False  # default True
    
    #
    # create RDOs for special pixel map monitoring
    #
    #job.PixelDigitization.RDOforSPM       = False      # default False
    
    #
    # simulate special pixels - only used if UsePixMapCondDB = False
    #
    #job.PixelDigitization.SpmNoiseProb   = 0.00001     # default 0.00001
    #job.PixelDigitization.SpmDisableProb = 0.00002     # default 0.00002
    #job.PixelDigitization.SpmNoBumpProb  = 0.00050     # default 0.00050
    #job.PixelDigitization.SpmBadTOTProb  = 0.00050     # default 0.00050

    #
    # simulated IOV - setIOV[once,all,step,rnd] 
    #
    #job.PixelDigitization.setIOVonce()

    #
    # Probabilities for random noise/in-efficiency.
    # The below probabilities are independent of special pixels.
    # They simulate random 'unknown' effects.
    #
    job.PixelDigitization.RndNoiseProb   = 0	# default 5e-8  
    job.PixelDigitization.SpmNoiseOccu   = 0	# default 1.E-5 
    job.PixelDigitization.RndDisableProb = 0	# default 0.009 

    #
    # for cosmics only!
    #    
    #job.PixelDigitization.UseComTime = True    # default False
    #job.PixelDigitization.TimeZero = -300.     # default 30
    #job.PixelDigitization.TimeBCN = 24         # default 5

    #
    # set the threshold and some parameters on charge drift
    #     
    #job.PixelDigitization.DiscrThresh = 4000   # default 4100
    #job.PixelDigitization.DiscrThreshVar = 400 # default 300
    #job.PixelDigitization.NumberOfCharges = 5  # default 10
    #job.PixelDigitization.NumberOfSteps = 100  # default 50
    #job.PixelDigitization.NoiseThresh = 200    # default 200
    #job.PixelDigitization.ThermalNoise = 220.0 # default 220.0 
      
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
# Suppress usage of pixel distortions when validating simulation
# (otherwise clusters are corrected for module bow while G4 is not)
#
from IOVDbSvc.CondDB import conddb
if not conddb.folderRequested('/Indet/PixelDist'):
  conddb.addFolder('PIXEL_OFL','/Indet/PixelDist',className="DetCondCFloat")
conddb.addOverride("/Indet/PixelDist","InDetPixelDist-nominal",className="DetCondCFloat")



# ###################################################################
#  Change the following lines if need to change PixReco COOL tag 
# ###################################################################
#
#
# Using a tag already available in COOL
#
#if not conddb.folderRequested('PIXEL/PixReco'):
#  conddb.addFolder('PIXEL_OFL','/PIXEL/PixReco')
#conddb.addOverride("/PIXEL/PixReco","PixelOfflineReco-7TeV-04")
#
#
# Using an sqlite file
#
#if not conddb.folderRequested('PIXEL/PixReco'):
#  conddb.addFolder('PIXEL_OFL','/PIXEL/PixReco')
#conddb.blockFolder("/PIXEL/PixReco")
#conddb.addFolderWithTag("mytest.db","/PIXEL/PixReco","PixelOfflineReco-7TeV-000-04-new",force=True) 
#
# ####################################################################




#
# Include clusterization
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


theApp.HistogramPersistency = 'ROOT'
if not 'OutputNTpl' in dir():
  OutputNTpl = "TrkValidation.root"

# Root file definition
if not hasattr(ServiceMgr, 'THistSvc'):
  from GaudiSvc.GaudiSvcConf import THistSvc
  ServiceMgr += THistSvc()

ServiceMgr.THistSvc.Output += [ "TRKVAL DATAFILE='" + OutputNTpl + "' TYPE='ROOT' OPT='RECREATE'" ]
theApp.Dlls += [ 'RootHistCnv' ]


#
# Include Pixel monitoring package
#

ServiceMgr.THistSvc.Output += [ "GLOBAL DATAFILE='PixelMonitoring.root' OPT='RECREATE'"]

#
# Add an AthenaMonManager algorithm to the list of algorithms to be ran
#
# AthenaMonManager is the Algorithm that manages many classes inheriting
# 	from ManagedMonitorToolBase
#
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
PixelMainsMon.RODErrorName = "Pixel_Error_Summary"
PixelMainsMon.SpacePointName = "PixelSpacePoints"
PixelMainsMon.ClusterName = "PixelClusters"
PixelMainsMon.TrackName = "Tracks"

job += monMan

