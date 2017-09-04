#
# This is a simple test jobO for the xAODMaker::BTaggingAuxFixerAlg algorithm.
#

# Read a file called AOD.pool.root:
from AthenaCommon.JobProperties import jobproperties as jp
jp.AthenaCommonFlags.FilesInput = [ "AOD.pool.root" ]
import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = jp.AthenaCommonFlags.FilesInput()

# Set up the main algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

# Schedule the TrackLinkFixerAlg algorithm:
theJob += \
    CfgMgr.xAODMaker__BTaggingAuxFixerAlg( "AODFix_BTagging_AntiKt4EMTopo",
                                           Containers = [
                                              "BTagging_AntiKt4EMTopoAux." ],
                                           OutputLevel = VERBOSE )

# Some final tweaking:
theApp.EvtMax = -1
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 100000000
