# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
##########################################################
# User Digitization Job Options

from SimuJobTransforms.DigitConfig import digitConfig
digitConfig.indetNoise = True
digitConfig.caloNoise  = True
digitConfig.muonNoise  = True

digitConfig.postOptions = "FastCaloSimHit/AF2Digitization.py"

##########################################################
