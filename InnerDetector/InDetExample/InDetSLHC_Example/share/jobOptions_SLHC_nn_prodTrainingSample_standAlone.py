##############################################################################
# jobOptions_SLHC_nn_prodTrainingSample_standAlone.py
# Based of DeLorenzi's:   "~lorenzi/public/clusterizationTools/DigiAndReco.py"
#
# This script read a series of simulated HIT files and produces the Ntuples
# needed to train the cluster splitting neuronal network.
# The ntuples produced are stored in TrkValidation.root
# -Validation
#       |-> PixelRIOs    : Cluster info.
#       |-> NNinput      : Input to train the NN.
#
# Note:   This jobOptions WILL NOT WORK as it is neither for SLHC nor for IBL.
#         YOU NEED TO EDIT PixelClusterValidationNtupleWriter.cxx
#         IN  InnerDetector/InDetValidation/InDetTrackValidation/InDetTrackValidation/
#         TO USE ToT INSTEAD OF CHARGE IN NNinput
#
# Note 2: This is a stand-alone jobOptions script that work todays but can't
#         guarantee that will work always, there are some picky config options down there...
#         I also prepared a script to work using the standard SLHC options. 
#
# Note 3: SCT is not needed to produce NN trainnig samples without tracking, but
#         you'll need it later on for tracking. So you can just leave it on so you
#         can reuse the produce digit.RDO file.
#
# Author: Tiago Perez <tperez@cern.ch>
# Date:   15-Dec-2011
##############################################################################
#
#
OutputLevel = WARNING
#
# Get flags
#
from AthenaCommon.AthenaCommonFlags import jobproperties
# Numbers of events to process: set -1 for all.
jobproperties.AthenaCommonFlags.EvtMax= -1

#
## OUTPUT
#
## Output dir. Leave it empty to write in the current working dir.
OUTDIR="~/scratch0/prodTrainingSample_standAlone/"
## Dir to write the digit file. This file can be very large, so better don't try to write
## your afs space.
RDOOUT="root://eosatlas.cern.ch//eos/atlas/user/t/tperez/prodTrainingSample_standAlone/"

#
## INPUT
#
## Input HIT files
DATADIR="root://eosatlas.cern.ch//eos/atlas/user/t/tperez/"
#
## Electrons
## File form sample: group.det-slhc.mc11_14TeV.4907070.multi_electrons.simul.HITS.etest1.sslhc01
##DATADIR+="group.det-slhc.mc11_14TeV.4907070.multi_electrons.simul.HITS.etest1.sslhc01.110907225930/"
##FILEPATH=DATADIR+"group.det-slhc.32103_001265.EXT0._00005.HITS.pool.root"
#
## MinBias
## File form sample: group.det-slhc.mc11_14TeV.4907070.multi_electrons.simul.HITS.etest1.sslhc01
FILEPATH=DATADIR+"mc11_slhcid.108119.Pythia8_minbias_Inelastic_high.merge.HITS.e876_s1333_s1335_tid514272_00/"
FILEPATH+="HITS.514272._000030.pool.root.1"
## ttbar
## FILEPATH=DATADIR+"mc11_slhcid.105568.ttbar_Pythia.simul.HITS.e842_s1333_tid510282_00/"
## FILEPATH+="HITS.510282._000429.pool.root.1"
#
#
jobproperties.AthenaCommonFlags.PoolHitsInput=[FILEPATH]
#
# RDO Output file
jobproperties.AthenaCommonFlags.PoolRDOOutput=RDOOUT+"minbias_standAlone_.digit.RDO.pool.root"
#
# Need to define SLHC layout (Standard UTOPIA) else WARNING:
from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion='ATLAS-SLHC-01-00-00'

#
# I dont need this, I'm not retrieving anything from Conditions DB
from AthenaCommon.GlobalFlags import globalflags
globalflags.ConditionsTag = "OFLCOND-SDR-BS14T-ATLAS-00"

