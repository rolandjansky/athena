from RecExConfig.Configured import Configured 

class JobSetupPrinter(Configured):
    """This file is only temporary to configure different outputs that are needed to
    debug the Run 3 CTPEmulation and HLT seeding
    """


    def __init__(self):
        from AthenaCommon.AlgSequence import AlgSequence 
        from AthenaCommon.Constants import DEBUG

        from TrigT1CaloFexSim.L1SimulationControlFlags import L1Phase1SimFlags as simflags

        topSequence = AlgSequence()
        if simflags.EnableDebugOutput():
            if hasattr(topSequence,"CTPEmulation"):
                topSequence.CTPEmulation.OutputLevel = DEBUG

            if hasattr(topSequence,"TrigSteer_HLT"):
                topSequence.TrigSteer_HLT.LvlConverterTool.OutputLevel = DEBUG
                topSequence.TrigSteer_HLT.LvlConverterTool.Lvl1ResultAccessTool.OutputLevel = DEBUG
            from AthenaCommon.AppMgr import ServiceMgr as svcMgr
            svcMgr.MessageSvc.debugLimit = 100000

        if hasattr(topSequence,"L1Muctpi_on_RDO"):
            print topSequence.L1Muctpi_on_RDO

        if hasattr(topSequence,"CTPEmulation"):
            print topSequence.CTPEmulation

        if hasattr(topSequence,"RoIBuilder"):
            print topSequence.RoIBuilder
        if hasattr(topSequence,"TrigSteer_HLT"):
            print topSequence.TrigSteer_HLT.LvlConverterTool

