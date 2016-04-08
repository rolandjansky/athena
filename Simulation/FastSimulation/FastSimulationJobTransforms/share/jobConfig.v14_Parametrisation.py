#################################################################
# Simulation Job Options to use new parametrization file 
from FastCaloSim.FastCaloSimConf import FastShowerCellBuilderTool
theFastShowerCellBuilderTool=FastShowerCellBuilderTool()

theFastShowerCellBuilderTool.DoNewEnergyEtaSelection=True
theFastShowerCellBuilderTool.DoEnergyInterpolation=True
theFastShowerCellBuilderTool.use_Ekin_for_depositions=True

# MISSING: setup for Database access to new parametrization !

include("FastSimulationJobTransforms/jobConfig.FastCaloSim_DB.py")

from IOVDbSvc.CondDB import conddb
conddb.addFolder('GLOBAL_OFL','/GLOBAL/AtlfastII/FastCaloSimParam <tag>FastCaloSim_v2</tag>')

#################################################################
