#!/usr/bin/env python2

import math
from ROOT import *
from DQUtils import fetch_iovs
from DQUtils.iov_arrangement import inverse_lblb

def setAtlasStyle():
    gROOT.LoadMacro("/afs/cern.ch/user/m/miokeefe/AtlasStyle/AtlasStyle.C")
    SetAtlasStyle()


def drawAtlasLabel(x, y, text):
    gROOT.LoadMacro("/afs/cern.ch/user/m/miokeefe/AtlasStyle/AtlasLabels.C")
    ATLASLabel(x, y, text,1)


def drawText(x, y, text):
    gROOT.LoadMacro("/afs/cern.ch/user/m/miokeefe/AtlasStyle/AtlasUtils.C")
    myText(x, y, 1, text)


def calculate_chi2(histo1, histo2):
    
    chi = 0
    NDF = 0

    mean_x1 = 0
    mean_x2 = 0
    N = 0
    for mybin in range(1, histo1.GetNbinsX()+1):
        mean_x1 += histo1.GetBinContent(mybin)
        mean_x2 += histo2.GetBinContent(mybin)
        if histo1.GetBinContent(mybin) != 0 and histo2.GetBinContent(mybin) != 0:
            N += 1

    mean_x1 = mean_x1/histo1.GetNbinsX()
    mean_x2 = mean_x2/histo2.GetNbinsX()

    for mybin in range(1, histo1.GetNbinsX()+1):
        x1 = histo1.GetBinContent(mybin)
        x2 = histo2.GetBinContent(mybin)
        
        numer = (x1 - x2)**2
        covariance = (x1 - mean_x1)*(x2 - mean_x2)/(N-1)
        denom = (histo2.GetBinError(mybin))**2 + (histo1.GetBinError(mybin))**2 - 2*covariance
        if (denom != 0 and numer !=0):
            chi += numer/denom
            NDF += 1

    NDF -= 1

    print "#########"
    print "chi2 = ", chi
    print "NDF =", NDF
    print "chi2/NDF = ", chi/NDF
    print "#########"



def plot_efficiency(histos, draw_options, legends, y_title, channel_label, filename):

    histos[0].GetXaxis().SetTitle("Position in train")
    histos[0].GetYaxis().SetTitle(y_title)
    histos[0].GetXaxis().SetRangeUser(1, 48)
    histos[0].GetYaxis().SetRangeUser(0.7, 1.0)

    colours = [kMagenta, kRed, kBlue, kGreen, kBlack]
    for histo in histos:
        histo.SetMarkerColor(colours[histos.index(histo)])
        histo.SetLineColor(colours[histos.index(histo)])
    
    leg = TLegend(0.75, 0.2, 0.93, 0.4)
    leg.SetBorderSize(0)
    for legend in legends:
        if draw_options[legends.index(legend)] == "hist":
            leg.AddEntry(histos[legends.index(legend)], legend, "l")
        else:
            leg.AddEntry(histos[legends.index(legend)], legend, draw_options[legends.index(legend)])

    c1 = TCanvas()
    histos[0].Draw("hist, " + draw_options[0])
    for x in range(1, len(histos)):
        histos[x].Draw("same, " + draw_options[x])

    leg.Draw()
    drawAtlasLabel(0.2, 0.35, "Work in progress")
    drawText(0.2, 0.30, channel_label)
    c1.Print(filename)