#
# Detector Flags (Pixel only)
#
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
#DetFlags.SCT_setOff()
DetFlags.TRT_setOff()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOn()
DetFlags.LVL1_setOff()
## Disable RDO write out.
##DetFlags.writeRDOPool.all_setOff()

from Digitization.DigitizationFlags import digitizationFlags
digitizationFlags.doInDetNoise=False
#
# Include Digitization
#
include("Digitization/Digitization.py")

#disable some Pixel stuff
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
pixelTool = svcMgr.GeoModelSvc.DetectorTools['PixelDetectorTool']
pixelTool.Alignable = False
#
#
from PixelConditionsTools.PixelConditionsToolsConf import PixelRecoDbTool
ToolSvc += PixelRecoDbTool()
ToolSvc.PixelRecoDbTool.InputSource = 0

Stream1.ItemList += [ "SiHitCollection#PixelHits" ]
#
# PixelDigitization configurable is added to the job sequence in
# PixelDigitization_jobOptions.py.
# It can be accessed by job.PixelDigitization as below.
#
from AthenaCommon.CfgGetter import getService, getPublicTool
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
pixeldigi = None
from PyJobTransforms.trfUtils import releaseIsOlderThan
if releaseIsOlderThan(19,2):
    if not digitizationFlags.doXingByXingPileUp() and hasattr(job, 'PixelDigitization'):
        if hasattr( job.PixelDigitization, 'DigitizationTool'):
            pixeldigi = job.PixelDigitization.DigitizationTool
        else:
            pixeldigi = job.PixelDigitization
    elif hasattr( ToolSvc, 'PixelDigitizationTool'):
        pixeldigi = ToolSvc.PixelDigitizationTool
    else:
        for alg in job:
            if hasattr(alg, 'PileUpTools'):
                pixeldigi = alg.PileUpTools[ "PixelDigitizationTool" ]
                break
            pass
else:
    pixeldigi = getPublicTool("PixelDigitizationTool")
if None == pixeldigi:
    raise AttributeError("PixelDigitization(Tool) not found.")

pixeldigi.EnableSpecialPixels = False
if hasattr(pixeldigi,'UseCalibCondDB'):
    pixeldigi.UseCalibCondDB = False
    pixeldigi.UsePixMapCondDB = False
    pixeldigi.UsePixCondSum = False
    pixeldigi.DisableDistortions = True
    pixeldigi.EnableNoise     = False  	# default True
    #
    pixeldigi.RndNoiseProb   = 0	# default 5e-8  
    pixeldigi.SpmNoiseOccu   = 0	# default 1.E-5 
    pixeldigi.RndDisableProb = 0	# default 0.009    
    print job.PixelDigitization
else:
    #From PixelDigitization-01-00-05 onwards configure tools directly
    calibSvc = getService("CalibSvc")
    calibSvc.UseCalibCondDB = False
    calibSvc.UsePixMapCondDB = False
    getPublicTool("SpecialPixelGenerator").UsePixCondSum = False
    getPublicTool("PixelBarrelChargeTool").DisableDistortions = True
    getPublicTool("PixelECChargeTool").DisableDistortions = True
    getPublicTool("DBMChargeTool").DisableDistortions = True
    getPublicTool("IblPlanarChargeTool").DisableDistortions = True
    getPublicTool("Ibl3DChargeTool").DisableDistortions = True
    pixeldigi.EnableNoise = False
    getPublicTool("PixelNoisyCellGenerator").RndNoiseProb   = 0
    getPublicTool("PixelNoisyCellGenerator").SpmNoiseOccu   = 0

if DetFlags.digitize.SCT_on():
    Stream1.ItemList += [ "SiHitCollection#SCT_Hits" ]
    from SCT_Digitization.SCT_DigitizationConf import SCT_FrontEnd
    theSCT_FrontEnd = SCT_FrontEnd("SCT_FrontEnd")
    theSCT_FrontEnd.MaxStripsPerSide= 1280
    theSCT_FrontEnd.UseCalibData=False
    ToolSvc += theSCT_FrontEnd

