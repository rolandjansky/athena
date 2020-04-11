#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration                   

import sys, os, glob
import ROOT
import argparse
import math
from array import array
from DQUtils import fetch_iovs
ROOT.gROOT.SetBatch(ROOT.kTRUE)
ROOT.gStyle.SetOptStat(0)

parser = argparse.ArgumentParser()
parser.add_argument('infile', type=str, help='input HIST file')
parser.add_argument('--out', type=str, help='output ROOT file')
parser.add_argument('--plotdir', type=str, help='Directory to dump plots', default='plots')
parser.add_argument('--debug', action='store_true', help='Be verbose in output')
parser.add_argument('--mode', type=str, help='Zee or Zmumu')
args = parser.parse_args()

infilename = args.infile
runmode    = args.mode
infile     = ROOT.TFile.Open(infilename, 'READ')


def trig_tag_and_probe(h, effcyt):
    yld    = (h[2], h[3])
    ylderr = (h.GetBinError(2), h.GetBinError(3))
    A, B   = yld
    o_z_one[0], o_z_two[0] = yld

    if B == 0:
        return

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


def reco_tag_and_probe(hmo, hms, hno, hns, effcyr):
    bin1 = hmo.GetXaxis().FindBin(86000)
    bin2 = hmo.GetXaxis().FindBin(95000)
    matchos, matchoserr = extract(hmo, bin1, bin2)
    matchss, matchsserr = extract(hms, bin1, bin2)
    nomatchos, nomatchoserr = extract(hno, bin1, bin2)
    nomatchss, nomatchsserr = extract(hns, bin1, bin2)

    A    = float(matchos-matchss)
    Aerr = (matchoserr**2+matchsserr**2)**.5
    B    = float(nomatchos-nomatchss)
    Berr = (nomatchoserr**2+nomatchsserr**2)**.5

    if Berr == 0:
        Berr = 1.
    if A == 0 or B/A == -1:
        eff = 1.
        inverrsq = 1.
    else:
        eff = 1./(1.+B/A)
        inverrsq = ((-B/A**2)*Aerr)**2+((1./A)*Berr)**2

    o_recoeff[0]     = eff
    o_recoeffstat[0] = (inverrsq**.5)*(eff**2)
    effcyr.SetBinContent(lbnum-lbnums[0]+1, eff)
    effcyr.SetBinError(lbnum-lbnums[0]+1, o_recoeffstat[0])


def template_method(hmo, hms, hno, hns, hto, hts, effcyr):
    noSign  = False
    doScale = False    
    
    tbin1 = hmo.GetXaxis().FindBin(75000)
    tbin2 = hmo.GetXaxis().FindBin(104000)
    tbin3 = hmo.GetXaxis().FindBin(120000)
    tbin4 = hmo.GetXaxis().FindBin(250000)

    scale_os = 1 if hto.Integral(tbin3, tbin4) == 0 else hno.Integral(tbin3, tbin4)/hto.Integral(tbin3, tbin4)
    scale_ss = 1 if hts.Integral(tbin3, tbin4) == 0 else hns.Integral(tbin3, tbin4)/hts.Integral(tbin3, tbin4)

    if doScale == True:
        hto.Scale(scale_os)
        hts.Scale(scale_ss)

    if noSign == True:
        hmo.Add(hms)
        hno.Add(hns)
        hto.Add(hts)

    matchos_peak, matchos_peakerr = extract(hmo, tbin1, tbin2)
    matchos_tail, matchos_tailerr = extract(hmo, tbin3, tbin4)
    matchss_peak, matchss_peakerr = extract(hms, tbin1, tbin2)
    matchss_tail, matchss_tailerr = extract(hms, tbin3, tbin4)

    nomatchos_peak, nomatchos_peakerr = extract(hno, tbin1, tbin2)
    nomatchos_tail, nomatchos_tailerr = extract(hno, tbin3, tbin4)
    nomatchss_peak, nomatchss_peakerr = extract(hns, tbin1, tbin2)
    nomatchss_tail, nomatchss_tailerr = extract(hns, tbin3, tbin4)

    templateos_peak, templateos_peakerr = extract(hto, tbin1, tbin2)
    templateos_tail, templateos_tailerr = extract(hto, tbin3, tbin4)
    templatess_peak, templatess_peakerr = extract(hts, tbin1, tbin2)
    templatess_tail, templatess_tailerr = extract(hts, tbin3, tbin4)

    totalos_peak = matchos_peak + nomatchos_peak
    totalos_tail = matchos_tail + nomatchos_tail
    totalss_peak = matchss_peak + nomatchss_peak
    totalss_tail = matchss_tail + nomatchss_tail

    totalos_peakerr = pow(totalos_peak, 0.5)
    totalss_peakerr = pow(totalss_peak, 0.5)
    totalos_tailerr = pow(totalos_tail, 0.5)
    totalss_tailerr = pow(totalss_tail, 0.5)

    if templatess_tail == 0:    
        eff = 1
        err = 1
    elif templateos_tail == 0:
        eff = 1
        err = 1

    if templatess_tail != 0 and templateos_tail != 0:
        n1 = matchos_peak
        n2 = (templateos_peak*matchss_tail)/templatess_tail
        d1 = totalos_peak
        d2 = (nomatchos_tail*templateos_peak)/templateos_tail

        eff = (n1 - n2)/(d1- d2)
        err = template_method_error(hmo, hms, hno, hns, hto, hts)   
 

    o_recoeff[0]     = eff
    o_recoeffstat[0] = err
    effcyr.SetBinContent(lbnum-lbnums[0]+1, eff)
    effcyr.SetBinError(lbnum-lbnums[0]+1, err)


