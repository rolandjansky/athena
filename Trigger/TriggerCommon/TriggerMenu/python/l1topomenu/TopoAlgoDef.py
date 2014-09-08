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
        
        alg = AlgConf.ClusterSortingAlg( name = 'TauSort', inputs = 'TauIn', outputs = 'SortedTaus', algoId = 1)
        alg.addgeneric('NumberOfClusters', HW.OutputWidthSortEM)
        alg.addvariable('MinIsolation', 0)
        alg.addvariable('MinEta', 0)
        alg.addvariable('MaxEta', 63)
        tm.registerAlgo(alg) 
        
        alg = AlgConf.JetSortingAlg( name = 'JetSort', inputs = 'JetIn', outputs = 'SortedJets', algoId = 2)
        alg.addgeneric('NumberOfJets', HW.OutputWidthSortJET)
        alg.addgeneric('JetSize', HW.DefaultJetSize)
        alg.addvariable('MinEta', 0)
        alg.addvariable('MaxEta', 63)
        tm.registerAlgo(alg)
        
        alg = AlgConf.JetSortingAlg( name = 'JetSort_Size2', inputs = 'JetIn', outputs = 'SortedJetsSize2', algoId = 3)
        alg.addgeneric('NumberOfJets', HW.OutputWidthSortJET)
        alg.addgeneric('JetSize', 1 if HW.DefaultJetSize==2 else 2)
        alg.addvariable('MinEta', 0)
        alg.addvariable('MaxEta', 63)
        tm.registerAlgo(alg) 

        alg = AlgConf.METSortingAlg( name = 'METSort', inputs = 'JetIn', outputs = 'MET', algoId = 4)
        alg.addvariable('MinEta', 0)
        alg.addvariable('MaxEta', 31)
        tm.registerAlgo(alg) 
        # decision algorithms

        alg = AlgConf.GenericListGenerator( name = 'GenericFromJet', inputs = 'JetsIn', outputs = 'JetsIn', algoId = 5 )
        tm.registerAlgo(alg)

        currentAlgoId = 0

        alg = AlgConf.EtCut( name = 'J25', inputs = 'SortedJets', outputs = 'J25_topo', algoId = currentAlgoId ); currentAlgoId += 1
        alg.addvariable('MinET', 25)
        tm.registerAlgo(alg)

        alg = AlgConf.DeltaPhiIncl1( name = 'DPhi_JsJs', inputs = 'SortedJets', outputs = ['JetDeltaPhiNarrow'], algoId = currentAlgoId ); currentAlgoId += 1
        alg.addgeneric('NumberLeading1', HW.OutputWidthSortJET)
        alg.addgeneric('NumberLeading2', HW.OutputWidthSortJET)
        alg.addvariable('MinET', 0)
        alg.addvariable('DeltaPhiMin', 0, 0)
        alg.addvariable('DeltaPhiMax', 16, 0)
        tm.registerAlgo(alg)

        alg = AlgConf.JetHT( name = 'HT200', inputs = 'JetsIn', outputs = ['JetDeltaPhiNarrow'], algoId = currentAlgoId ); currentAlgoId += 1
        tm.registerAlgo(alg)

        alg = AlgConf.EtCut( name = 'EM25', inputs = 'SortedClusters', outputs = ['EM25_topo'], algoId = currentAlgoId ); currentAlgoId += 1
        alg.addvariable('MinET', 25)
        tm.registerAlgo(alg)

        alg = AlgConf.DeltaPhiIncl1( name = 'DPhi_EMsEMs', inputs = 'SortedClusters', outputs = ['EMDeltaPhiNarrow'], algoId = currentAlgoId ); currentAlgoId += 1
        alg.addgeneric('NumberLeading1', HW.OutputWidthSortEM)
        alg.addgeneric('NumberLeading2', HW.OutputWidthSortEM)
        alg.addvariable('MinET', 0)
        alg.addvariable('DeltaPhiMin', 0, 0)
        alg.addvariable('DeltaPhiMax', 16, 0)
        tm.registerAlgo(alg)

        alg = AlgConf.DeltaEtaIncl1( name = 'DEta_EMsEMs', inputs = 'SortedClusters', outputs = 'DEta_EMEM10', algoId = currentAlgoId ); currentAlgoId += 1
        alg.addgeneric('NumberLeading1', HW.OutputWidthSortEM)
        alg.addgeneric('NumberLeading2', HW.OutputWidthSortEM)
        alg.addvariable('MinET', 0)
        alg.addvariable('DeltaEtaMin', 0, 0)
        alg.addvariable('DeltaEtaMax', 10, 0)
        tm.registerAlgo(alg)

        alg = AlgConf.EtCut( name = 'TAU25', inputs = 'SortedTaus', outputs = ['TAU25_topo'], algoId = currentAlgoId ); currentAlgoId += 1
        alg.addvariable('MinET', 25)
        tm.registerAlgo(alg)
        
        alg = AlgConf.DeltaPhiIncl1( name = 'DPhi_TAUsTAUs', inputs = 'SortedTaus', outputs = ['DPhi_TAU16'], algoId = currentAlgoId ); currentAlgoId += 1
        alg.addgeneric('NumberLeading1', HW.OutputWidthSortTAU)
        alg.addgeneric('NumberLeading2', HW.OutputWidthSortTAU)
        alg.addvariable('MinET', 0)
        alg.addvariable('DeltaPhiMin', 0, 0)
        alg.addvariable('DeltaPhiMax', 16, 0)
        tm.registerAlgo(alg)

        alg = AlgConf.DeltaEtaIncl1( name = 'DEta_TAUTAU_10', inputs = 'SortedTaus', outputs = 'DEta_TAUTAU10', algoId = currentAlgoId ); currentAlgoId += 1
        alg.addgeneric('NumberLeading1', HW.OutputWidthSortEM)
        alg.addgeneric('NumberLeading2', HW.OutputWidthSortEM)
        alg.addvariable('MinET', 0)
        alg.addvariable('DeltaEtaMin', 0, 0)
        alg.addvariable('DeltaEtaMax', 10, 0)
        tm.registerAlgo(alg)


        alg = AlgConf.METCut( name = 'XE', inputs = 'MET', outputs = 'XE25_topo', algoId = currentAlgoId ); currentAlgoId += 1
        alg.addvariable('XE', 25)
        tm.registerAlgo(alg)



        
        
