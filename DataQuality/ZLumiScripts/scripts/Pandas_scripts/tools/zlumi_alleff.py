#!/usr/bin/env python
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration                   

import sys, os, glob
import ROOT
import math
from array import array

def trig_tag_and_probe(h, lb_minus_one_trig_eff):
    yld    = (h[2], h[3])
    ylderr = (h.GetBinError(2), h.GetBinError(3))
    A, B   = yld

    if B == 0:
        return lb_minus_one_trig_eff[0], lb_minus_one_trig_eff[1]
    
    eff = 1./(float(A)/B/2.+1.)
    inverrsq = ((1/2./B)*ylderr[0])**2+((A/2./B**2)*ylderr[1])**2
    return eff, (inverrsq**.5)*(eff**2)


def reco_tag_and_probe(hmo, hms, hno, hns, lb_minus_one_reco_eff):
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
        eff = lb_minus_one_reco_eff[0]
        err = lb_minus_one_reco_eff[1]
        #print("Using LB-1 reco eff - muons", lb_minus_one_reco_eff[2], eff, err)
    else:
        eff = 1./(1.+B/A)
        inverrsq = ((-B/A**2)*Aerr)**2+((1./A)*Berr)**2
        err = (inverrsq**.5)*(eff**2)

    return eff, err


def template_method(hmo, hms, hno, hns, hto, hts, do_scale, lb_minus_one_reco_eff, corr = None):
    no_sign  = False
    
    tbin1 = hmo.GetXaxis().FindBin(75000)
    tbin2 = hmo.GetXaxis().FindBin(104000)
    tbin3 = hmo.GetXaxis().FindBin(120000)
    tbin4 = hmo.GetXaxis().FindBin(250000)

    if no_sign == True:
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
        eff = lb_minus_one_reco_eff[0]
        err = lb_minus_one_reco_eff[1]
        #print("Using LB-1 reco eff - electrons", lb_minus_one_reco_eff[2], eff, err)
    elif templateos_tail == 0:
        eff = lb_minus_one_reco_eff[0]
        err = lb_minus_one_reco_eff[1]
        #print("Using LB-1 reco eff - electrons", lb_minus_one_reco_eff[2], eff, err)

    if templatess_tail != 0 and templateos_tail != 0:
        n1 = matchos_peak
        n2 = templateos_peak*(matchss_tail/templatess_tail)
        d1 = totalos_peak
        if not do_scale: 
            d2 = templateos_peak*(nomatchos_tail/templateos_tail)
        else:
            d2 = templateos_peak*( (totalos_tail - matchos_tail/corr) /templateos_tail )

        try: 
            eff = (n1 - n2)/(d1 - d2)
            err = template_method_error(hmo, hms, hno, hns, hto, hts)   
        except ZeroDivisionError: 
            eff = lb_minus_one_reco_eff[0]
            err = lb_minus_one_reco_eff[1]
            #print("Using LB-1 reco eff - electrons", lb_minus_one_reco_eff[2], eff, err)


    return eff, err
    

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
        print(a, da, dva, da*dva)
        print(b, db, dvb, db*dvb)
        print(c, dc, dvc, dc*dvc)
        print(d, dd, dvd, dd*dvd)
        print(e, de, dve, de*dve)
        print(f, df, dvf, df*dvf)
        print(g, dg, dvg, dg*dvg)

    err = math.sqrt((da*dva)**2 + (db*dvb)**2 + (dc*dvc)**2 + (dd*dvd)**2 + (de*dve)**2 + (df*dvf)**2 + (dg*dvg)**2)
    return err

def container_efficiency(h_photon, h_photon_total_input, h_fit, h_pass, h_tos, h_tss):
    h_photon_total = h_photon_total_input.Clone()
    h_temp = h_tos.Clone()
    h_temp.Add(h_tss)

    h_bkg = h_fit.Clone()
    h_bkg.Reset()
    for xbin in range(1, h_bkg.GetNbinsX()+1): 
        mass = h_bkg.GetBinCenter(xbin)
        h_bkg.SetBinContent(xbin, h_fit.GetFunction("pol2").Eval(mass))
        # This is not correct, but use for now as a proxy -> not sure how to return error from TF1
        if h_fit.GetFunction("pol2").Eval(mass) < 0: 
            print("Negative fit!", xbin, h_fit.GetFunction("pol2").Eval(mass))
            h_bkg.SetBinError(xbin, 0)
        else: 
            h_bkg.SetBinError(xbin, pow(h_fit.GetFunction("pol2").Eval(mass), 0.5))


    bkg, bkgerr = extract(h_bkg, h_bkg.FindBin(86000), h_bkg.FindBin(96000))
    
    # Test with no bkg subtraciton
    #bkg = 0
    #bkgerr = 0
    
    bin1 = h_pass.FindBin(120000)
    bin2 = h_pass.FindBin(250000)

    bin86 = h_pass.FindBin(86000)
    bin96 = h_pass.FindBin(96000)

    xmin = 1
    xmax = h_photon.GetNbinsX()
   
    a, da = extract(h_pass, bin86, bin96)
    b, db = extract(h_temp, bin86, bin96)
    c, dc = extract(h_photon_total, bin86, bin96)
    d, dd = bkg, bkgerr

    s1, ds1 = extract(h_pass, bin1, bin2)
    s2, ds2 = extract(h_temp, bin1, bin2)
    s3, ds3 = extract(h_photon, xmin, xmax)
    s4, ds4 = extract(h_photon_total, xmin, xmax)
   
    numer = a - (b * s1/s2)
    denom = s3/s4 * (c - d)

    v2 = pow(numer + denom, 2)
    try:
        dda = denom/v2
    except ZeroDivisionError:
        dda = 1
    try:
        ddb = (s1/s2)*(-denom)/v2
    except ZeroDivisionError:
        ddb = 1
    try:
        ddc = -(s3/s4) * numer/v2
    except ZeroDivisionError:
        ddc = 1
    try:
        ddd = (s3/s4) * numer/v2
    except ZeroDivisionError:
        ddd = 1
    try: 
        dds1 = (denom * b/s2)/v2
    except ZeroDivisionError:
        dds1 = 1
    try: 
        dds2 = (denom * b * s1/s2**2)/v2
    except ZeroDivisionError:
        dds2 = 1
    try: 
        dds3 = -(numer) * (denom)/s3 /v2
    except ZeroDivisionError:
        dds3 = 1
    try: 
        dds4 = -(numer) * (denom)/s4 /v2
    except ZeroDivisionError:
        dds4 = 1

    try: 
        conteff = numer / (numer + denom)
        conteffstat = math.sqrt((da*dda)**2 + (db*ddb)**2 + (dc*ddc)**2 + (dd*ddd)**2 + (ds1*dds1)**2 + (ds2*dds2)**2 + (ds3*dds3)**2 + (ds4*dds4)**2)
    except: 
        conteff = 0.0
        conteffstat = 0.0

    return conteff, conteffstat


def extract(histogram, bin1, bin2):
    error = 0
    for xbin in range(bin1, bin2): 
        error += pow(histogram.GetBinError(xbin), 2)
    error = math.sqrt(error)    

    return (histogram.Integral(bin1, bin2), error)

    #dbl = ROOT.Double()
    #rv1     = histogram.IntegralAndError(bin1, bin2, dbl)
    #return (rv1, float(dbl))