def template_method_error(hmo, hms, hno, hns, hto, hts):
    bin1 = hmo.GetXaxis().FindBin(75000)
    bin2 = hmo.GetXaxis().FindBin(104000)
    bin3 = hmo.GetXaxis().FindBin(120000)
    bin4 = hmo.GetXaxis().FindBin(250000)

    a, da = extract(hmo, bin1, bin2)
    b, db = extract(hms, bin3, bin4)
    c, dc = extract(hto, bin1, bin2)
    d, dd = extract(hts, bin3, bin4)
    e, de = extract(hno, bin1, bin2)
    f, df = extract(hno, bin3, bin4)
    g, dg = extract(hto, bin3, bin4)

    try:
        dva = 1/(a-(c*f)/g+e)-(a-(b*c)/d)/(a-(c*f)/g+e)**2
    except ZeroDivisionError:
        return 1
    try:
        dvb = -c/(d*(-(c*f)/g+a+e))
    except ZeroDivisionError:
        return 1    
    try:
        dvc = -(g*((a+e)*b*g-a*d*f))/(d*(f*c+(-a-e)*g)**2)
    except ZeroDivisionError:
        return 1
    try:
        dvd = (b*c)/((-(c*f)/g+a+e)*d**2)
    except ZeroDivisionError:
        return 1
    try:
        dve = -(a-(b*c)/d)/(e-(c*f)/g+a)**2
    except ZeroDivisionError:
        return 1
    try:
        dvf = (c*(a-(b*c)/d))/(g*(-(c*f)/g+a+e)**2)
    except ZeroDivisionError:
        return 1
    try:
        dvg = -(c*(a*d-b*c)*f)/(d*((a+e)*g-c*f)**2)
    except ZeroDivisionError:
        return 1

    verbose = False
    if verbose == True:
        print a, da, dva, da*dva
        print b, db, dvb, db*dvb
        print c, dc, dvc, dc*dvc
        print d, dd, dvd, dd*dvd
        print e, de, dve, de*dve
        print f, df, dvf, df*dvf
        print g, dg, dvg, dg*dvg

    err = math.sqrt((da*dva)**2 + (db*dvb)**2 + (dc*dvc)**2 + (dd*dvd)**2 + (de*dve)**2 + (df*dvf)**2 + (dg*dvg)**2)
    return err

