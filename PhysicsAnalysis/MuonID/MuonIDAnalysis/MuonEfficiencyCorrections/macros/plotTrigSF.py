# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#!/usr/bin/env python
from optparse import OptionParser

p = OptionParser(usage="usage: <path:ROOT file directory>", version="0.1")

p.add_option('--pref',         type='string', default=None,          dest='pref')
p.add_option('--outdir', '-o', type='string', default='plot-flip',   dest='outdir')
p.add_option('--rfile', '-r',  type='string', default=None,          dest='rfile')

p.add_option('--debug',         action='store_true', default=False,   dest='debug')
p.add_option('--save', '-s',    action='store_true', default=False,   dest='save')
p.add_option('--wait', '-w',    action='store_true', default=False,   dest='wait')
p.add_option('--do-err',        action='store_true', default=False,   dest='do_err')

(options, args) = p.parse_args() 

import os
import re
import sys
import time
import ROOT    

#-----------------------------------------------------------------------------
# Configure python and C++ objects, add input files and execute C++ code
#    
def main():

    import PhysicsLight.PhysicsLightConfig  as Config

    Config.setPlotDefaults(ROOT)            

    can = ROOT.TCanvas('can', 'can', 650, 600)
    can.Draw()

    ROOT.gStyle.SetPaintTextFormat('0.2f')
    ROOT.gStyle.SetPalette(1)
    ROOT.gROOT.ForceStyle()

    rf = ROOT.TFile(args[0], 'READ')

    regions = ['barrel_sf', 'endcap_sf']

    err_hists = ['err_stat_up',
                 'err_stat_dw',
                 'err_syst_up',
                 'err_syst_dw',
                 ]
    
    for r in regions:
        plotNom(r, rf, can)
        
        if options.do_err:
            for h in err_hists:
                plotErr(r, '%s_%s' %(r, h), rf, can)

#-----------------------------------------------------------------------------
def plotErr(hbase, hsyst, rf, can):

    print 'plotErr - %s / %s' %(hsyst, hbase)

    hb = rf.Get(hbase)
    hs = rf.Get(hsyst)

    hb.SetStats(False)
    hs.SetStats(False)

    hs.Divide(hb)

    hs.Draw('COLZ TEXT')

    updateCanvas(can, '%s_over_nominal' %hsyst)

#-------------------------------------------------------------------------
def updateCanvas(can, name=None):
    
    if not can:
        sys.exit(0)

    can.Modified()
    can.Update()

    if options.wait:
        can.WaitPrimitive()

    if not can:
        sys.exit(0)
        
    if options.save and name != None:
        can.Print('%s.pdf' %name, 'pdf')

#-----------------------------------------------------------------------------
def plotNom(hbase, rf, can):

    print 'plotNom - %s' %(hbase)

    hb = rf.Get(hbase)

    hb.SetStats(False)
    hb.Draw('COLZ TEXT')

    updateCanvas(can, hbase)

if __name__ == "__main__":
    main()
