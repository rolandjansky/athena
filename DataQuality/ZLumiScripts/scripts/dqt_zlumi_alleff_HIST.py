#!/usr/bin/env python
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration                   
import sys, os, glob
import ROOT

ROOT.gStyle.SetOptStat(0)

#ACCEPTANCE = 3.173927e-01
ACCEPTANCE = 3.323224e-01

import argparse
parser = argparse.ArgumentParser()
parser.add_argument('infile', type=str, help='input HIST file')
parser.add_argument('--out', type=str, help='output ROOT file')
parser.add_argument('--plotdir', type=str, help='Directory to dump plots',
                    default='plots')
parser.add_argument('--debug', action='store_true', help='Be verbose in output')
parser.add_argument('--mode', type=str, help='Zee or Zmumu')


args = parser.parse_args()

infilename = args.infile
infile = ROOT.TFile.Open(infilename, 'READ')

runmode = args.mode
print 'Running in', runmode, 'mode'


runname = None
for k in infile.GetListOfKeys():
    if k.GetName().startswith('run_'):
        runname = k.GetName()
        break
if not runname:
    print 'Cannot find run directory in input file'
    sys.exit(1)
else:
    print 'Found runname', runname

lbdirs = []
for k in infile.Get(runname).GetListOfKeys():
    if k.GetName().startswith('lb_'):
        lbdirs.append(k.GetName())

print 'Now to dump'
lbnums = sorted([int(_[3:]) for _ in lbdirs])

effcyt = ROOT.TH1F('effcyt', 'Trigger efficiency', lbnums[-1]-lbnums[0]+1, lbnums[0]-0.5, 
               lbnums[-1]+0.5)
effcyr = ROOT.TH1F('effcyr', 'Loose muon reco efficiency', lbnums[-1]-lbnums[0]+1, lbnums[0]-0.5, 
               lbnums[-1]+0.5)
effcya = ROOT.TH1F('effcya', 'Combined acc x efficiency', lbnums[-1]-lbnums[0]+1, lbnums[0]-0.5, 
               lbnums[-1]+0.5)
effcydir = ROOT.TH1F('effcydir', 'Direct acc x efficiency', lbnums[-1]-lbnums[0]+1, lbnums[0]-0.5, 
               lbnums[-1]+0.5)

from array import array
fout = ROOT.TFile(args.out if args.out else '%s_all.root' % runname[4:], 'RECREATE')
o_run = array('I', [0])
o_lb = array('I', [0])
o_lbwhen = array('d', [0., 0.])
o_z_one = array('f', [0.])
o_z_two = array('f', [0.])
o_trigeff = array('f', [0.])
o_trigeffstat = array('f', [0.])
o_recoeff = array('f', [0.])
o_recoeffstat = array('f', [0.])
o_alleff = array('f', [0.])
o_alleffstat = array('f', [0.])
o_ae = array('f', [0.])
o_aestat = array('f', [0.])
tl = ROOT.TTree( 'lumitree', 'Luminosity tree' )
tl.Branch('run', o_run, 'run/i')
tl.Branch('lb', o_lb, 'lb/i')
tl.Branch('lbwhen', o_lbwhen, 'lbwhen[2]/D')
tl.Branch('z_one', o_z_one, 'z_one/F')
tl.Branch('z_two', o_z_two, 'z_two/F')
tl.Branch('trigeff', o_trigeff, 'trigeff/F')
tl.Branch('trigeffstat', o_trigeffstat, 'trigeffstat/F')
tl.Branch('recoeff', o_recoeff, 'recoeff/F')
tl.Branch('recoeffstat', o_recoeffstat, 'recoeffstat/F')
tl.Branch('alleff', o_alleff, 'alleff/F')
tl.Branch('alleffstat', o_alleffstat, 'alleffstat/F')
tl.Branch('ae', o_ae, 'ae/F')
tl.Branch('aestat', o_aestat, 'aestat/F')


