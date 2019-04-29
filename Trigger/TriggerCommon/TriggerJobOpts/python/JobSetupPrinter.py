from RecExConfig.Configured import Configured 

class JobSetupPrinter(Configured):
    """This file is only temporary to configure different outputs that are needed to
    debug the Run 3 CTPEmulation and HLT seeding
    """


    def __init__(self):
        from AthenaCommon.AlgSequence import AlgSequence 
        from AthenaCommon.Constants import DEBUG

        topSequence = AlgSequence()
        topSequence.TrigSteer_HLT.LvlConverterTool.OutputLevel = DEBUG
        topSequence.TrigSteer_HLT.LvlConverterTool.Lvl1ResultAccessTool.OutputLevel = DEBUG

        print topSequence.CTPEmulation
        print topSequence.RoIBuilder
        print topSequence.TrigSteer_HLT.LvlConverterTool

