#
# This is a simple test jobO for the xAODMaker::DynVarFixerAlg algorithm.
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
    CfgMgr.xAODMaker__DynVarFixerAlg( "AODFix_DynAuxVariables",
                                      Containers = [
                                          "CombinedMuonTrackParticlesAux.",
                                          "BTagging_AntiKt4EMTopoAux.",
                                          "MuonsAux." ],
                                      OutputLevel = DEBUG )

# Some final tweaking:
theApp.EvtMax = -1
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 100000000
