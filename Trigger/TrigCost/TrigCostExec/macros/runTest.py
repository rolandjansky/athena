#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from optparse import OptionParser

p = OptionParser(usage='usage: -c <xml configuration file> -n <number of events>', version='1.1')

p.add_option('-c', type= 'string', default = '', dest = 'config', help = 'path to XML configuration file')
p.add_option('-n', type = 'int',   default = 10, dest = 'nevent', help = 'number of events')

(options, args) = p.parse_args()

import os, string, sys
import ROOT, PyCintex

ROOT.gSystem.Load('libTrigMonitoringEventDict')
ROOT.gSystem.Load('libTrigCostBase')
ROOT.gSystem.Load('libTrigCostBaseDict')
ROOT.gSystem.Load('libTrigCostRate')
ROOT.gSystem.Load('libTrigCostExec')
ROOT.gSystem.Load('libTrigCostExecDict')

from ROOT import Anp
    
#-----------------------------------------------------------------------------
# Configure C++ objects, add input files and process data
#    
if __name__ == '__main__':
    
    ROOT.gStyle.SetOptStat('rmeio')
    ROOT.gStyle.SetFillColor(10)
    ROOT.gStyle.SetFrameFillColor(10)
    ROOT.gStyle.SetCanvasColor(10)
    ROOT.gStyle.SetPadColor(10)
    ROOT.gStyle.SetTitleFillColor(10)
    ROOT.gStyle.SetStatColor(10)
    ROOT.gROOT.SetBatch(True)

    #
    # Create, configure and execute C++ module
    #    
    test = ROOT.Anp.TestInterface()
    test.RunTest(options.config, options.nevent)