def plot_2D_eff(hnz, htr, hmo, hms, hno, hns, h_2D, channel):    
    bin1 = hmo.GetXaxis().FindBin(86000)
    bin2 = hmo.GetXaxis().FindBin(95000)

    for bin_bcid in range(1, 49):
        for bin_pileup in range(1, hmo.GetNbinsZ()+1):
            matchos   = 0
            matchss   = 0
            nomatchos = 0
            nomatchss = 0
            for bin_mass in range(bin1, bin2+1):
                matchos   += hmo.GetBinContent(bin_mass, bin_bcid, bin_pileup)
                matchss   += hms.GetBinContent(bin_mass, bin_bcid, bin_pileup)
                nomatchos += hno.GetBinContent(bin_mass, bin_bcid, bin_pileup)
                nomatchss += hns.GetBinContent(bin_mass, bin_bcid, bin_pileup)
            
            A = htr.GetBinContent(2, bin_bcid, bin_pileup)
            B = htr.GetBinContent(3, bin_bcid, bin_pileup)
        
            Aerr = htr.GetBinError(2, bin_bcid, bin_pileup)
            Berr = htr.GetBinError(3, bin_bcid, bin_pileup)
        
            if B == 0:
                continue

            trigeff = 1./(float(A)/B/2.+1.)
            inverrsq = ((1/2./B)*Aerr)**2+((A/2./B**2)*Berr)**2
            trigerr = (inverrsq**.5)*(trigeff**2)


            A    = float(matchos-matchss)
            Aerr = (matchos+matchss)**.5
            B    = float(nomatchos-nomatchss)
            Berr = (nomatchos+nomatchss)**.5

            if Berr == 0:
                Berr = 1.
            if A == 0 or B/A == -1:
                recoeff = 1.
                inverrsq = 1.
            else:
                recoeff = 1./(1.+B/A)
                inverrsq = ((-B/A**2)*Aerr)**2+((1./A)*Berr)**2

            recoerr = (inverrsq**.5)*(recoeff**2)

            eff = (1 - (1 - trigeff)**2) * recoeff**2
            err = ((recoeff**2*2*(1-trigeff)*trigerr)**2 + (2*recoeff*(1-(1-trigeff)**2)*recoerr)**2)**0.5

            h_2D.SetBinContent(bin_bcid, bin_pileup, eff)
            h_2D.SetBinError(bin_bcid, bin_pileup, err)

    h_2D.GetYaxis().SetTitle("Pileup")
    h_2D.GetXaxis().SetTitle("Position in train")
    h_2D.GetYaxis().SetRangeUser(0, 70)
    h_2D.GetXaxis().SetRangeUser(1, 48)
    h_2D.GetZaxis().SetRangeUser(0.5, 1.0)

    c1 = TCanvas()
    h_2D.Draw("colz")
    gPad.SetRightMargin(0.15)
    c1.Print("2D_eff_"+channel+".eps")
  
    cross_sec = 1.929
    purity = 0.9935
    if channel == "zmm":
        acceptance = 0.3323224
    if channel == "zee":
        acceptance = 0.2996


    n_events_corrected = 0
    err = 0
    for xbin in range(1, hnz.GetNbinsX()+1):
        for ybin in range(1, hnz.GetNbinsY()+1):
            mu = hnz.GetYaxis().GetBinCenter(ybin)
            if channel == "zee":
                corr = 9.02495e-01 + 2.15661e-05*mu - 7.04916e-06*mu*mu
            if channel == "zmm":
                corr = 9.90808e-01 - 9.99749e-05*mu - 1.40241e-06*mu*mu

            n_events_corrected += hnz.GetBinContent(xbin, ybin) * h_2D.GetBinContent(xbin, ybin) * corr
            try:
                err += n_events_corrected * math.sqrt((hnz.GetBinError(xbin, ybin)/hnz.GetBinContent(xbin, ybin))**2 + (h_2D.GetBinError(xbin, ybin)/h_2D.GetBinContent(xbin, ybin))**2)
            except ZeroDivisionError:
                err += 0

    err = math.sqrt(err)
    lumi = n_events_corrected*purity/acceptance/cross_sec
    print "Lumi =", round(lumi/1000000, 4), "+/-", round(err/1000000, 4)

    return


