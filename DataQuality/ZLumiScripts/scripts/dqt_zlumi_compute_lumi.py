#!/usr/bin/env python
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import ROOT
import sys, os
import logging
logging.basicConfig(format='%(levelname)s:%(message)s', level=logging.INFO)
import argparse
parser = argparse.ArgumentParser()
parser.add_argument('infile', type=str, help='input HIST file')
parser.add_argument('--grl', type=str, help='Specify an input GRL')
parser.add_argument('--out', type=str, help='output ROOT file')
parser.add_argument('--tag', type=str, help='Lumi tag',
                    default='OflLumiAcct-001')
parser.add_argument('--useofficial', action='store_true', help='Use official lumi folder (otherwise, use OflLumiAcct')
parser.add_argument('--lumifolder', type=str, help='Lumi folder', default='/TRIGGER/OFLLUMI/OflPrefLumi')
parser.add_argument('--lumitag', type=str, help='Lumi tag', default='OflLumi-13TeV-009')
parser.add_argument('--plotdir', type=str, help='Directory to dump plots',
                    default='plots')
parser.add_argument('--mudep', type=int, help='Run mu-dependent efficiencies',
                    default=0)
parser.add_argument('--dblivetime', action='store_true',
                    help='Look up livetime from DB')
parser.add_argument('--mode', type=str, help='Zee or Zmumu')

args = parser.parse_args()

BINWIDTH=10

ZPURITYFACTOR=0.9935
ZXSEC=1.929
#ZATIMESC=0.2578
ZATIMESC=0.29632

def mu_dep_eff(mu):
    #make breakpoint at 8 match
    if 0 <= mu < 8: return 0.3152 #0.3141
    elif 8 <= mu < 27: return 0.3191 - 0.000493*mu
    elif 27 <= mu: return 0.3443 - 0.00143*mu
    else:
        print 'WTF??'
        return ZATIMESC

ROOT.gStyle.SetOptStat(0)

fin = ROOT.TFile.Open(args.infile)
runname = None
for key in fin.GetListOfKeys():
    if key.GetName().startswith('run_'):
        runname = key.GetName()
        break

if args.grl:
    import DQUtils
    grl = DQUtils.grl.load_grl(args.grl)
else:
    grl = None

if not runname:
    logging.critical("Can't find run_* directory in input file %s", args.infile)
    sys.exit(1)

z_m = fin.Get('%s/GLOBAL/DQTGlobalWZFinder/m_Z_Counter_mu' % runname)
if args.out:
    outfname = args.out
else:
    outfname = '%s_data.root' % runname[4:]

runmode = args.mode
print 'Running in', runmode, 'mode'
if runmode == 'Zee':
    z_m = fin.Get('%s/GLOBAL/DQTGlobalWZFinder/m_Z_Counter_el' % runname)
    if not z_m:
        logging.critical("Can't retrieve m_Z_Counter_el")
        sys.exit(1)

if runmode == 'Zmumu':
    z_m = fin.Get('%s/GLOBAL/DQTGlobalWZFinder/m_Z_Counter_mu' % runname)
    if not z_m:
        logging.critical("Can't retrieve m_Z_Counter_mu")
        sys.exit(1)


fout = None
t = None

from array import array
o_passgrl = array('i', [0])
o_mu = array('f', [0.])

