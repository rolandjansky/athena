#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration                    

import ROOT
import sys
import argparse
from array import array

ROOT.gROOT.SetBatch(ROOT.kTRUE)

parser = argparse.ArgumentParser()
parser.add_argument('recofile', type=str, help='File with per-LB yields')
parser.add_argument('efffile', type=str, help='File with efficiencies')
parser.add_argument('outfile', type=str, help='Output file')
parser.add_argument('--nlb', type=int, help='# of LBs to combine', default=20)
parser.add_argument('--mode', type=str, help='Zee or Zmumu')
parser.add_argument('--campaign', type=str, help='MC campaign corresponding to year of run')

args          = parser.parse_args()
runmode       = args.mode
campaign      = args.campaign
recozfname    = args.recofile
effzfname     = args.efffile
outfname      = args.outfile
LUMIBLOCKS    = args.nlb

ZXSEC         = 1.929
ZPURITYFACTOR = 0.9935

if runmode == "Zmumu":
    ACCEPTANCE = 0.3323224
if runmode == "Zee":
    ACCEPTANCE = 0.2996


def correction(mu):
    if runmode == "Zee":
        if campaign == "mc16a":
            return 8.83702e-01 - 5.04610e-04*mu - 3.96025e-06*mu*mu
        if campaign == "mc16d":
            return 8.74735e-01 - 1.64286e-04*mu - 7.32040e-06*mu*mu
        if campaign == "mc16e":
            return 9.02495e-01 + 2.15661e-05*mu - 7.04916e-06*mu*mu 

    if runmode == "Zmumu":
        if campaign == "mc16a":
            return 9.90074e-01 - 5.34716e-06*mu - 3.23366e-06*mu*mu
        if campaign == "mc16d":
            return 9.91619e-01 - 1.21674e-04*mu - 1.58362e-06*mu*mu
        if campaign == "mc16e":
            return 9.90808e-01 - 9.99749e-05*mu - 1.40241e-06*mu*mu


recozfile = ROOT.TFile.Open(recozfname)
effzfile  = ROOT.TFile.Open(effzfname)
recoztree = recozfile.lumitree
effztree  = effzfile.lumitree
entrydict = {}