def container_efficiency(h_photon, h_pass, h_tos, h_tss):
    h_temp = h_tos.Clone()
    h_temp.Add(h_tss)

    bin1 = h_pass.FindBin(120000)
    bin2 = h_pass.FindBin(250000)
    
    if h_pass.Integral(bin1, bin2) != 0 and h_temp.Integral(bin1, bin2) != 0:
        scale = h_pass.Integral(bin1, bin2)/h_temp.Integral(bin1, bin2)
        h_temp.Scale(scale)
        scale_err = scale*pow((1/math.sqrt(h_pass.Integral(bin1, bin2)))+(1/math.sqrt(h_temp.Integral(bin1, bin2))), 0.5)
    else: 
        scale = 1
        scale_err = 1

    bin66  = h_photon.FindBin(66000)
    bin75  = h_photon.FindBin(75000)
    bin100 = h_photon.FindBin(100000)
    bin116 = h_photon.FindBin(116000)

    N6675   = h_photon.Integral(bin66, bin75)
    N100116 = h_photon.Integral(bin100, bin116)
    sigW    = 96 - 86
    bkgW    = (75 - 66) + (116 - 100)

    bkg = (N6675+N100116)*sigW/bkgW
    bkgerr = pow(N6675 + N100116, 0.5)*sigW/bkgW

    bin86 = h_pass.FindBin(86000)
    bin96 = h_pass.FindBin(96000)
    
    passval = h_pass.Integral(bin86, bin96) - h_temp.Integral(bin86, bin96)
    passerr = pow(h_pass.Integral(bin86, bin96) + math.sqrt(scale)*h_temp.Integral(bin86, bin96) + scale_err**2, 0.5)
   
    photonval = h_photon.Integral(bin86, bin96)
    photonerr = pow(h_photon.Integral(bin86, bin96), 0.5)

    a = passval
    b = photonval
    c = bkg
    da = passerr
    db = photonerr
    dc = bkgerr

    try:
        dda = (b-c)/(b-c+a)**2
    except ZeroDivisionError:
        dda = 1
    try:
        ddb = -a/(b-c+a)**2
    except ZeroDivisionError:
        ddb = 1
    try:
        ddc = a/(b-c+a)**2
    except ZeroDivisionError:
        ddc = 1

    o_conteff[0] = passval/(photonval - bkg + passval)
    o_conteffstat[0] = math.sqrt((da*dda)**2 + (db*ddb)**2 + (dc*ddc)**2)


def extract(histogram, bin1, bin2):
    dbl     = ROOT.Double()
    rv1     = histogram.IntegralAndError(bin1, bin2, dbl)
    return (rv1, float(dbl))


print 'Running in', runmode, 'mode'
if runmode == "Zmumu":
    ACCEPTANCE = 0.3323224
if runmode == "Zee":
    ACCEPTANCE = 0.2996


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

effcyt   = ROOT.TH1F('effcyt', 'Trigger efficiency', lbnums[-1]-lbnums[0]+1, lbnums[0]-0.5, lbnums[-1]+0.5)
effcyr   = ROOT.TH1F('effcyr', 'Loose electron reco efficiency', lbnums[-1]-lbnums[0]+1, lbnums[0]-0.5, lbnums[-1]+0.5)
effcya   = ROOT.TH1F('effcya', 'Combined acc x efficiency', lbnums[-1]-lbnums[0]+1, lbnums[0]-0.5, lbnums[-1]+0.5)
effcydir = ROOT.TH1F('effcydir', 'Direct acc x efficiency', lbnums[-1]-lbnums[0]+1, lbnums[0]-0.5, lbnums[-1]+0.5)

fout = ROOT.TFile(args.out if args.out else '%s_all.root' % runname[4:], 'RECREATE')
o_run           = array('I', [0])
o_lb            = array('I', [0])
o_lbwhen        = array('d', [0., 0.])
o_z_one         = array('f', [0.])
o_z_two         = array('f', [0.])
o_trigeff       = array('f', [0.])
o_trigeffstat   = array('f', [0.])
o_recoeff       = array('f', [0.])
o_recoeffstat   = array('f', [0.])
o_conteff       = array('f', [0.])
o_conteffstat   = array('f', [0.])
o_alleff        = array('f', [0.])
o_alleffstat    = array('f', [0.])
o_ae            = array('f', [0.])
o_aestat        = array('f', [0.])

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
tl.Branch('conteff', o_conteff, 'conteff/F')
tl.Branch('conteffstat', o_conteffstat, 'conteffstat/F')
tl.Branch('alleff', o_alleff, 'alleff/F')
tl.Branch('alleffstat', o_alleffstat, 'alleffstat/F')
tl.Branch('ae', o_ae, 'ae/F')
tl.Branch('aestat', o_aestat, 'aestat/F')


