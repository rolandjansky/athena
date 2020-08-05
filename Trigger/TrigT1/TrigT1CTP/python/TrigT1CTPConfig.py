# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from TrigT1CTP.TrigT1CTPConf import LVL1CTP__CTPSimulation

class DefaultCTPSimulation(LVL1CTP__CTPSimulation):

    def __init__(self, name = "DefaultCTPSimulation"):
        super( DefaultCTPSimulation, self ).__init__( name )

        from AthenaCommon.Logging import logging
        self.log = logging.getLogger( 'CTPSimulation' )

        self.AthenaMonTools = []

    def setDefaultRandomService(self, rndmSvc = 'AtRanluxGenSvc'):
        # Random service is not needed anymore in Run 3 for CTP
        pass
        
class CTPSimulation(DefaultCTPSimulation):
    __slots__ = []
    
    def __init__(self, name = "CTPSimulation"):
        super( CTPSimulation, self ).__init__( name )

        self.setDefaultRandomService()
        self.DoRNDM=True
        self.DoPSCL=True
        
    def setDefaults(self, handle):
        pass

class CTPSimulationOnData(DefaultCTPSimulation):
    __slots__ = []
    
    def __init__(self, name = "CTPSimulation"):
        super( CTPSimulationOnData, self ).__init__( name )

        self.setDefaultRandomService()
        self.IsData = True 
        self.PrescaleMode = 2
        self.DoBPTX=True
        self.DoNIM=True
        self.DoRNDM=True
        self.DoPSCL=False
                
    def setDefaults(self, handle):
        pass

class CTPSimulationInReco(DefaultCTPSimulation):
    __slots__ = []
    
    def __init__(self, name = "CTPSimulation"):
        super( CTPSimulationInReco, self ).__init__( name )

        self.setDefaultRandomService()
        self.DoRNDM=True
        self.DoPSCL=True
        self.DoNIM=True
        
        # add validation histograms
        from TrigT1CTP.TrigT1CTPMonitoring import TrigT1CTPValidationMonitoring
        self.AthenaMonTools += [ TrigT1CTPValidationMonitoring() ]

        from TriggerJobOpts.TriggerFlags import TriggerFlags
        if "Log" in  TriggerFlags.enableMonitoring():
            from TriggerJobOpts.Lvl1TriggerGetter import getLvl1OutputLevel
            self.OutputLevel=getLvl1OutputLevel()
            
    def setDefaults(self, handle):
        from TriggerJobOpts.TriggerFlags import TriggerFlags
        handle.DoCalo  = TriggerFlags.doCalo()
        handle.DoMuon  = TriggerFlags.doMuon()
        handle.DoBCM   = TriggerFlags.doBcm()
        handle.DoLUCID = TriggerFlags.doLucid()
        handle.DoZDC   = TriggerFlags.doZdc()
            
class CTPSimulationInDigi(DefaultCTPSimulation):
    __slots__ = []
    
    def __init__(self, name = "CTPSimulation"):
        super( CTPSimulationInDigi, self ).__init__( name )

        from AthenaCommon.Logging import logging
        log = logging.getLogger( 'CTPSimulation' )

        from Digitization.DigitizationFlags import jobproperties

        if hasattr( jobproperties.Digitization, 'rndmSvc' ):
            log.info("will take RndmSvc from jobproperties.Digitization")

            self.RndmSvc = jobproperties.Digitization.rndmSvc()
            jobproperties.Digitization.rndmSeedList.addSeed(self.RndmEngine, 1979283043, 1924452189)
        else:
            self.setDefaultRandomService()

    def setDefaults(self, handle):
        DefaultCTPSimulation.setDefaults(handle)
        handle.DoBPTX=False
        handle.DoNIM=True
        handle.DoRNDM=True
        handle.DoPSCL=True
        #handle.TrigConfigSvc="TrigConf::LVL1ConfigSvc/LVL1ConfigSvc"
        
class CTPSimulationInOverlay(DefaultCTPSimulation):
    __slots__ = []
    
    def __init__(self, name = "CTPSimulation2", OverlayCTPRandomEngineName = "CTPSimulation2"):
        super( CTPSimulationInOverlay, self ).__init__( name )

        from AthenaCommon.Logging import logging
        log = logging.getLogger( 'CTPSimulation2' )

        self.RndmEngine = OverlayCTPRandomEngineName
        from Digitization.DigitizationFlags import jobproperties

        if hasattr( jobproperties.Digitization, 'rndmSvc' ):
            log.info("will take RndmSvc from jobproperties.Digitization")

            self.RndmSvc = jobproperties.Digitization.rndmSvc()
            jobproperties.Digitization.rndmSeedList.addSeed(self.RndmEngine, 1979283043, 1924452189)
        else:
            self.setDefaultRandomService()

    def setDefaults(self, handle):
        DefaultCTPSimulation.setDefaults(handle)
        #handle.TrigConfigSvc="TrigConf::LVL1ConfigSvc/LVL1ConfigSvc"
