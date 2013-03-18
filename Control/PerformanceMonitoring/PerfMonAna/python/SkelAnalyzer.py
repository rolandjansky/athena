# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file: SkelAnalyzer.py
# @purpose: a set of classes to analyze (Skel) data from a perfmon tuple
# @author: Sebastien Binet <binet@cern.ch>

""" A set of classes to analyze (Skel) data from a perfmon tuple
"""
#
#
__author__  = 'Sebastien Binet'
__version__ = "$Revision: 1.3 $"
__doc__     = "A set of classes to analyze (Skel) data from a perfmon tuple."

import logging
from PerfMonAna.PyRootLib import importRoot
from PerfMonAna.Data      import Data
from PerfMonAna.Analyzer  import Analyzer

class SkelAnalyzer( Analyzer ):

    typeName = 'Skel'

    def __init__(self, name, fileName):
        Analyzer.__init__(self, name, fileName, '<none>')
        return

    def bookHistos(self):
        ROOT = importRoot()
        #Analyzer.__bookHistos(self)

        meta = self.meta.skel

        nJobOpts = meta.cfg().size()+1
        return

    def fillHistos(self):

        self.msg.debug("filling histograms...")

        self.msg.debug("filling histograms... [DONE]")
        return

    pass # class SkelAnalyzer