from DQUtils import fetch_iovs
#rset=set(_[0] for _ in rlb)
#print rset
lblb = fetch_iovs("LBLB", runs=int(runname[4:])).by_run
for lb in sorted(lbdirs):
    if runmode == "Zee":
        h = infile.Get('%s/%s/GLOBAL/DQTGlobalWZFinder/m_eltrigtp_matches' % (runname, lb))
        hmo = infile.Get('%s/%s/GLOBAL/DQTGlobalWZFinder/m_ele_tight_good_os' % (runname, lb))
        hms = infile.Get('%s/%s/GLOBAL/DQTGlobalWZFinder/m_ele_tight_good_ss' % (runname, lb))
        hno = infile.Get('%s/%s/GLOBAL/DQTGlobalWZFinder/m_ele_tight_bad_os' % (runname, lb))
        hns = infile.Get('%s/%s/GLOBAL/DQTGlobalWZFinder/m_ele_tight_bad_ss' % (runname, lb))
    if runmode == "Zmumu":
        h = infile.Get('%s/%s/GLOBAL/DQTGlobalWZFinder/m_mutrigtp_matches' % (runname, lb))
        hmo = infile.Get('%s/%s/GLOBAL/DQTGlobalWZFinder/m_muloosetp_match_os' % (runname, lb))
        hms = infile.Get('%s/%s/GLOBAL/DQTGlobalWZFinder/m_muloosetp_match_ss' % (runname, lb))
        hno = infile.Get('%s/%s/GLOBAL/DQTGlobalWZFinder/m_muloosetp_nomatch_os' % (runname, lb))
        hns = infile.Get('%s/%s/GLOBAL/DQTGlobalWZFinder/m_muloosetp_nomatch_ss' % (runname, lb))
    lbnum = int(lb[3:])
    yld = (h[2], h[3])
    ylderr = (h.GetBinError(2), h.GetBinError(3))
    #print yld, ylderr
    A, B = yld
    o_z_one[0], o_z_two[0] = yld
    if B == 0: continue
    eff = 1./(float(A)/B/2.+1.)
    inverrsq = ((1/2./B)*ylderr[0])**2+((A/2./B**2)*ylderr[1])**2
    o_trigeff[0] = eff
    o_trigeffstat[0] = (inverrsq**.5)*(eff**2)
    o_run[0], o_lb[0] = int(runname[4:]), lbnum
    try:
        iov = lblb[int(runname[4:])][lbnum-1]
        o_lbwhen[0], o_lbwhen[1] = iov.StartTime/1e9, iov.EndTime/1e9
    except Exception, e:
        o_lbwhen[0], o_lbwhen[1] = 0, 0
    effcyt.SetBinContent(lbnum-lbnums[0]+1, eff)
    effcyt.SetBinError(lbnum-lbnums[0]+1, o_trigeffstat[0])

    def extract(histogram):
        dbl = ROOT.Double()
        rv1 = histogram.IntegralAndError(21, 30, dbl)
        return (rv1, float(dbl))
    matchos, matchoserr = extract(hmo)
    matchss, matchsserr = extract(hms)
    nomatchos, nomatchoserr = extract(hno)
    nomatchss, nomatchsserr = extract(hns)
    if args.debug:
        print lb
        print ' ->', matchos, matchoserr
        print ' ->', matchss, matchsserr
        print ' ->', nomatchos, nomatchoserr
        print ' ->', nomatchss, nomatchsserr
    A = float(matchos-matchss)
    Aerr = (matchoserr**2+matchsserr**2)**.5
    B = float(nomatchos-nomatchss)
    Berr = (nomatchoserr**2+nomatchsserr**2)**.5
    if Berr == 0: Berr = 1.
    if A == 0 or B/A == -1: 
        eff = 1.
        inverrsq = 1.
    else:
        eff = 1./(1.+B/A)
        inverrsq = ((-B/A**2)*Aerr)**2+((1./A)*Berr)**2
    o_recoeff[0] = eff
    o_recoeffstat[0] = (inverrsq**.5)*(eff**2)
    effcyr.SetBinContent(lbnum-lbnums[0]+1, eff)
    effcyr.SetBinError(lbnum-lbnums[0]+1, o_recoeffstat[0])

    o_ae[0] = ACCEPTANCE*(1-(1-o_trigeff[0])**2)*(o_recoeff[0])**2
    o_aestat[0] = ACCEPTANCE*((o_recoeff[0]**2*2*(1-o_trigeff[0])*o_trigeffstat[0])**2+(2*o_recoeff[0]*(1-(1-o_trigeff[0])**2)*o_recoeffstat[0])**2)**.5
    o_alleff[0] = (1-(1-o_trigeff[0])**2)*(o_recoeff[0])**2
    o_alleffstat[0] = ((o_recoeff[0]**2*2*(1-o_trigeff[0])*o_trigeffstat[0])**2+(2*o_recoeff[0]*(1-(1-o_trigeff[0])**2)*o_recoeffstat[0])**2)**.5
    effcya.SetBinContent(lbnum-lbnums[0]+1, o_ae[0])
    effcya.SetBinError(lbnum-lbnums[0]+1, o_aestat[0])


    tl.Fill()
