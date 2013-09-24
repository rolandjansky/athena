##########################################################
# User Digitization Job Options

from SimuJobTransforms.DigitConfig import digitConfig
digitConfig.indetNoise = True
digitConfig.caloNoise  = True
digitConfig.muonNoise  = True

digitConfig.postOptions = "FastCaloSimHit/AF2Digitization.py"

##########################################################
