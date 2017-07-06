#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from operator import attrgetter

from L1TopoConfigUtils import *
from L1TopoOutputLine import *
from L1TopoAlgo import *

class L1TopoMenu:

    def __init__(self, connection, smk):
        self._connection = connection  
        self._smk = 0 
        self.version = ""
        self.name = ""
        self.id = 0
        self.algorithms = []
        self.outputLines = []   

        # connection to the trigger DB
        self._cursor, self._schemaname = getTriggerDBCursor(self._connection)

        # Load the L1 Topo menu corresponding to the SMK
        self._set_smk(smk)
        
    def __str__(self):
        s =  "SMK: {} \n" .format(self.smk)
        s += "L1Topo menu version : {}, name : {} , id : {} \n" .format(self.version, self.name, self.id)
        s += "L1Topo algorithms : %i\n" % len(self.algorithms)
        s += "L1Topo Items   : %i\n" % len(self.outputLines)
        return s


    def _set_smk(self, smk):
        self._smk = smk
        
        # get L1 topo menu
        self.version, self.name, self.id = getL1TopoMenu(self._cursor, self._schemaname, self._smk) 
        
        # get output lines
        lines = getL1TopoOutputLines(self._cursor, self._schemaname, self.id) 
        for line in lines:
            self.outputLines.append(L1TopoOutputLine(*line))
        # get the trigger items using the L1Topo output lines    
        for line in self.outputLines:
            items = getL1TopoTriggerItems(self._cursor, self._schemaname, self._smk, line.name)
            for item in items:
                line.triggerItems.append(TriggerItem(*item))
            line.triggerItems = sorted(line.triggerItems, key=attrgetter("ctpId"))
        self.outputLines = sorted(self.outputLines, key=attrgetter("counter"))
        

        # get algorithms
        algos = getL1TopoAlgorithms(self._cursor, self._schemaname, self.id) 
        for algo in algos:
            self.algorithms.append(L1TopoAlgo(*algo))
        self.algorithms = sorted(self.algorithms, key=attrgetter("kind", "algoId")) 

        # get algorithms inputs, outputs and parameters
        for algo in self.algorithms:

            inputs = getL1TopoAlgoInputs(self._cursor, self._schemaname, algo.id) 
            for el in inputs:
                algo.inputs.append(AlgoInput(*el))
            algo.inputs = sorted(algo.inputs, key=attrgetter("position")) 
            
            outputs = getL1TopoAlgoOutputs(self._cursor, self._schemaname, algo.id) 
            for el in outputs:
                algo.outputs.append(AlgoOutput(*el))
            algo.outputs = sorted(algo.outputs, key=attrgetter("selection")) 
            
            fixedParams = getL1TopoAlgoFixedParameters(self._cursor, self._schemaname, algo.id) 
            for el in fixedParams:
                algo.fixedParameters.append(AlgoFixedParameter(*el))
        
            RegisterParams = getL1TopoAlgoRegisterParameters(self._cursor, self._schemaname, algo.id) 
            for el in RegisterParams:
                algo.registerParameters.append(AlgoRegisterParameter(*el))
            algo.registerParameters = sorted(algo.registerParameters, key=attrgetter("position")) 


    def _get_smk(self):
        return self._smk

    smk = property(_get_smk, _set_smk)