tl.Write()
print 'Done'

c1 = ROOT.TCanvas()
effcya.SetMarkerStyle(21)
effcya.SetMarkerColor(ROOT.kBlue)
effcya.GetYaxis().SetRangeUser(0.25,0.31)
effcya.Draw('PE')
c1.Print(os.path.join(args.plotdir, '%s_combined_efficiency.eps' % runname[4:]))
fout.WriteTObject(effcya)
c1.Clear()
effcyt.SetMarkerStyle(21)
effcyt.SetMarkerColor(ROOT.kBlue)
effcyt.GetYaxis().SetRangeUser(0.66,0.86)
effcyt.Draw('PE')
c1.Print(os.path.join(args.plotdir, '%s_trigger_efficiency.eps' % runname[4:]))
fout.WriteTObject(effcyt)
c1.Clear()
effcyr.SetMarkerStyle(21)
effcyr.SetMarkerColor(ROOT.kBlue)
effcyr.GetYaxis().SetRangeUser(0.9,1.0)
effcyr.Draw('PE')
c1.Print(os.path.join(args.plotdir, '%s_reco_efficiency.eps' % runname[4:]))
fout.WriteTObject(effcyr)
fout.Close()

if runmode == "Zee":
    sumweights = infile.Get('%s/GLOBAL/DQTDataFlow/m_sumweights' % runname)
    ctr = infile.Get('%s/GLOBAL/DQTGlobalWZFinder/m_Z_Counter_el' % runname)
if runmode == "Zmumu":
    sumweights = infile.Get('%s/GLOBAL/DQTDataFlow/m_sumweights' % runname)
    ctr = infile.Get('%s/GLOBAL/DQTGlobalWZFinder/m_Z_Counter_mu' % runname)
if sumweights:
    for ibin in xrange(1,sumweights.GetNbinsX()+1):
        o_lb[0] = int(sumweights.GetBinCenter(ibin))
        ctrbin = ctr.FindBin(o_lb[0])
        print ibin, o_lb[0], sumweights[ibin], ctr[ctrbin]
        if sumweights[ibin] == 0: continue
        p = ctr[ctrbin]/sumweights[ibin]
        o_alleff[0]=p
        try:
            o_alleffstat[0]=(p*(1-p))**.5*(sumweights.GetBinError(ibin)/sumweights[ibin])
        except ValueError:
            o_alleffstat[0]=(sumweights.GetBinError(ibin)/sumweights[ibin])
        effcydir.SetBinContent(effcydir.FindBin(o_lb[0]), p)
        effcydir.SetBinError(effcydir.FindBin(o_lb[0]), o_alleffstat[0])

    effcya.GetYaxis().SetRangeUser(0.27,0.31)
    effcya.Draw('PE')
    effcydir.SetMarkerStyle(20)
    effcydir.SetMarkerColor(ROOT.kRed)
    effcydir.Draw('SAME,PE')
    leg=ROOT.TLegend(0.65, 0.7, 0.89, 0.89)
    leg.AddEntry(effcya, 'Predicted A#epsilon', 'PE')
    leg.AddEntry(effcydir, 'Actual A#epsilon', 'PE')
    leg.Draw()
    c1.Print(os.path.join(args.plotdir, '%s_tp_comparison.eps' % runname[4:]))

    effcyrat=effcydir.Clone()
    effcyrat.Divide(effcya)
    effcyrat.SetTitle('MC Correction Factor')
    effcyrat.SetXTitle('<#mu>')
    effcyrat.Draw('PE')
    effcyrat.Fit('pol1')
    c1.Print(os.path.join(args.plotdir, '%s_tp_correction.eps' % runname[4:]))
