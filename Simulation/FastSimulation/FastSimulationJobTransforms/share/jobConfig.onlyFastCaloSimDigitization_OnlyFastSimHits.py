##########################################################
# User Digitization Job Options to run FastCaloSim in digitization step

from SimuJobTransforms.DigitConfig import digitConfig
digitConfig.postOptions = "FastCaloSimHit/AF2Digitization_OnlyFastSimHits.py"

##########################################################
