# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from TrigT1CTP.TrigT1CTPConf import *

class DefaultCTPSimulation(LVL1CTP__CTPSimulation):
    __slots__ = []
    
    def __init__(self, name = "DefaultCTPSimulation"):
        super( DefaultCTPSimulation, self ).__init__( name )

        from AthenaCommon.Logging import logging
        log = logging.getLogger( 'CTPSimulation' )
 
        
        # set properties
        self.PrescaleMode       = 2         # random prescales
        self.UseDeadTime        = False     # no dead time simulation
        self.IsData             = False     # default is to run on MC file

        self.RndmEngine = 'CTPSimulation'
        
        ### this should not be done at all, since it instantiates a config svc without proper configuration!!
        ### LVL1ConfigSvc
        ##from AthenaCommon.AppMgr import ServiceMgr
        ##if not hasattr( ServiceMgr, 'LVL1ConfigSvc' ):
        ##    log.info("will setup LVL1ConfigSvc and add instance to ServiceMgr")
        ##    from TrigConfigSvc.TrigConfigSvcConfig import LVL1ConfigSvc
        ##    LVL1ConfigSvc = LVL1ConfigSvc('LVL1ConfigSvc')
        ##    ServiceMgr += LVL1ConfigSvc

    def setDefaults(self, handle):
        from AthenaCommon.DetFlags import DetFlags
        handle.DoCalo  = DetFlags.simulateLVL1.Calo_on()
        handle.DoMuon  = DetFlags.simulateLVL1.Muon_on()
        handle.DoBCM   = DetFlags.simulateLVL1.BCM_on()
        handle.DoLUCID = DetFlags.simulateLVL1.Lucid_on()
        handle.DoZDC   = DetFlags.simulateLVL1.ZDC_on()
        
    def setDefaultRandomService(self, rndmSvc = 'AtRanluxGenSvc'):
        from AthenaCommon.Logging import logging
        log = logging.getLogger( 'CTPSimulation' )

        # instruct CTP simulation to use given random generator service
        self.RndmSvc = rndmSvc

        # set up random generator service if not done before
        from AthenaCommon.AppMgr import ServiceMgr
        if not hasattr( ServiceMgr, rndmSvc):
            log.debug( 'adding %s RndmSvc to ServiceMgr' % rndmSvc )
            
            from AthenaServices.AthenaServicesConf import  AtRndmGenSvc, AtRanluxGenSvc
            randomSvc = eval(rndmSvc)(rndmSvc)
            ServiceMgr += randomSvc

        # init random number seeds
        log.info( 'will setup seeds for RndmSvc %s' % rndmSvc )
        getattr(ServiceMgr, rndmSvc).Seeds += [ str(self.RndmEngine) + " 1979283043 1924452189" ]
        
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
