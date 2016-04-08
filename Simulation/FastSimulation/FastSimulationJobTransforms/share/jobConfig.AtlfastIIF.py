from AthenaCommon.GlobalFlags import globalflags
from FastSimulationConfig.FastSimulationFlags import jobproperties

# include the Fast Calo Sim reconstruction setup
jobproperties.FastSimulation.doFastCaloSim = True

include("FastSimulationJobTransforms/jobConfig.v14_Parametrisation.py")

jobproperties.FastSimulation.doFatrasID=True
jobproperties.FastSimulation.doFatrasMuon=True
jobproperties.FastSimulation.doParametrisedMuon=False

from FatrasExample.FatrasJobProperties import FatrasFlags
#FatrasFlags.FastCaloSim is NOT needed when running in Reco_trf: FastCaloSim is naturally integrated in the calorec part
#FatrasFlags.FastCaloSim = True

# set Muon Reco and ID reco on
FatrasFlags.RefittingID=False
FatrasFlags.ReconstructionID=True
FatrasFlags.EventPreProcessing=True
# (MS)
FatrasFlags.RefittingMS=False
FatrasFlags.DigitizationMS=True
FatrasFlags.ReconstructionMS=True

from FatrasExample.FatrasKeys import FatrasKeyFlags
FatrasKeyFlags.InputMcEventCollection = 'GEN_EVENT'

from FatrasExample.FatrasKeys import FatrasKeyFlags
from FastCaloSim.FastCaloSimConf import FastShowerCellBuilderTool
theFastShowerCellBuilderTool=FastShowerCellBuilderTool()
theFastShowerCellBuilderTool.McLocation=FatrasKeyFlags.McEventCollection()
#theFastShowerCellBuilderTool.OutputLevel = ERROR

rec.abortOnUncheckedStatusCode=False