o_lb_rb = array('I', [0,0])
o_lbwhen_rb = array('d', [0., 0.])
o_zlumi_rb = array('f', [0.])
o_zlumistat_rb = array('f', [0.])
o_offlumi_rb = array('f', [0.])
o_mu_rb = array('f', [0.])
o_lblive_rb = array('f', [0.])
if True:
    fout = ROOT.TFile.Open(outfname, 'RECREATE')
    o_run = array('I', [int(runname[4:])])
    o_lb = array('I', [0])
    o_lbwhen = array('d', [0., 0.])
    o_zraw = array('f', [0.])
    o_zrawstat = array('f', [0.])
    o_zlumi = array('f', [0.])
    o_zlumistat = array('f', [0.])
    o_offlumi = array('f', [0.])
    o_lblive = array('f', [0.])
    o_lhcfill = array('I', [0])
    t = ROOT.TTree( 'lumitree', 'Luminosity tree' )
    t.Branch('run', o_run, 'run/i')
    t.Branch('lb', o_lb, 'lb/i')
    t.Branch('lbwhen', o_lbwhen, 'lbwhen[2]/D')
    t.Branch('zraw', o_zraw, 'zraw/F')
    t.Branch('zrawstat', o_zrawstat, 'zrawstat/F')
    t.Branch('zlumi', o_zlumi, 'zlumi/F')
    t.Branch('zlumistat', o_zlumistat, 'zlumistat/F')
    t.Branch('offlumi', o_offlumi, 'offlumi/F')
    t.Branch('mu', o_mu, 'mu/F')
    t.Branch('lblive', o_lblive, 'lblive/F')
    t.Branch('pass_grl', o_passgrl, 'pass_grl/I')
    t.Branch('lhcfill', o_lhcfill, 'lhcfill/i')

    t_rb = ROOT.TTree( 'lumitree_rb', 'Luminosity tree, rebinned' )
    t_rb.Branch('run', o_run, 'run/i')
    t_rb.Branch('lb', o_lb_rb, 'lb[2]/i')
    t_rb.Branch('lbwhen', o_lbwhen_rb, 'lbwhen[2]/D')
    t_rb.Branch('zlumi', o_zlumi_rb, 'zlumi/F')
    t_rb.Branch('zlumistat', o_zlumistat_rb, 'zlumistat/F')
    t_rb.Branch('offlumi', o_offlumi_rb, 'offlumi/F')
    t_rb.Branch('mu', o_mu_rb, 'mu/F')
    t_rb.Branch('lblive', o_lblive_rb, 'lblive/F')

    ROOT.gROOT.cd('/')


lb_length = fin.Get('%s/GLOBAL/DQTGlobalWZFinder/m_lblength_lb' % runname)
lbmin, lbmax = lb_length.GetXaxis().GetXmin(), lb_length.GetXaxis().GetXmax()
logging.info('low, high LBs: %s, %s', lbmin, lbmax)

if args.dblivetime:
    logging.info('Starting livetime lookup ... (remove when we have a proper in-file implementation ...)')
    livetime = ROOT.TProfile('livetime', 'Livetime', int(lbmax-lbmin), lbmin, lbmax)
else:
    livetime = fin.Get('%s/GLOBAL/DQTGlobalWZFinder/m_livetime_lb' % runname)

official_lum = ROOT.TProfile('official_lum', 'official integrated luminosity', int(lbmax-lbmin), lbmin, lbmax)
official_lum_zero = ROOT.TProfile('official_lum_zero', 'official inst luminosity', int(lbmax-lbmin), lbmin, lbmax)
official_mu = ROOT.TProfile('official_mu', 'official mu', int(lbmax-lbmin), lbmin, lbmax)
from DQUtils import fetch_iovs
from DQUtils.iov_arrangement import inverse_lblb
lblb = fetch_iovs("LBLB", runs=int(runname[4:]))
lbtime = inverse_lblb(lblb)
#print list(lbtime)
iovs_acct = fetch_iovs('COOLOFL_TRIGGER::/TRIGGER/OFLLUMI/LumiAccounting', lbtime.first.since, lbtime.last.until, tag=args.tag)
if args.useofficial:
    iovs_lum = fetch_iovs('COOLOFL_TRIGGER::%s' % args.lumifolder, lblb.first.since, lblb.last.until, tag=args.lumitag, channels=[0])
    #print list(iovs_lum)
lb_start_end = {}
lb_lhcfill = {}
for iov in lblb:
    lb_start_end[iov.since & 0xffffffff] = (iov.StartTime/1e9, iov.EndTime/1e9)

for iov in iovs_acct:
    if not lbmin < iov.LumiBlock < lbmax:
        continue
    lb_lhcfill[iov.LumiBlock] = iov.FillNumber
    if args.dblivetime:
        livetime.Fill(iov.LumiBlock, iov.LiveFraction)
    #print iov.InstLumi, iovs_lum[iov.LumiBlock-1].LBAvInstLumi
    if not args.useofficial:
        official_lum_zero.Fill(iov.LumiBlock, iov.InstLumi/1e3)
        official_lum.Fill(iov.LumiBlock, iov.InstLumi*iov.LBTime*iov.LiveFraction/1e3)
        official_mu.Fill(iov.LumiBlock, iov.AvEvtsPerBX)
    else:
        offlumiov = [_ for _ in iovs_lum if _.since.lumi==iov.LumiBlock]
        if len(offlumiov) != 1: 
            print 'MAJOR PROBLEM, LUMI IOV MISMATCH'
            print len(offlumiov)
            continue
        offlumiov = offlumiov[0]
        official_lum_zero.Fill(iov.LumiBlock, offlumiov.LBAvInstLumi/1e3)
        official_lum.Fill(iov.LumiBlock, offlumiov.LBAvInstLumi*iov.LBTime*iov.LiveFraction/1e3)
        official_mu.Fill(iov.LumiBlock, offlumiov.LBAvEvtsPerBX)

