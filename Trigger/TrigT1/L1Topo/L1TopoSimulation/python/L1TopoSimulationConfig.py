# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from L1TopoSimulation.L1TopoSimulationConf import LVL1__L1TopoSimulation

class L1TopoSimulation ( LVL1__L1TopoSimulation ):

    def __init__( self, name = "L1TopoSimulation" ):
        super( L1TopoSimulation, self ).__init__( name )

        from L1TopoSimulation.L1TopoSimulationMonitoring import L1TopoSimulationMonitoring
        self.AthenaMonTools += [ L1TopoSimulationMonitoring() ]
