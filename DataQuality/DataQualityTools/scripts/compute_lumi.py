#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import ROOT
import sys
import logging
logging.basicConfig(format='%(levelname)s:%(message)s', level=logging.INFO)

BINWIDTH=5

ZPURITYFACTOR=0.9935
ZXSEC=1.929
ZATIMESC=0.2578
#ZATIMESC=0.29632

ROOT.gStyle.SetOptStat(0)

fin = ROOT.TFile.Open(sys.argv[1])
runname = None
for key in fin.GetListOfKeys():
    if key.GetName().startswith('run_'):
        runname = key.GetName()
        break

if not runname:
    logging.critical("Can't find run_* directory in input file %s", sys.argv[1])
    sys.exit(1)

z_m = fin.Get('%s/GLOBAL/DQTGlobalWZFinder/m_Z_Counter_mu' % runname)

if not z_m:
    logging.critical("Can't retrieve m_Z_Counter_mu")
    sys.exit(1)

lb_length = fin.Get('%s/GLOBAL/DQTGlobalWZFinder/m_lblength_lb' % runname)
lbmin, lbmax = lb_length.GetXaxis().GetXmin(), lb_length.GetXaxis().GetXmax()

logging.info('Starting livetime lookup ... (remove when we have a proper in-file implementation ...)')
livetime = ROOT.TProfile('livetime', 'Livetime', int(lbmax-lbmin), lbmin, lbmax)
official_lum = ROOT.TProfile('official_lum', 'official integrated luminosity', int(lbmax-lbmin), lbmin, lbmax)
official_lum_zero = ROOT.TProfile('official_lum_zero', 'official inst luminosity', int(lbmax-lbmin), lbmin, lbmax)
from DQUtils import fetch_iovs
from DQUtils.iov_arrangement import inverse_lblb
lblb = fetch_iovs("LBLB", runs=int(runname[4:]))
lbtime = inverse_lblb(lblb)
#print list(lbtime)
iovs = fetch_iovs('COOLOFL_TRIGGER::/TRIGGER/OFLLUMI/LumiAccounting', lbtime.first.since, lbtime.last.until, tag='OflLumiAcct-13TeV-003')
#print list(iovs)
for iov in iovs:
    if not lbmin < iov.LumiBlock < lbmax:
        continue
    livetime.Fill(iov.LumiBlock, iov.LiveFraction)
    official_lum_zero.Fill(iov.LumiBlock, iov.InstLumi/1e3)
    official_lum.Fill(iov.LumiBlock, iov.InstLumi*iov.LBTime*iov.LiveFraction)

divisor = lb_length.Clone('divisor').ProjectionX()
px = livetime.ProjectionX()
divisor.Multiply(px)

nrebinned_bins = ((lbmax-lbmin) // BINWIDTH) + 1

lumiplot_m = ROOT.TH1F('lumiplot_m', 'Lumi, Z->#mu#mu (Run %s)' % runname[4:], 
                       int(nrebinned_bins),
                       lbmin, lbmin+BINWIDTH*nrebinned_bins)
lumiplot_m.SetXTitle('LB')
xsec_m = ROOT.TH1F('xsec_m', 'eff #sigma, Z->#mu#mu', int(nrebinned_bins),
                       lbmin, lbmin+BINWIDTH*nrebinned_bins)
lumiplot_raw_m =  ROOT.TH1F('lumiplot_raw_m', 'Lumi, Z->#mu#mu, per LB', 
                           int(lbmax-lbmin),
                           lbmin, lbmax)

num_m, lum, denom = 0, 0, 0
for ibin in xrange(1, int(lbmax-lbmin)+1):
    if divisor[ibin] > 0:
        lumiplot_raw_m.SetBinContent(ibin, z_m[ibin]/divisor[ibin]*ZPURITYFACTOR/ZATIMESC/ZXSEC)
        lumiplot_raw_m.SetBinError(ibin, z_m[ibin]**.5/divisor[ibin]*ZPURITYFACTOR/ZATIMESC/ZXSEC)
    num_m += z_m[ibin]
    denom += divisor[ibin]
    lum += official_lum[ibin]
    if (ibin % BINWIDTH) == 0:
        ribin = int(ibin // BINWIDTH) 
        if denom > 0:
            lumiplot_m.SetBinContent(ribin, num_m/denom*ZPURITYFACTOR/ZATIMESC/ZXSEC)
            lumiplot_m.SetBinError(ribin, num_m**.5/denom*ZPURITYFACTOR/ZATIMESC/ZXSEC)
        if lum > 0:
            xsec_m.SetBinContent(ribin, num_m/lum*ZPURITYFACTOR/ZATIMESC)
            xsec_m.SetBinError(ribin, num_m**.5/lum*ZPURITYFACTOR/ZATIMESC)
        num_m, lum, denom = 0, 0, 0
        

leg = ROOT.TLegend(0.6, 0.75, 0.89, 0.88)
lumiplot_m.Draw()
official_lum_zero.SetLineColor(ROOT.kRed)
official_lum_zero.Draw('SAME,HIST')
leg.AddEntry(lumiplot_m, 'Z luminosity')
leg.AddEntry(official_lum_zero, 'ATLAS preferred lumi', 'L')
leg.SetBorderSize(0)
leg.Draw()
