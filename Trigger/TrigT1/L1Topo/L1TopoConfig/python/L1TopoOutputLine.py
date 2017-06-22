#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from L1TopoConfigUtils import *

class L1TopoOutputLine:

    def __init__(self, name, algoName, algoId, module, fpga, clock, bit):
        self.name = name
        self.algoName = algoName      
        self.algoId = algoId
        self.module = module
        self.fpga = fpga
        self.clock = clock
        self.bit = bit
        self.counter = 0
        self.triggerItems = []
        self._calCounter()


    def __str__(self):
        s = ""

        if (self.counter % 32) == 0:
            s += " \n "
            s += "Module {}, FPGA {} \n" .format(self.module, self.fpga) 
            s += "------------------------- \n"

        s +=  "{} : {} \t" .format(self.counter, self.name)
        s += " (algorithm {}, id = {})" .format(self.algoName, self.algoId)
        s += " on line {} and clock {} \n" .format(self.bit, self.clock) 
        for item in self.triggerItems:
            s += str(item)
        return s


    def _calCounter(self):
        self.counter =  2 * (32 * self.module + 16 * self.fpga + self.bit) + self.clock
        if self.counter > 191:
            print "Trigger item {} has illegal counter {} \n" .format(self.name, self.counter)



class TriggerItem:

    def __init__(self, ctpId, name):
        self.ctpId = ctpId
        self.name = name
        
    def __str__(self):
        return "\t Used by trigger item {} : name = {} \n" .format(self.ctpId, self.name) 