for i in xrange(recoztree.GetEntries()):
    recoztree.GetEntry(i)
    if not recoztree.pass_grl: 
        continue
    # If livetime less than 10 sec, ignore
    if recoztree.lblive < 10 : 
        continue
    
    effztree.Draw('alleff:alleffstat', 'run==%s&&lb==%s' % (recoztree.run, recoztree.lb), 'goff')
    if effztree.GetSelectedRows() == 0:
        print 'Broken for run, lb %s %s' % (recoztree.run, recoztree.lb)
        print 'We THINK there are %d events here ...' % (recoztree.zraw)
        continue
    
    lbzero = (recoztree.lb // LUMIBLOCKS)*LUMIBLOCKS
    run = recoztree.run
    if (run, lbzero) not in entrydict: 
        entrydict[(run, lbzero)] = {'zcount': 0., 'zcounterrsq': 0., 'livetime': 0., 'lbwhen': [-1, -1], 'mu': 0., 'offlumi': 0., 'rolleff': 0., 'rollefferrsq': 0., 'lhcfill': recoztree.lhcfill}
    
    thisdict = entrydict[(run, lbzero)]
    effcy = (effztree.GetV1()[0]*correction(recoztree.mu))
    #thisdict['zcount'] += recoztree.zraw/effcy
    #thisdict['zcounterrsq'] += (1/effcy*recoztree.zrawstat)**2+(recoztree.zraw/effcy**2*effztree.GetV2()[0])**2
    thisdict['zcount'] += recoztree.zraw
    thisdict['zcounterrsq'] += recoztree.zrawstat**2
    effcywght = (effztree.GetV2()[0]*correction(recoztree.mu))**2
    thisdict['rolleff'] += effcy/effcywght
    thisdict['rollefferrsq'] += 1/effcywght
    loclivetime = recoztree.lblive
    #loclivetime = (recoztree.lbwhen[1]-recoztree.lbwhen[0])
    thisdict['livetime'] += loclivetime
    thisdict['mu'] += recoztree.mu*loclivetime
    thisdict['offlumi'] += recoztree.offlumi*loclivetime
    
    if thisdict['lbwhen'][0] > recoztree.lbwhen[0] or thisdict['lbwhen'][0] == -1:
        thisdict['lbwhen'][0] = recoztree.lbwhen[0]    
    if thisdict['lbwhen'][1] < recoztree.lbwhen[1] or thisdict['lbwhen'][1] == -1:
        thisdict['lbwhen'][1] = recoztree.lbwhen[1]

 
fout = ROOT.TFile.Open(outfname, 'RECREATE')
o_run            = array('I', [0])
o_lb             = array('I', [0])
o_lbwhen         = array('d', [0., 0.])
o_zrate          = array('f', [0.])
o_zratestat      = array('f', [0.])
o_zlumi          = array('f', [0.])
o_zlumistat      = array('f', [0.])
o_mu             = array('f', [0.])
o_alleffcorr     = array('f', [0.])
o_alleffcorrstat = array('f', [0.])
o_offlumi        = array('f', [0.])
o_lblive         = array('f', [0.])
o_lhcfill        = array('I', [0])

t = ROOT.TTree( 'lumitree', 'Luminosity tree' )
t.Branch('run', o_run, 'run/i')
t.Branch('lb', o_lb, 'lb/i')
t.Branch('lbwhen', o_lbwhen, 'lbwhen[2]/D')
t.Branch('zrate', o_zrate, 'zrate/F')
t.Branch('zratestat', o_zratestat, 'zratestat/F')
t.Branch('zlumi', o_zlumi, 'zlumi/F')
t.Branch('zlumistat', o_zlumistat, 'zlumistat/F')
t.Branch('offlumi', o_offlumi, 'offlumi/F')
t.Branch('mu', o_mu, 'mu/F')
t.Branch('alleffcorr', o_alleffcorr, 'alleffcorr/F')
t.Branch('alleffcorrstat', o_alleffcorrstat, 'alleffcorrstat/F')
t.Branch('lblive', o_lblive, 'lblive/F')
t.Branch('lhcfill', o_lhcfill, 'lhcfill/i')

for entry, entryval in sorted(entrydict.items()):
    if entryval['livetime'] > 0:
        entryval['mu'] /= entryval['livetime']
        entryval['offlumi'] /= entryval['livetime']
        eff = entryval['rolleff']/entryval['rollefferrsq']
        efferr = 1/entryval['rollefferrsq']**.5
        entryval['zrate'] = entryval['zcount']/eff/entryval['livetime']
        entryval['zratestat'] = (entryval['zcounterrsq']/eff + (entryval['zcount']/eff**2*efferr)**2)**.5/entryval['livetime']
        o_run[0], o_lb[0] = entry
        o_lbwhen[0], o_lbwhen[1] = entryval['lbwhen']
        o_zrate[0] = entryval['zrate']
        o_zratestat[0] = entryval['zratestat']
        o_zlumi[0] = o_zrate[0]*ZPURITYFACTOR/ACCEPTANCE/ZXSEC
        o_zlumistat[0] = o_zratestat[0]*ZPURITYFACTOR/ACCEPTANCE/ZXSEC
        o_mu[0] = entryval['mu']
        o_alleffcorr[0] = eff
        o_alleffcorrstat[0] = efferr
        o_offlumi[0] = entryval['offlumi']
        o_lblive[0] = entryval['livetime']
        o_lhcfill[0] = entryval['lhcfill']
        
        t.Fill()

#t.Write()
newrzt = recoztree.CloneTree()
newrzt.SetName("recolumitree")
newezt = effztree.CloneTree()
newezt.SetName("efflumitree")
fout.Write()
fout.Close()