def plot_deviation(h_inclusive, channel_label, y_label, filename):
    # check for deviation
    # calculate average over "stable" range, 
    # then calculate ratio
    average = 0
    nbins= 0
    for bin in range(5, h_inclusive.GetNbinsX()+1):
        if h_inclusive.GetBinContent(bin) != 0:
            average += h_inclusive.GetBinContent(bin)
            nbins += 1

    average = average/(nbins)

    # calculate standard deviation
    stdev = 0
    for x in range(5, h_inclusive.GetNbinsX()+1):
        if h_inclusive.GetBinContent(x) != 0:
            stdev += pow(h_inclusive.GetBinContent(x) - average, 2)

    stdev = math.sqrt(stdev/(nbins-1))

    line1 = TLine(1, stdev, 48, stdev)
    line2 = TLine(1, -stdev, 48, -stdev)
    line1.SetLineStyle(3)
    line2.SetLineStyle(3)
    line1.SetLineWidth(2)
    line2.SetLineWidth(2)

    h_inclusive.Scale(1/average)
    h_inclusive.GetYaxis().SetTitle(y_label)
    h_inclusive.GetXaxis().SetTitle("Position in train")

    h_temp = h_inclusive.Clone()
    h_temp.Divide(h_inclusive)
    h_inclusive.Add(h_temp, -1)
    
    h_inclusive.GetYaxis().SetRangeUser(-0.05, 0.05)
    h_inclusive.GetXaxis().SetRangeUser(1, 48)

    c1 = TCanvas()
    h_inclusive.Draw("ep")
    line1.Draw("same")
    line2.Draw("same")
    h_inclusive.Draw("same, ep")
    drawAtlasLabel(0.2, 0.35, "Work in progress")
    drawText(0.2, 0.30, channel_label)
    c1.Print(filename)


def BCID_reco_vs_pileup(hmo, hms, hno, hns, histo):
    
    bin1 = hmo.GetXaxis().FindBin(86000)
    bin2 = hmo.GetXaxis().FindBin(95000)

    merge_bins = 0
    matchos = 0
    matchss = 0
    nomatchos = 0
    nomatchss = 0

    for bin_pileup in range(1, hmo.GetNbinsZ()+1):
        for bin_bcid in range(1, 49):
            for bin_mass in range(bin1, bin2+1):
                matchos += hmo.GetBinContent(bin_mass, bin_bcid, bin_pileup)
                matchss += hms.GetBinContent(bin_mass, bin_bcid, bin_pileup)
                nomatchos += hno.GetBinContent(bin_mass, bin_bcid, bin_pileup)
                nomatchss += hns.GetBinContent(bin_mass, bin_bcid, bin_pileup)
        
        merge_bins += 1
        if merge_bins % 5 != 0: 
            continue

        A    = float(matchos-matchss)
        Aerr = (matchos+matchss)**.5
        B    = float(nomatchos-nomatchss)
        Berr = (nomatchos+nomatchss)**.5

        if Berr == 0:
            Berr = 1.
        if A == 0 or B/A == -1:
            eff = 0.0
            inverrsq = 0.0
        else:
            eff = 1./(1.+B/A)
            inverrsq = ((-B/A**2)*Aerr)**2+((1./A)*Berr)**2

        err = (inverrsq**.5)*(eff**2)

        histo.SetBinContent(bin_pileup, eff)
        histo.SetBinError(bin_pileup, err)

        matchos = 0
        matchss = 0
        nomatchos = 0
        nomatchss = 0

    return


def BCID_plot_N1_N2(h_input, h_N1, h_N2):
    
    for bin_pileup in range(1, h_input.GetNbinsZ()+1):
        N1 = 0
        N2 = 0
        for bin_bcid in range(1, 49):
            N1 += h_input.GetBinContent(2, bin_bcid, bin_pileup)
            N2 += h_input.GetBinContent(3, bin_bcid, bin_pileup)
       
        h_N1.SetBinContent(bin_pileup, N1)
        h_N1.SetBinError(bin_pileup, math.sqrt(N1))
        h_N2.SetBinContent(bin_pileup, N2)
        h_N2.SetBinError(bin_pileup, math.sqrt(N2))

    return


def BCID_trig_tag_and_probe(h, lower_z_bin, upper_z_bin, histo):
   
    for bin_bcid in range(1, 49):
        A = 0
        B = 0
        Aerr = 0
        Berr = 0
        for bin_pileup in range(lower_z_bin, upper_z_bin):
            A += h.GetBinContent(2, bin_bcid, bin_pileup)
            B += h.GetBinContent(3, bin_bcid, bin_pileup)
        
            Aerr += h.GetBinError(2, bin_bcid, bin_pileup)**2
            Berr += h.GetBinError(3, bin_bcid, bin_pileup)**2

        if B == 0:
            continue

        Aerr = math.sqrt(Aerr)
        Berr = math.sqrt(Berr)

        eff = 1./(float(A)/B/2.+1.)
        inverrsq = ((1/2./B)*Aerr)**2+((A/2./B**2)*Berr)**2
        err = (inverrsq**.5)*(eff**2)

        histo.SetBinContent(bin_bcid, eff)
        histo.SetBinError(bin_bcid, err)

    return 

