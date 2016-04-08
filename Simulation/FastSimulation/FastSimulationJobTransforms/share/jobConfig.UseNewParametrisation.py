#################################################################
# Simulation Job Options to use new parametrization file 
from FastCaloSim.FastCaloSimConf import FastShowerCellBuilderTool
theFastShowerCellBuilderTool=FastShowerCellBuilderTool()

theFastShowerCellBuilderTool.ParticleParametrizationFileName="ParticleParametrization_9.3.2010.root"
#theFastShowerCellBuilderTool.OutputLevel=DEBUG

#################################################################

include("FastSimulationJobTransforms/jobConfig.v14_Parametrisation.py")