#
# Suppress usage of pixel distortions when validating simulation
# (otherwise clusters are corrected for module bow while G4 is not)
#
from IOVDbSvc.CondDB import conddb
if not conddb.folderRequested('/Indet/PixelDist'):
    conddb.addFolder('PIXEL_OFL','/Indet/PixelDist')
    conddb.addOverride("/Indet/PixelDist","InDetPixelDist-nominal")

#
# Include clusterization
# (need to set up services not already configured for digitization) 
#
#include ("PixelConditionsServices/PixelRecoDb_jobOptions.py")
#
## Disable some COOL queries ?
from PixelConditionsTools.PixelConditionsToolsConf import PixelRecoDbTool
ToolSvc += PixelRecoDbTool()
ToolSvc.PixelRecoDbTool.InputSource = 0

## Configure the clusterization tool
from SiClusterizationTool.SiClusterizationToolConf import InDet__ClusterMakerTool
ClusterMakerTool = InDet__ClusterMakerTool( name = "InDet::ClusterMakerTool",
                                            UsePixelCalibCondDB = False )
ToolSvc += ClusterMakerTool

## Configure PixelConditionsSummarySvc
from PixelConditionsServices.PixelConditionsServicesConf import PixelConditionsSummarySvc
InDetPixelConditionsSummarySvc = PixelConditionsSummarySvc()
InDetPixelConditionsSummarySvc.UseSpecialPixelMap = False
InDetPixelConditionsSummarySvc.UseDCS             = False
InDetPixelConditionsSummarySvc.UseByteStream      = False
ServiceMgr += InDetPixelConditionsSummarySvc
print InDetPixelConditionsSummarySvc

from InDetPrepRawDataFormation.InDetPrepRawDataFormationConf import InDet__PixelClusterization
job += InDet__PixelClusterization("PixelClusterization")
print job.PixelClusterization

from SiSpacePointFormation.SiSpacePointFormationConf import InDet__SiTrackerSpacePointFinder
job += InDet__SiTrackerSpacePointFinder("PixelSpacePoints",
                                        ProcessSCTs = False)
print job.PixelSpacePoints

#
# Include PixelValidationNtuple 
# with some information about Geant4 hits
#
from InDetTrackValidation.InDetTrackValidationConf import InDet__PixelClusterValidationNtupleWriter
job += InDet__PixelClusterValidationNtupleWriter("PixelNtupleWriter",
                                                 NtupleFileName 	  = 'TRKVAL',
                                                 NtupleDirectoryName	  = 'Validation',
                                                 NtupleTreeName 	  = 'PixelRIOs',
                                                 PixelClusterContainer    = 'PixelClusters',
                                                 WriteDetailedPixelInformation = False,
                                                 DoHits 		  = True,
                                                 DoMC			  = True,
                                                 FindNotAssociatedParticle= False,
                                                 WriteNNTraining          = True,
                                                 # Extra flags ONLY ON PRIVATE InDetTrackValidation/PixelClusterValidationNtupleWriter
                                                 UseToT = True,
                                                 DetGeo = 'SLHC')
print job.PixelNtupleWriter

theApp.HistogramPersistency = 'ROOT'
if not 'OutputNTpl' in dir():
  OutputNTpl = OUTDIR+"TrkValidation_noTrack_StandAlone_.root"

# Root file definition
if not hasattr(ServiceMgr, 'THistSvc'):
  from GaudiSvc.GaudiSvcConf import THistSvc
  ServiceMgr += THistSvc()

ServiceMgr.THistSvc.Output += [ "TRKVAL DATAFILE='" + OutputNTpl + "' TYPE='ROOT' OPT='RECREATE'" ]
theApp.Dlls += [ 'RootHistCnv' ]

