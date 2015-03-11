###############################################################################
# jobOptions_SLHC_nn_prodTrainingSample.py
#
# script that reads a series of simulated HIT files, runs digitization and
# clusterization and produces the Ntuples needed to train the cluster splitting
# neuronal network.
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
# Note 2: This jobOptions are based on InDetSLHCExample options. There there
#         is also a stand alone .py file in this dir. 
#
# Author: Tiago Perez <tperez@cern.ch>
# Date:   9-Jan-2012
##############################################################################
#--------------------------------------------------------------
# Template jobOptions: SLHC 
#  - Digitization
#--------------------------------------------------------------

from AthenaCommon.GlobalFlags import globalflags
globalflags.ConditionsTag = "OFLCOND-SDR-BS14T-ATLAS-00"

include("InDetSLHC_Example/preInclude.SLHC.py")

include("InDetSLHC_Example/preInclude.SiliconOnly.py")

from AthenaCommon.AthenaCommonFlags import jobproperties
jobproperties.AthenaCommonFlags.EvtMax=-1
#
## Input data
DATADIR="root://eosatlas.cern.ch//eos/atlas/user/t/tperez/"
#
## MinBias
#FILEPATH+="mc11_slhcid.108119.Pythia8_minbias_Inelastic_high.merge.HITS.e876_s1333_s1335_tid514272_00/"
#FILEPATH+="HITS.514272._000030.pool.root.1"
#
## ttbar
FILEPATH=DATADIR+"mc11_slhcid.105568.ttbar_Pythia.simul.HITS.e842_s1333_tid510282_00/"
FILEPATH+="HITS.510282._000429.pool.root.1"
#
#
jobproperties.AthenaCommonFlags.PoolHitsInput=[FILEPATH]
jobproperties.AthenaCommonFlags.PoolRDOOutput=DATADIR+"ttbar.digit.RDO.pool.root"

from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion='ATLAS-SLHC-01-00-00'

from Digitization.DigitizationFlags import jobproperties 
jobproperties.Digitization.doInDetNoise=False

include ( "Digitization/Digitization.py" )

include("InDetSLHC_Example/postInclude.SLHC_Digitization.py")

#
# Start clusterization
#
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
  OutputNTpl = "TrkValidation_noTrack_ttbar_.root"

# Root file definition
if not hasattr(ServiceMgr, 'THistSvc'):
  from GaudiSvc.GaudiSvcConf import THistSvc
  ServiceMgr += THistSvc()

ServiceMgr.THistSvc.Output += [ "TRKVAL DATAFILE='" + OutputNTpl + "' TYPE='ROOT' OPT='RECREATE'" ]
theApp.Dlls += [ 'RootHistCnv' ]
#
#
#

MessageSvc = Service( "MessageSvc" )
#increase the number of letter reserved to the alg/tool name from 18 to 30
MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M"
# to change the default limit on number of message per alg
MessageSvc.defaultLimit = 9999999  # all messages
# Set output level threshold among DEBUG, INFO, WARNING, ERROR, FATAL 
MessageSvc.OutputLevel = INFO

include("InDetSLHC_Example/postInclude.SLHC_Setup.py")
