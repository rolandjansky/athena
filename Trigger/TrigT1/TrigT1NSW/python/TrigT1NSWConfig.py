# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from TrigT1NSW.TrigT1NSWConf import *

class DefaultNSWL1Simulation(NSWL1__NSWL1Simulation):
    __slots__ = []
    
    def __init__(self, name = "DefaultNSWL1Simulation"):
        super( DefaultNSWL1Simulation, self ).__init__( name )

        from AthenaCommon.Logging import logging
        log = logging.getLogger( 'NSWL1Simulation' )
 
        
        # set steering properties
        self.DoOffline      = True     # so far only offline simulation is available

    def setDefaults(self, handle):
        # so far no additional defaults
        pass
        
        
class NSWL1Simulation(DefaultNSWL1Simulation):
    __slots__ = []
    
    def __init__(self, name = "NSWL1Simulation"):
        super( NSWL1Simulation, self ).__init__( name )

        # add validation histograms 
        from TrigT1NSW.TrigT1NSWMonitoring import TrigT1NSWValidationMonitoring
        self.AthenaMonTools += [ TrigT1NSWValidationMonitoring() ]


    def setDefaults(self, handle):
        #DefaultNSWL1Simulation.setDefault(handle)
        pass