def BCID_template_method(hmo, hms, hno, hns, hto, hts, lower_z_bin, upper_z_bin, histo):
    noSign  = False
    doScale = False    
    
    tbin1 = hmo.GetXaxis().FindBin(75000)
    tbin2 = hmo.GetXaxis().FindBin(104000)
    tbin3 = hmo.GetXaxis().FindBin(120000)
    tbin4 = hmo.GetXaxis().FindBin(250000)

    for bin_bcid in range(1, 49):
        scale_os = 1 if hto.Integral(tbin3, tbin4) == 0 else hno.Integral(tbin3, tbin4)/hto.Integral(tbin3, tbin4)
        scale_ss = 1 if hts.Integral(tbin3, tbin4) == 0 else hns.Integral(tbin3, tbin4)/hts.Integral(tbin3, tbin4)

        if doScale == True:
            hto.Scale(scale_os)
            hts.Scale(scale_ss)

        matchos_peak = 0
        matchss_peak = 0
        nomatchos_peak = 0
        nomatchss_peak = 0
        templateos_peak = 0
        templatess_peak = 0
        matchos_tail = 0
        matchss_tail = 0
        nomatchos_tail = 0
        nomatchss_tail = 0
        templateos_tail = 0
        templatess_tail = 0
        for bin_pileup in range(lower_z_bin, upper_z_bin):
            for bin_mass in range(tbin1, tbin2):
                matchos_peak += hmo.GetBinContent(bin_mass, bin_bcid, bin_pileup)
                matchss_peak += hms.GetBinContent(bin_mass, bin_bcid, bin_pileup)
                nomatchos_peak += hno.GetBinContent(bin_mass, bin_bcid, bin_pileup)
                nomatchss_peak += hns.GetBinContent(bin_mass, bin_bcid, bin_pileup)
                templateos_peak += hto.GetBinContent(bin_mass, bin_bcid, bin_pileup)
                templatess_peak += hts.GetBinContent(bin_mass, bin_bcid, bin_pileup)

            for bin_mass in range(tbin3, tbin4):
                matchos_tail += hmo.GetBinContent(bin_mass, bin_bcid, bin_pileup)
                matchss_tail += hms.GetBinContent(bin_mass, bin_bcid, bin_pileup)
                nomatchos_tail += hno.GetBinContent(bin_mass, bin_bcid, bin_pileup)
                nomatchss_tail += hns.GetBinContent(bin_mass, bin_bcid, bin_pileup)
                templateos_tail += hto.GetBinContent(bin_mass, bin_bcid, bin_pileup)
                templatess_tail += hts.GetBinContent(bin_mass, bin_bcid, bin_pileup)

        totalos_peak = matchos_peak + nomatchos_peak
        totalos_tail = matchos_tail + nomatchos_tail
        totalss_peak = matchss_peak + nomatchss_peak
        totalss_tail = matchss_tail + nomatchss_tail
        
        a, da = matchos_peak, math.sqrt(matchos_peak)
        b, db = matchss_tail, math.sqrt(matchss_tail)
        c, dc = templateos_peak, math.sqrt(templateos_peak)
        d, dd = templatess_tail, math.sqrt(templatess_tail)
        e, de = nomatchos_peak, math.sqrt(nomatchos_peak)
        f, df = nomatchss_tail, math.sqrt(nomatchss_tail)
        g, dg = templateos_tail, math.sqrt(templateos_tail)

        if templatess_tail == 0:    
            eff = 1 
            err = 1 
        if templateos_tail == 0:
            eff = 1 
            err = 1 

        if templatess_tail != 0 and templateos_tail != 0:
            n1 = matchos_peak
            n2 = (templateos_peak*matchss_tail)/templatess_tail
            d1 = totalos_peak
            d2 = (nomatchos_tail*templateos_peak)/templateos_tail
            eff = (n1 - n2)/(d1- d2) 
            
            dva = 1/(a-(c*f)/g+e)-(a-(b*c)/d)/(a-(c*f)/g+e)**2
            dvb = -c/(d*(-(c*f)/g+a+e))
            dvc = -(g*((a+e)*b*g-a*d*f))/(d*(f*c+(-a-e)*g)**2)
            dvd = (b*c)/((-(c*f)/g+a+e)*d**2)
            dve = -(a-(b*c)/d)/(e-(c*f)/g+a)**2
            dvf = (c*(a-(b*c)/d))/(g*(-(c*f)/g+a+e)**2)
            dvg = -(c*(a*d-b*c)*f)/(d*((a+e)*g-c*f)**2)
            err = math.sqrt((da*dva)**2 + (db*dvb)**2 + (dc*dvc)**2 + (dd*dvd)**2 + (de*dve)**2 + (df*dvf)**2 + (dg*dvg)**2)
     
        histo.SetBinContent(bin_bcid, eff)
        histo.SetBinError(bin_bcid, err)

    return


