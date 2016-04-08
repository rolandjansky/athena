##########################################################
# User Digitization Job Options to run FastCaloSim in digitization step and write FastShowerInfoContainers into the pool file

from FastCaloSim.FastCaloSimConf import FastShowerCellBuilderTool
FastShowerCellBuilderTool.StoreFastShowerInfo = True

from SimuJobTransforms.DigitConfig import digitConfig
digitConfig.postOptions = "FastCaloSim/writeFastShowerInfoContainer_at_AF2Digitization.py"

##########################################################
