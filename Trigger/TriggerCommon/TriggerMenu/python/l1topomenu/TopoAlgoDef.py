# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
log = logging.getLogger('TriggerMenu.L1Topo.py')

from TriggerMenu.l1topo.TopoAlgos import SortingAlgo, DecisionAlgo
from TriggerJobOpts.TriggerFlags import TriggerFlags

# algorithm python base classes generated from C++ code
import L1TopoAlgorithms.L1TopoAlgConfig as AlgConf
import L1TopoHardware.L1TopoHardware as HW

class TopoAlgoDef:

    @staticmethod
    def registerTopoAlgos(tm):

        # sorting algorithms

        alg = AlgConf.ClusterSortingAlg( name = 'ClusterSort_EM_NoIso', inputs = 'ClusterIn', outputs = 'SortedClusters_EM_NoIso', algoId = 0)
        alg.addgeneric('NumberOfClusters', HW.OutputWidthSortEM)
        alg.addvariable('MinIsolation', 0)
        alg.addvariable('MinEta', 0)
        alg.addvariable('MaxEta', 63)
        tm.registerAlgo(alg) 
        
        alg = AlgConf.JetSortingAlg( name = 'JetSort', inputs = 'JetIn', outputs = 'SortedJetsDefault', algoId = 1)
        alg.addgeneric('NumberOfJets', HW.OutputWidthSortJET)
        alg.addgeneric('JetSize', HW.DefaultJetSize)
        alg.addvariable('MinEta', 0)
        alg.addvariable('MaxEta', 63)
        tm.registerAlgo(alg)
        
        alg = AlgConf.JetSortingAlg( name = 'JetSort_Size2', inputs = 'JetIn', outputs = 'SortedJetsSize2', algoId = 2)
        alg.addgeneric('NumberOfJets', HW.OutputWidthSortJET)
        alg.addgeneric('JetSize', 1 if HW.DefaultJetSize==2 else 2)
        alg.addvariable('MinEta', 0)
        alg.addvariable('MaxEta', 63)
        tm.registerAlgo(alg) 


        # decision algorithms
        
        alg = AlgConf.DeltaPhiLeading( name = 'ClusterDeltaPhi_12', inputs = ['SortedClusters_EM_NoIso', 'SortedClusters_EM_NoIso'], outputs = ['ClusterDeltaPhi_Loose', 'ClusterDeltaPhi_Tight'], algoId = 0)
        alg.addgeneric('MaxTob', 6)
        alg.addvariable('MinET', 5)
        alg.addvariable('DeltaPhiMin', 12, 0)
        alg.addvariable('DeltaPhiMax', 31, 0)
        alg.addvariable('DeltaPhiMin', 22, 1)
        alg.addvariable('DeltaPhiMax', 31, 1)
        tm.registerAlgo(alg)


        alg = AlgConf.DeltaPhiIncl1( name = 'JetDefaultDeltaPhiIncl_12', inputs = ['SortedJetsDefault'], outputs = ['JetDeltaPhiNarrow','JetDeltaPhiWide'], algoId = 1)
        alg.addgeneric('NumberLeading1', 4)
        alg.addgeneric('NumberLeading2', 4)
        alg.addvariable('MinET', 5)
        alg.addvariable('DeltaPhiMin', 20, 0)
        alg.addvariable('DeltaPhiMax', 27, 0)
        alg.addvariable('DeltaPhiMin', 27, 1)
        alg.addvariable('DeltaPhiMax', 32, 1)
        tm.registerAlgo(alg)

        
        alg = AlgConf.DeltaEtaLeading( name = 'ClusterDeltaEta_12', inputs = ['SortedClusters_EM_NoIso', 'SortedJetsSize2'], outputs = ['ClusterDeltaEta_Loose', 'ClusterDeltaEta_Tight'], algoId = 2)
        alg.addgeneric('MaxTob', 6)
        alg.addvariable('MinET', 5)
        alg.addvariable('DeltaEtaMin', 12, 0)
        alg.addvariable('DeltaEtaMax', 31, 0)
        alg.addvariable('DeltaEtaMin', 22, 1)
        alg.addvariable('DeltaEtaMax', 31, 1)
        tm.registerAlgo(alg)

        

        
        
