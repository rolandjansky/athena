#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

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

        elif self.exlusionset=='cool':
            # these things are not stored in COOL
            self.exclFromCmpList = ['Deadtime','MuctpiInfo','Random','PrescaledClock','Cable','PrioritySet', 'TriggerCounterList']
            self.ignoreAttr['LVL1Config'] = ['id', 'name', 'version']
            self.ignoreAttr['PrescaleSet'] = ['name', 'version']
            self.ignoreAttr['Prescale'] = ['n', 'm', 'd', 'N', 'M', 'D']
            self.ignoreAttr['TriggerMenu'] = ['name']
            self.ignoreAttr['TriggerItem'] = ['id', 'trigger_type', 'definition', 'comment']
            self.ignoreAttr['TriggerThreshold'] = ['id', 'bitnum']
            self.ignoreAttr['TriggerThresholdValue'] = ['version']
            self.ignoreAttr['BunchGroupSet'] = ['version', 'name']
            self.ignoreAttr['BunchGroup'] = ['version']
            self.ignoreAttr['CaloInfo'] = ['name', 'version', 'global_scale']

        elif self.exlusionset=='rtt':
            # these things are not stored in COOL
            self.exclFromCmpList = ['BunchGroupSet', 'CaloInfo', 'MuctpiInfo', 'PrescaledClock', 'Random', 'Cable', 'TriggerCounterList']
            self.ignoreAttr['TriggerItem'] = ['id', 'definition']
            self.ignoreAttr['TriggerThreshold'] = ['id', 'active']
            self.ignoreAttr['TriggerThresholdValue'] = ['version']
            self.ignoreAttr['PrioritySet'] = ['version']
            self.ignoreAttr['LVL1Config'] = ['version']

        elif self.exlusionset=='tt':
            # these things change when uploading with the TT and downloading with 2XMLApp
            self.exclFromCmpList = ['BunchGroupSet', 'PrioritySet']
            self.ignoreAttr['LVL1Config'] = ['id']
            self.ignoreAttr['CaloInfo'] = ['version']
            self.ignoreAttr['Deadtime'] = ['version']
            self.ignoreAttr['MuctpiInfo'] = ['version']
            self.ignoreAttr['TriggerItem'] = ['definition', 'trigger_type']
            self.ignoreAttr['PrescaledClock'] = ['version']

        else:
            print >>self, "Don't know about comparison environment %s. Should be nothing, 'cool' or 'rtt'" % self.exlusionset
            sys.exit(0)

        self.doc1 = self.parseFile(files[0])
        self.doc2 = self.parseFile(files[1])


    def diff(self):
        equal = super(CompareL1TopoXML,self).diff(self.doc1, self.doc2)
        print >> self, "L1 topo menus are%s equal" % ('' if equal else ' not')
        return ""
