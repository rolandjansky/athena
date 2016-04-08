
from AthenaCommon.GlobalFlags import globalflags
from FastSimulationConfig.FastSimulationFlags import jobproperties

jobproperties.FastSimulation.doFatrasID=True
jobproperties.FastSimulation.doFatrasMuon=True
jobproperties.FastSimulation.doParametrisedMuon=True

from FatrasExample.FatrasJobProperties import FatrasFlags
# set Muon Reco and ID reco on
FatrasFlags.RefittingID=False
FatrasFlags.ReconstructionID=True
# (MS)
FatrasFlags.RefittingMS=True
FatrasFlags.DigitizationMS=True
FatrasFlags.ReconstructionMS=True

# set the cluster merging parameters
from FatrasExample.FatrasClusterCreation import FatrasClusterCreationFlags
FatrasClusterCreationFlags.PixSimpleClusterMerge=True
FatrasClusterCreationFlags.PixClusterMergeSharedRDOs=True
FatrasClusterCreationFlags.PixClusterMergeAttachedRDOs=True
FatrasClusterCreationFlags.PixUseClusterMerger=True

from FatrasExample.FatrasKeys import FatrasKeyFlags
from FastCaloSim.FastCaloSimConf import FastShowerCellBuilderTool
theFastShowerCellBuilderTool=FastShowerCellBuilderTool()
theFastShowerCellBuilderTool.McLocation=FatrasKeyFlags.McEventCollection()
theFastShowerCellBuilderTool.OutputLevel = ERROR

# get a handle on the ServiceManager which holds all the services
# from AthenaCommon.AppMgr import ServiceMgr
# ServiceMgr.StoreGateSvc.Dump = True
