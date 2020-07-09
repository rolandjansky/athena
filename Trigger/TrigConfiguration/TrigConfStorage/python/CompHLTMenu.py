#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function
import sys

from TrigConfStorage.CompareMenuXML import CompareMenuXML

class CompareHLTXML(CompareMenuXML):
    def __init__(self, files, **args):
        super(CompareHLTXML,self).__init__(**args)

        self.uniqID = {
            'HLT_MENU' : 'single',
            'CHAIN_LIST' : 'single',
            'SEQUENCE_LIST' : 'single',
            'GROUP_LIST' : 'single',
            'SIGNATURE_LIST' : 'single',
            'STREAMTAG_LIST' : 'single',
            'TRIGGERTYPE_LIST' : 'single',
            'CHAIN' : 'chain_name',
            'GROUP' : 'name',
            'SIGNATURE' : 'signature_counter',
            'TRIGGERELEMENT' : 'te_name',
            'TRIGGERTYPE' : 'bit',
            'STREAMTAG' : 'stream',
            'SEQUENCE' : 'output',
            'PRESCALES' : 'single',
            }

        # elements that need to print information about the parent when printed
        self.needParentInfo = ['GROUP', 'GROUP_LIST', 'TRIGGERTYPE', 'TRIGGERTYPE_LIST']

        self.exclFromCmpList = []
        self.ignoreAttr = {}

        # elements for which the information is stored as #PCDATA content
        self.checkText = []

        ## configure comparison

        # depending on the the environment exclude elements from the comparison
        if self.exlusionset=='default':
            self.exclFromCmpList = ['PRESCALES']
            self.ignoreAttr = {'CHAIN' : ['EBstep', 'rerun_prescale','pass_through']}

        elif self.exlusionset=='cool':
            # these things are not stored in COOL
            self.exclFromCmpList = ['SEQUENCE']
            self.ignoreAttr['STREAMTAG'] = ['obeyLB']
            self.ignoreAttr['SEQUENCE'] = ['algorithm']

        elif self.exlusionset=='rtt':
            # these things are not stored in COOL
            self.exclFromCmpList = ['SEQUENCE']
            self.ignoreAttr['HLT_MENU'] = ['id']
            self.ignoreAttr['CHAIN'] = ['rerun_prescale']

        elif self.exlusionset=='tt':
            # these things change when uploading with the TT and downloading with 2XMLApp
            self.exclFromCmpList = []

        else:
            print("Don't know about comparison environment %s. Should be nothing, 'cool' or 'rtt'" % self.exlusionset, file=self)
            sys.exit(0)
            
        # parse the two files and create dom-docs
        self.doc1 = self.parseFile(files[0])
        self.doc2 = self.parseFile(files[1])


    def diff(self):
        equal = super(CompareHLTXML,self).diff(self.doc1, self.doc2)
        print("HLT menus are%s equal" % ('' if equal else ' not'), file=self)
        return "" #self.textoutput