divisor = lb_length.Clone('divisor').ProjectionX()
px = livetime.ProjectionX()
divisor.Multiply(px)

nrebinned_bins = ((lbmax-lbmin) // BINWIDTH) + 1

if runmode == 'Zee':
    lumititle = 'Lumi, Z->ee (Run %s)' % runname[4:]
    efftitle = 'eff #sigma, Z->ee'
    lumirawtitle = 'Lumi, Z->ee per LB'
if runmode == 'Zmumu':
    lumititle = 'Lumi, Z->#mu#mu (Run %s)' % runname[4:]
    efftitle = 'eff #sigma, Z->#mu#mu'
    lumirawtitle = 'Lumi, Z->#mu#mu per LB'


lumiplot_m = ROOT.TH1F('lumiplot_m', lumititle % runname[4:], 
                       int(nrebinned_bins),
                       lbmin, lbmin+BINWIDTH*nrebinned_bins)
lumiplot_m_ratio = ROOT.TH1F('lumiplot_m_ratio', 'Z/official lumi ratio (Run %s)' % runname[4:], 
                       int(nrebinned_bins),
                       lbmin, lbmin+BINWIDTH*nrebinned_bins)
lumiplot_m.SetXTitle('LB')
lumiplot_m.SetYTitle('Luminosity (x 10^{33} cm^{-2} s^{-1})')
xsec_m = ROOT.TH1F('xsec_m', efftitle, int(nrebinned_bins),
                       lbmin, lbmin+BINWIDTH*nrebinned_bins)
lumiplot_raw_m =  ROOT.TH1F('lumiplot_raw_m', lumirawtitle, 
                           int(lbmax-lbmin),
                           lbmin, lbmax)

num_m, lum, denom, weighted_mu = 0, 0, 0, 0
tot_num_m, tot_denom, tot_lum = 0, 0, 0
for ibin in xrange(1, int(lbmax-lbmin)+1):
    profileflag=True
    try:
        z_m[ibin]
    except IndexError, e:
        logging.error('Something unfortunate has happened; LB %d missing from Z count' % (ibin + lbmin - 1))
        profileflag=False
    if args.mudep:
        l_zatimesc = mu_dep_eff(official_mu[ibin])
    else:
        l_zatimesc = ZATIMESC
    if grl and not DQUtils.grl.grl_contains_run_lb(grl, (int(runname[4:]), int(lumiplot_raw_m.GetBinCenter(ibin)))):
        o_passgrl[0]=0
    else:
        o_passgrl[0]=1
    if divisor[ibin] > 0 and profileflag:
        lumiplot_raw_m.SetBinContent(ibin, z_m[ibin]/divisor[ibin]*ZPURITYFACTOR/l_zatimesc/ZXSEC)
        lumiplot_raw_m.SetBinError(ibin, z_m[ibin]**.5/divisor[ibin]*ZPURITYFACTOR/l_zatimesc/ZXSEC)
    o_mu[0] = official_mu[ibin]
    if o_passgrl[0]:
        if profileflag:
            num_m += z_m[ibin]; tot_num_m += z_m[ibin]
        denom += divisor[ibin]; tot_denom += divisor[ibin]
        lum += official_lum[ibin]; tot_lum += official_lum[ibin]
        weighted_mu += o_mu[0]*divisor[ibin]
    

    # fill tree
    if t:
        #print ibin, lumiplot_raw_m.GetBinCenter(ibin)
        o_lb[0] = int(lumiplot_raw_m.GetBinCenter(ibin))
        o_lbwhen[0] = lb_start_end[o_lb[0]][0]
        o_lbwhen[1] = lb_start_end[o_lb[0]][1]
        o_zraw[0] = z_m[ibin] if profileflag else 0
        o_zrawstat[0] = z_m.GetBinError(ibin) if profileflag else 0
        o_zlumi[0] = lumiplot_raw_m[ibin]
        o_zlumistat[0] = lumiplot_raw_m.GetBinError(ibin)
        o_offlumi[0] = official_lum_zero[ibin]
        o_lblive[0] = divisor[ibin]
        o_lhcfill[0] = lb_lhcfill[o_lb[0]]
        t.Fill()

    if (ibin % BINWIDTH) == 0:
        ribin = int(ibin // BINWIDTH) 
        o_mu_rb[0] = weighted_mu/denom if denom > 0 else 0
        if args.mudep:
            l_zatimesc = mu_dep_eff(o_mu_rb[0])
        else:
            l_zatimesc = ZATIMESC
        if denom > 0:
            lumiplot_m.SetBinContent(ribin, num_m/denom*ZPURITYFACTOR/l_zatimesc/ZXSEC)
            lumiplot_m.SetBinError(ribin, num_m**.5/denom*ZPURITYFACTOR/l_zatimesc/ZXSEC)
        if lum > 0:
            xsec_m.SetBinContent(ribin, num_m/lum*ZPURITYFACTOR/l_zatimesc)
            xsec_m.SetBinError(ribin, num_m**.5/lum*ZPURITYFACTOR/l_zatimesc)
        if denom > 0:
            o_zlumi_rb[0] = num_m/denom*ZPURITYFACTOR/l_zatimesc/ZXSEC
            o_zlumistat_rb[0] = num_m**.5/denom*ZPURITYFACTOR/l_zatimesc/ZXSEC
            o_offlumi_rb[0] = lum/denom
            if o_offlumi_rb[0] > 0:
                lumiplot_m_ratio.SetBinContent(ribin, o_zlumi_rb[0]/o_offlumi_rb[0])
                lumiplot_m_ratio.SetBinError(ribin, o_zlumistat_rb[0]/o_offlumi_rb[0])
        else:
            o_zlumi_rb[0] = 0.
            o_zlumistat_rb[0] = 0.
            o_offlumi_rb[0] = 0.
            o_mu_rb[0] = 0.
        o_lb_rb[1] = int(lumiplot_raw_m.GetBinCenter(ibin))
        o_lb_rb[0] = int(lumiplot_raw_m.GetBinCenter(ibin)-BINWIDTH+1)
        o_lbwhen_rb[0] = lb_start_end[o_lb_rb[0]][0]
        o_lbwhen_rb[1] = lb_start_end[o_lb_rb[1]][1]
        o_lblive_rb[0] = denom
        if t:
            t_rb.Fill()
        
        num_m, lum, denom, weighted_mu = 0, 0, 0, 0
        
if fout:
    fout.cd()
    t.Write()
    t_rb.Write()

    if tot_lum > 0:
        tge = ROOT.TGraphErrors(1)
        tge.SetPoint(0, int(runname[4:]), tot_num_m*ZPURITYFACTOR/ZATIMESC/ZXSEC/tot_lum)
        tge.SetPointError(0, 0, tot_num_m**.5*ZPURITYFACTOR/ZATIMESC/ZXSEC/tot_lum)
        tge.SetName('lum_ratio')
        tge.Write()
    fout.Close()

c1 = ROOT.TCanvas()
c1.SetTickx()
c1.SetTicky()
leg = ROOT.TLegend(0.6, 0.75, 0.89, 0.88)
lumiplot_m.Draw()
official_lum_zero.SetLineColor(ROOT.kRed)
official_lum_zero.Draw('SAME,HIST')
leg.AddEntry(lumiplot_m, 'Z luminosity')
leg.AddEntry(official_lum_zero, 'ATLAS preferred lumi', 'L')
leg.SetBorderSize(0)
leg.Draw()
c1.Print(os.path.join(args.plotdir, '%s_lumi.eps' % runname[4:]))
c1.Print(os.path.join(args.plotdir, '%s_lumi.png' % runname[4:]))

c1.Clear()
lumiplot_m_ratio.Draw()
c1.Print(os.path.join(args.plotdir, '%s_lumi_ratio.eps' % runname[4:]))
c1.Print(os.path.join(args.plotdir, '%s_lumi_ratio.png' % runname[4:]))
