#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from __future__ import print_function
import sys

from TrigConfStorage.CompareMenuXML import CompareMenuXML

class CompareSetupXML(CompareMenuXML):
    def __init__(self, files, **args):
        super(CompareSetupXML,self).__init__(**args)

        self.uniqID = {
            'setup' : 'single',
            'component' : 'alias',
            'parameter' : 'name',
            }

        # elements that need to print information about the parent when printed
        self.needParentInfo = ['parameter']

        # elements for which the information is stored as #PCDATA content
        self.checkText = []

        ## configure comparison

        # depending on the the environment exclude elements from the comparison
        if self.exlusionset=='default':
            self.exclFromCmpList = []
            self.ignoreAttr = { 'component' : ['py_name', 'py_package'] }

        else:
            print("Don't know about comparison environment %s. Should be nothing, 'cool' or 'rtt'" % self.exlusionset, file=self)
            sys.exit(0)
            
        # parse the two files and create dom-docs
        self.doc1 = self.parseFile(files[0])
        self.doc2 = self.parseFile(files[1])


    def diff(self):
        equal = super(CompareSetupXML,self).diff(self.doc1, self.doc2)
        print("HLT setups are%s equal" % ('' if equal else ' not'), file=self)
        return ""

