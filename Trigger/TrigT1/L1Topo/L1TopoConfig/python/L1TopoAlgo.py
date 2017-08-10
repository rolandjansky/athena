#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from L1TopoConfigUtils import *

class L1TopoAlgo:

    def __init__(self, toId, algoName, algoId, algoType, algoKind):
        self.id = toId
        self.name = algoName
        self.algoId = algoId
        self.type = algoType 
        self.kind = algoKind
        self.inputs = []
        self.outputs = []
        self.fixedParameters = []
        self.registerParameters = []


    def __str__(self):       
        s = " \n "
        s += "{} algorithm {} : {}/{} \n" .format(self.kind, self.algoId, self.type, self.name)
        for el in self.inputs:
            s += str(el)
        for el in self.outputs:
            s += el.__str__(self.kind)
        s += "\t Fixed parameters: %i \n" % len(self.fixedParameters)  
        for el in self.fixedParameters:
            s += str(el)
        s += "\t Register parameters: %i \n" % len(self.registerParameters)  
        for el in self.registerParameters:
            s += str(el)
        return s


class AlgoInput:

    def __init__(self, name, value, position):
        self.name = name
        self.value = value
        self.position = position
        
    def __str__(self):
        if self.position >= 0:
            return "\t Input {} : {}, name = {} \n" .format(self.position, self.value, self.name) 
        else:
            return "\t Input : {}, name = {} \n" .format(self.value, self.name) 

class AlgoOutput:

    def __init__(self, name, value, selection, bitname):
        self.name = name
        self.value = value
        self.selection = selection
        self.bitname = bitname
        
    def __str__(self, kind=None):
        if kind == "Sort":
            return "\t Output : {} \n" .format(self.value) 
        elif kind == "Decision":
            return "\t Output {} : {} \n" .format(self.selection, self.bitname) 
        else :
            return "\t Output value : {}, selection : {}, bitname : {} \n" .format(self.value, self.selection, self.bitname) 
          
        
class AlgoFixedParameter:

    def __init__(self, name, value):
        self.name = name
        self.value = value
              
    def __str__(self):
        return "\t \t  {} \t : {} \n" .format(self.name, self.value) 
    
       
class AlgoRegisterParameter:

    def __init__(self, name, value, position, selection):
        self.name = name
        self.value = value
        self.position = position
        self.selection = selection

    def __str__(self):
        if self.selection >= 0:
            return "\t \t output {} : {} \t = {}\n" .format(self.selection, self.name, self.value) 
        else:
            return "\t \t {} \t : {} \n" .format(self.name, self.value) 