def BCID_reco_tag_and_probe(hmo, hms, hno, hns, lower_z_bin, upper_z_bin, histo):    
    bin1 = hmo.GetXaxis().FindBin(86000)
    bin2 = hmo.GetXaxis().FindBin(95000)

    for bin_bcid in range(1, 49):
        matchos = 0
        matchss = 0
        nomatchos = 0
        nomatchss = 0
        for bin_pileup in range(lower_z_bin, upper_z_bin):
            for bin_mass in range(bin1, bin2+1):
                matchos += hmo.GetBinContent(bin_mass, bin_bcid, bin_pileup)
                matchss += hms.GetBinContent(bin_mass, bin_bcid, bin_pileup)
                nomatchos += hno.GetBinContent(bin_mass, bin_bcid, bin_pileup)
                nomatchss += hns.GetBinContent(bin_mass, bin_bcid, bin_pileup)

        A    = float(matchos-matchss)
        Aerr = (matchos+matchss)**.5
        B    = float(nomatchos-nomatchss)
        Berr = (nomatchos+nomatchss)**.5

        if Berr == 0:
            Berr = 1.
        if A == 0 or B/A == -1:
            eff = 1.
            inverrsq = 1.
        else:
            eff = 1./(1.+B/A)
            inverrsq = ((-B/A**2)*Aerr)**2+((1./A)*Berr)**2

        err = (inverrsq**.5)*(eff**2)

        histo.SetBinContent(bin_bcid, eff)
        histo.SetBinError(bin_bcid, err)
    
    return

