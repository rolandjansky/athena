#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from __future__ import print_function
import sys

from TrigConfStorage.CompareMenuXML import CompareMenuXML

class CompareL1XML(CompareMenuXML):
    def __init__(self, files, **args):
        super(CompareL1XML,self).__init__(**args)

        self.uniqID = {
            'LVL1Config' : 'single',
            'PrescaleSet' : 'single',
            'BunchGroupSet' : 'single',
            'BunchGroup' : 'internalNumber',
            'Bunch' : 'bunchNumber',
            'CaloInfo' : 'single',
            'Isolation' : 'single',
            'Parametrization' : 'single',
            'MinimumTOBPt' : 'single',
            'METSignificance' : 'single',
            'TriggerCounter' : 'single',
            'MuctpiInfo' : 'single',
            'PrescaledClock' : 'single',
            'Random' : 'single',
            'Cable' : 'single',
            'Signal' : 'single',
            'TriggerMenu' : 'single',
            'TriggerThresholdList' : 'single',
            'TriggerItem' : 'name',
            'InternalTrigger' : 'name',
            'TriggerCondition' : 'name',
            'TriggerThreshold' : 'name',
            'TriggerThresholdValue' : 'name',
            'TriggerCounterList' : 'single',
            'Prescale' : 'ctpid',
            'NOT' : 'single',
            'OR' : 'single',
            'AND' : 'single',
            }
        self.needParentInfo = ['NOT', 'OR', 'AND', 'InternalTrigger','Bunch']
        self.checkText = ['Prescale']
        self.ignoreAttr = {}

        self.needParentInfo = []
        self.checkText = []
        # configure comparison
        self.exclFromCmpList = []

        if self.exlusionset=='default':
            self.exclFromCmpList = ['BunchGroupSet','CaloInfo', 'Deadtime', 'MuctpiInfo', 'PrescaledClock', 'Random', 'Cable']
            self.ignoreAttr['TriggerItem'] = ['definition']
            self.ignoreAttr['TriggerThreshold'] = ['id','version']

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
            print("Don't know about comparison environment %s. Should be nothing, 'cool' or 'rtt'" % self.exlusionset, file=self)
            sys.exit(0)

        self.doc1 = self.parseFile(files[0])
        self.doc2 = self.parseFile(files[1])


    def diff(self):
        equal = super(CompareL1XML,self).diff(self.doc1, self.doc2)
        print("LVL1 menus are%s equal" % ('' if equal else ' not'), file=self)
        return ""
