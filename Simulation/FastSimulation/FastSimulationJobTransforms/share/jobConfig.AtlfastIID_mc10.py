
print "Running jobConfig.AtlfastIID.py"

#from FastCaloSim.FastCaloSimConf import FastShowerCellBuilderTool
#theFastShowerCellBuilderTool=FastShowerCellBuilderTool()

#include("FastSimulationJobTransforms/jobConfig.onlyFastCaloSimDigitization.py")
include("FastCaloSimHit/AF2Digitization.py")
include("FastSimulationJobTransforms/jobConfig.v14_Parametrisation.py")
include("FastSimulationJobTransforms/jobConfig.FastCaloSim_ID_cuts.py")

#theFastShowerCellBuilderTool.OutputLevel=DEBUG
