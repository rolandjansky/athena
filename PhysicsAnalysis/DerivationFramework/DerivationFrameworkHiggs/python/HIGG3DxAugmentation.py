
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

    import PyUtils.AthFile as af
    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    f = af.fopen(athenaCommonFlags.PoolAODInput()[0])
    # Sherpa needs some special treatment
    isFromSherpa = True
    try:
        generator = f.fileinfos['metadata']['/TagInfo']['generators']
        logger.info("Generator name: %s" % generator)
        if "Powheg" in generator or "MadGraph" in generator or "McAtNlo" in generator: isFromSherpa = False
    except:
        try:
            logger.info("Trying to infer generator type from MC channel number...")
            # hopefully should never need to resort to this...
            mcChannelNumber = f.mc_channel_numbers[0]
            if mcChannelNumber in [410009]: isFromSherpa = False
            if mcChannelNumber in range(361700,361899): isFromSherpa = False
            if mcChannelNumber in range(361100,361110): isFromSherpa = False
            if mcChannelNumber in range(363600,363671): isFromSherpa = False
            if mcChannelNumber in range(363123,363170): isFromSherpa = False
            if mcChannelNumber in range(363725,363748): isFromSherpa = False
            if mcChannelNumber in range(361500,361534): isFromSherpa = False
            if mcChannelNumber in range(361628,361642): isFromSherpa = False
        except:
            logger.warning("THIS SAMPLE DOES NOT HAVE METADATA INFO .... assuming SHERPA!!!")
            pass
        pass

    toolName = toolNamePrefix+"TruthDecoratorTool"
    from DerivationFrameworkHiggs.DerivationFrameworkHiggsConf import DerivationFramework__HIGG3TruthDecorator
    HIGG3DTruthDecoratorTool = DerivationFramework__HIGG3TruthDecorator(name        = toolName,
                                                                        OutputLevel = VERBOSE,
                                                                        IsSherpa    = isFromSherpa,
                                                                        )
    logger.info("Created HIGG3TruthDecorator with name {}".format(toolName))
    return HIGG3DTruthDecoratorTool



