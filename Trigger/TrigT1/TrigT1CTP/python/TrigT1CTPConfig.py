# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from TrigT1CTP.TrigT1CTPConf import LVL1CTP__CTPSimulation

class CTPSimulation(LVL1CTP__CTPSimulation):

    def __init__(self, name = "CTPSimulation"):
        super( CTPSimulation, self ).__init__( name )

        from AthenaCommon.Logging import logging
        self.log = logging.getLogger( 'CTPSimulation' )


class CTPSimulationOnData(CTPSimulation):
    __slots__ = []
    
    def __init__(self, name = "CTPSimulation"):
        super( CTPSimulationOnData, self ).__init__( name )

        self.IsData = True
        self.ForceBunchGroupPattern = False # on data we will take the bunchgroups from COOL
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        if hasattr(svcMgr,'DSConfigSvc'):
            # this case is still needed for reading Run 2 data configuration from the TriggerDB
            self.TrigConfigSvc = "TrigConfigSvc"


class CTPSimulationInReco(CTPSimulation):
    __slots__ = []
    
    def __init__(self, name = "CTPSimulation"):
        super( CTPSimulationInReco, self ).__init__( name )


class CTPSimulationInDigi(CTPSimulation):
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


class CTPSimulationInOverlay(CTPSimulation):
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

