
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

#############################################
# Common code used for HIGG3DX augmentation #
# D.Shope                                   #
# david.richard.shope@cern.ch               #
# May 2019                                  #
#############################################

from AthenaCommon.Logging import logging
logger = logging.getLogger( 'HIGG3DxAugmentation' )

# for debugging output
from AthenaCommon.Constants import INFO,WARNING,DEBUG,VERBOSE

def getHIGG3TruthDecoratorTool(toolNamePrefix = "HIGG3DX"):
    toolName = toolNamePrefix+"TruthDecoratorTool"
    from DerivationFrameworkHiggs.DerivationFrameworkHiggsConf import DerivationFramework__HIGG3TruthDecorator
    HIGG3DTruthDecoratorTool = DerivationFramework__HIGG3TruthDecorator(name        = toolName,
                                                                        OutputLevel = VERBOSE,
                                                                        )
    logger.info("Created HIGG3TruthDecorator with name {}".format(toolName))
    return HIGG3DTruthDecoratorTool