lblb = fetch_iovs("LBLB", runs=int(runname[4:])).by_run
for lb in sorted(lbdirs):
    if runmode == "Zee":
        h = infile.Get('%s/%s/GLOBAL/DQTGlobalWZFinder/m_eltrigtp_matches_os' % (runname, lb)) 
        hmo = infile.Get('%s/%s/GLOBAL/DQTGlobalWZFinder/m_ele_tight_good_os' % (runname, lb))
        hms = infile.Get('%s/%s/GLOBAL/DQTGlobalWZFinder/m_ele_tight_good_ss' % (runname, lb))
        hno = infile.Get('%s/%s/GLOBAL/DQTGlobalWZFinder/m_ele_tight_bad_os' % (runname, lb))
        hns = infile.Get('%s/%s/GLOBAL/DQTGlobalWZFinder/m_ele_tight_bad_ss' % (runname, lb))
        hto = infile.Get('%s/%s/GLOBAL/DQTGlobalWZFinder/m_ele_template_os' % (runname, lb))
        hts = infile.Get('%s/%s/GLOBAL/DQTGlobalWZFinder/m_ele_template_ss' % (runname, lb))
        hphoton = infile.Get("%s/%s/GLOBAL/DQTGlobalWZFinder/m_elContainertp_nomatch" % (runname, lb))
        hpass   = infile.Get("%s/%s/GLOBAL/DQTGlobalWZFinder/m_ele_tight_passkine" % (runname, lb))

    if runmode == "Zmumu":
        h = infile.Get('%s/%s/GLOBAL/DQTGlobalWZFinder/m_mutrigtp_matches' % (runname, lb))
        hmo = infile.Get('%s/%s/GLOBAL/DQTGlobalWZFinder/m_muloosetp_match_os' % (runname, lb))
        hms = infile.Get('%s/%s/GLOBAL/DQTGlobalWZFinder/m_muloosetp_match_ss' % (runname, lb))
        hno = infile.Get('%s/%s/GLOBAL/DQTGlobalWZFinder/m_muloosetp_nomatch_os' % (runname, lb))
        hns = infile.Get('%s/%s/GLOBAL/DQTGlobalWZFinder/m_muloosetp_nomatch_ss' % (runname, lb))

    lbnum  = int(lb[3:])
    trig_tag_and_probe(h, effcyt)
    if runmode == "Zmumu":
        reco_tag_and_probe(hmo, hms, hno, hns, effcyr)   
    elif runmode == "Zee": 
        template_method(hmo, hms, hno, hns, hto, hts, effcyr)

    o_ae[0]         = ACCEPTANCE*(1-(1-o_trigeff[0])**2)*(o_recoeff[0])**2
    o_aestat[0]     = ACCEPTANCE*((o_recoeff[0]**2*2*(1-o_trigeff[0])*o_trigeffstat[0])**2+(2*o_recoeff[0]*(1-(1-o_trigeff[0])**2)*o_recoeffstat[0])**2)**.5
    o_alleff[0]     = (1-(1-o_trigeff[0])**2)*(o_recoeff[0])**2
    o_alleffstat[0] = ((o_recoeff[0]**2*2*(1-o_trigeff[0])*o_trigeffstat[0])**2+(2*o_recoeff[0]*(1-(1-o_trigeff[0])**2)*o_recoeffstat[0])**2)**.5
    
    effcya.SetBinContent(lbnum-lbnums[0]+1, o_ae[0])
    effcya.SetBinError(lbnum-lbnums[0]+1, o_aestat[0])


    tl.Fill()
tl.Write()
print 'Done'

c1 = ROOT.TCanvas()
c1.SetBatch(ROOT.kTRUE)
effcya.SetMarkerStyle(21)
effcya.SetMarkerColor(ROOT.kBlue)
effcya.GetYaxis().SetRangeUser(0, 0.4)
effcya.Draw('PE')
c1.Print(os.path.join(args.plotdir, '%s_combined_efficiency.eps' % runname[4:]))
fout.WriteTObject(effcya)
c1.Clear()
effcyt.SetMarkerStyle(21)
effcyt.SetMarkerColor(ROOT.kBlue)
effcyt.GetYaxis().SetRangeUser(0.6,1.0)
effcyt.Draw('PE')
effcyt.Fit('pol1')
c1.Print(os.path.join(args.plotdir, '%s_trigger_efficiency.eps' % runname[4:]))
fout.WriteTObject(effcyt)
c1.Clear()
effcyr.SetMarkerStyle(21)
effcyr.SetMarkerColor(ROOT.kBlue)
effcyr.GetYaxis().SetRangeUser(0.7,1.0)
effcyr.Draw('PE')
effcyr.Fit('pol1')
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
        if sumweights[ibin] == 0: continue
        p = ctr[ctrbin]/sumweights[ibin]
        o_alleff[0]=p
        try:
            o_alleffstat[0]=(p*(1-p))**.5*(sumweights.GetBinError(ibin)/sumweights[ibin])
        except ValueError:
            o_alleffstat[0]=(sumweights.GetBinError(ibin)/sumweights[ibin])
        effcydir.SetBinContent(effcydir.FindBin(o_lb[0]), p)
        effcydir.SetBinError(effcydir.FindBin(o_lb[0]), o_alleffstat[0])

    effcya.GetYaxis().SetRangeUser(0.00,0.50)
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
