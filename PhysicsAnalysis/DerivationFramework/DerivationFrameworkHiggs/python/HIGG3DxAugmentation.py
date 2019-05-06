
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

#############################################
# Common code used for HIGG3DX augmentation #
# D.Shope                                   #
# david.richard.shope@cern.ch               #
# May 2019                                  #
#############################################

def getHIGG3TruthDecoratorTool(toolNamePrefix = "HIGG3DX"):
    from DerivationFrameworkHiggs.DerivationFrameworkHiggsConf import DerivationFramework__HIGG3TruthDecorator
    HIGG3DTruthDecoratorTool = DerivationFramework__HIGG3TruthDecorator(name = toolNamePrefix+"TruthDecoratorTool",
                                                                         OutputLevel       = VERBOSE,
                                                                         )
    return HIGG3DTruthDecoratorTool



