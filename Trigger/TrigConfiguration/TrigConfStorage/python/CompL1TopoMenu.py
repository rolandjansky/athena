#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from __future__ import print_function
import sys

from TrigConfStorage.CompareMenuXML import CompareMenuXML

class CompareL1TopoXML(CompareMenuXML):
    def __init__(self, files, **args):
        super(CompareL1TopoXML,self).__init__(**args)

        self.uniqID = {
            'TOPO_MENU' : 'single',
            'OutputList' : 'single',
            'OutputList.Output' : 'algname',
            'TopoConfig' : 'single',
            'SortAlgo' : 'algoId',
            'DecisionAlgo' : 'algoId',
            'Fixed' : 'single',
            'Variable' : 'single',
            'Generic' : 'name',
            'Input' : 'single',
            'Fixed.Output' : 'single',
            'Bit' : 'single',
            'Parameter' : 'pos',
            }
        self.needParentInfo = []
        self.checkText = []

        # configure comparison
        self.exclFromCmpList = []
        self.ignoreAttr = {}

        if self.exlusionset=='default':
            pass
        else:
            print("Don't know about comparison environment %s. Should be nothing, 'cool' or 'rtt'" % self.exlusionset, file=self)
            sys.exit(0)

        self.doc1 = self.parseFile(files[0])
        self.doc2 = self.parseFile(files[1])


    def diff(self):
        equal = super(CompareL1TopoXML,self).diff(self.doc1, self.doc2)
        print("L1 topo menus are%s equal" % ('' if equal else ' not'), file=self)
        return ""