def main(filename, run_number, channel):
    rfile = TFile(filename)
    runname = "run_" + run_number
    rfile.cd(runname)
    keys = sorted([key.GetName() for key in gDirectory.GetListOfKeys() if key.GetName().startswith("lb")])
   
    ZXSEC         = 1.929
    ZPURITYFACTOR = 0.9935
    
    if channel == "zee":
        ACCEPTANCE = 0.2996
        hnz = rfile.Get('%s/GLOBAL/DQTGlobalWZFinder/m_BCID_pileup_zee' % (runname)) 
        htr = rfile.Get('%s/GLOBAL/DQTGlobalWZFinder/m_eltrigtp_matches_os_BCID_pileup' % (runname)) 
        hmo = rfile.Get('%s/GLOBAL/DQTGlobalWZFinder/m_ele_tight_good_os_BCID_pileup' % (runname))
        hms = rfile.Get('%s/GLOBAL/DQTGlobalWZFinder/m_ele_tight_good_ss_BCID_pileup' % (runname))
        hno = rfile.Get('%s/GLOBAL/DQTGlobalWZFinder/m_ele_tight_bad_os_BCID_pileup' % (runname))
        hns = rfile.Get('%s/GLOBAL/DQTGlobalWZFinder/m_ele_tight_bad_ss_BCID_pileup' % (runname))
        hto = rfile.Get('%s/GLOBAL/DQTGlobalWZFinder/m_ele_template_os_BCID_pileup' % (runname))
        hts = rfile.Get('%s/GLOBAL/DQTGlobalWZFinder/m_ele_template_ss_BCID_pileup' % (runname))

    if channel == "zmm":
        ACCEPTANCE = 0.3323224
        hnz = rfile.Get('%s/GLOBAL/DQTGlobalWZFinder/m_BCID_pileup_zmm' % (runname)) 
        htr = rfile.Get('%s/GLOBAL/DQTGlobalWZFinder/m_mutrigtp_matches_BCID_pileup' % (runname)) 
        hmo = rfile.Get('%s/GLOBAL/DQTGlobalWZFinder/m_muloosetp_match_os_BCID_pileup' % (runname))
        hms = rfile.Get('%s/GLOBAL/DQTGlobalWZFinder/m_muloosetp_match_ss_BCID_pileup' % (runname))
        hno = rfile.Get('%s/GLOBAL/DQTGlobalWZFinder/m_muloosetp_nomatch_os_BCID_pileup' % (runname))
        hns = rfile.Get('%s/GLOBAL/DQTGlobalWZFinder/m_muloosetp_nomatch_ss_BCID_pileup' % (runname))

    h_trig_eff_0_mu_50    = TH1F("h_trig_eff_0_mu_50", "h_trig_eff_0_mu_50", 50, 1, 50)
    h_trig_eff_51_mu_100  = TH1F("h_trig_eff_51_mu_100", "h_trig_eff_51_mu_100", 50, 1, 50)
    h_trig_eff_101_mu_150 = TH1F("h_trig_eff_101_mu_150", "h_trig_eff_101_mu_150", 50, 1, 50)
    h_trig_eff_151_mu_200 = TH1F("h_trig_eff_151_mu_200", "h_trig_eff_151_mu_200", 50, 1, 50)
    h_trig_eff_full       = TH1F("h_trig_eff_full", "h_trig_eff_full", 50, 1, 50)

    bin1 = 40
    bin2 = 60
    step_size = 20
    BCID_trig_tag_and_probe(htr, bin1, bin2, h_trig_eff_0_mu_50)
    BCID_trig_tag_and_probe(htr, bin1+step_size, bin2+step_size, h_trig_eff_51_mu_100)
    BCID_trig_tag_and_probe(htr, bin1+(2*step_size), bin2+(2*step_size), h_trig_eff_101_mu_150)
    BCID_trig_tag_and_probe(htr, bin1+(3*step_size), bin2+(3*step_size), h_trig_eff_151_mu_200)
    BCID_trig_tag_and_probe(htr, 1, 200, h_trig_eff_full)
  

    if channel == "zee":
        channel_label = "Z #rightarrow ee counting"#, Run number " + run_number
    if channel == "zmm":
        channel_label = "Z #rightarrow #mu#mu counting"#, Run number " + run_number


    plot_efficiency([h_trig_eff_0_mu_50, h_trig_eff_51_mu_100, h_trig_eff_101_mu_150, h_trig_eff_151_mu_200, h_trig_eff_full], 
                    ["ep", "ep", "ep", "ep", "hist"], 
                    ["20 < #mu < 30", "30 < #mu < 40", "40 < #mu < 50", "50 < #mu < 60", "Inclusive"], 
                    "Trigger efficiency", 
                    channel_label, 
                    "trigger_eff_vs_BCID_pileup_"+channel+".eps")
    
    plot_efficiency([h_trig_eff_full], ["ep"], ["Inclusive"], "Trigger efficiency", channel_label, "trigger_eff_vs_BCID_pileup_"+channel+"_inclusive.eps")
    plot_deviation(h_trig_eff_full, channel_label, "Deviation (#varepsilon^{trig})", "trigger_deviation_"+channel+"_inclusive.eps")

    h_N1_pileup = TH1F("h_N1_pileup", "h_N1_pileup", 200, 1, 100)
    h_N2_pileup = TH1F("h_N2_pileup", "h_N2_pileup", 200, 1, 100)
    BCID_plot_N1_N2(htr, h_N1_pileup, h_N2_pileup)

    h_N2_pileup.SetMarkerColor(kRed)
    h_N2_pileup.SetLineColor(kRed)
    h_N2_pileup.GetYaxis().SetTitle("Entries")
    h_N2_pileup.GetXaxis().SetTitle("Average pileup")
    h_N2_pileup.GetXaxis().SetRangeUser(10, 70)
    h_N2_pileup.GetYaxis().SetRangeUser(0, 2000)

    leg = TLegend(0.75, 0.5, 0.93, 0.7)
    leg.SetBorderSize(0)
    leg.AddEntry(h_N1_pileup, "N_{1}^{trigger-matched}")
    leg.AddEntry(h_N2_pileup, "N_{2}^{trigger-matched}")

    c1 = TCanvas()
    h_N2_pileup.Draw("ep")
    h_N1_pileup.Draw("same, ep")
    leg.Draw()
    drawAtlasLabel(0.2, 0.85, "Work in progress")
    drawText(0.2, 0.80, channel_label)
    c1.Print("N1_N2_vs_pileup_"+channel+".eps")

    h_reco_eff_0_mu_50    = TH1F("h_reco_eff_0_mu_50", "h_reco_eff_0_mu_50", 50, 1, 50)
    h_reco_eff_51_mu_100  = TH1F("h_reco_eff_51_mu_100", "h_reco_eff_51_mu_100", 50, 1, 50)
    h_reco_eff_101_mu_150 = TH1F("h_reco_eff_101_mu_150", "h_reco_eff_101_mu_150", 50, 1, 50)
    h_reco_eff_151_mu_200 = TH1F("h_reco_eff_151_mu_200", "h_reco_eff_151_mu_200", 50, 1, 50)
    h_reco_eff_full       = TH1F("h_reco_eff_full", "h_reco_eff_full", 50, 1, 50)

    bin1 = 40
    bin2 = 60

    if channel == "zee":
        BCID_template_method(hmo, hms, hno, hns, hto, hts, bin1, bin2, h_reco_eff_0_mu_50)
        BCID_template_method(hmo, hms, hno, hns, hto, hts, bin1+20, bin2+20, h_reco_eff_51_mu_100)
        BCID_template_method(hmo, hms, hno, hns, hto, hts, bin1+40, bin2+40, h_reco_eff_101_mu_150)
        BCID_template_method(hmo, hms, hno, hns, hto, hts, bin1+60, bin2+60, h_reco_eff_151_mu_200)
        BCID_template_method(hmo, hms, hno, hns, hto, hts, 1, 200, h_reco_eff_full)
    elif channel == "zmm":
        BCID_reco_tag_and_probe(hmo, hms, hno, hns, bin1, bin2, h_reco_eff_0_mu_50)
        BCID_reco_tag_and_probe(hmo, hms, hno, hns, bin1+20, bin2+20, h_reco_eff_51_mu_100)
        BCID_reco_tag_and_probe(hmo, hms, hno, hns, bin1+40, bin2+40, h_reco_eff_101_mu_150)
        BCID_reco_tag_and_probe(hmo, hms, hno, hns, bin1+60, bin2+60, h_reco_eff_151_mu_200)
        BCID_reco_tag_and_probe(hmo, hms, hno, hns, 1, 200, h_reco_eff_full)
  

    plot_efficiency([h_reco_eff_0_mu_50, h_reco_eff_51_mu_100, h_reco_eff_101_mu_150, h_reco_eff_151_mu_200, h_reco_eff_full], 
                    ["ep", "ep", "ep", "ep", "hist"], 
                    ["20 < #mu < 30", "30 < #mu < 40", "40 < #mu < 50", "50 < #mu < 60", "Inclusive"], 
                    "Reconstruction efficiency", 
                    channel_label, 
                    "reco_eff_vs_BCID_pileup_"+channel+".eps")
    
    plot_efficiency([h_reco_eff_full], ["ep"], ["Inclusive"], "Reconstruction efficiency", channel_label, "reco_eff_vs_BCID_pileup_"+channel+"_inclusive.eps")
    plot_deviation(h_reco_eff_full, channel_label, "Deviation (#varepsilon^{reco})", "reco_deviation_"+channel+"_inclusive.eps")

    h_2D = TH2F("h_2D", "h_2D", 50, 0, 50, 200, 0, 100)
    plot_2D_eff(hnz, htr, hmo, hms, hno, hns, h_2D, channel)
    

if __name__=="__main__":
    gROOT.SetBatch(kTRUE)
    setAtlasStyle()
    main("output_merged.root", "merged", "zee")
    main("output_merged.root", "merged", "zmm")
