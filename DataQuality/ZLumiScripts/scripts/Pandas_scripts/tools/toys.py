#!/usr/bin/env python

import numpy as np

def nan_divide(numer, denom): 
    return np.divide(numer.astype(np.float), denom.astype(np.float), out=np.full(numer.size, np.nan), where=denom!=0)


def toy_trigeff(trig_n1, trig_n2, ntoys=10000):
    trig_n1 = np.random.poisson(trig_n1, ntoys)
    trig_n2 = np.random.poisson(trig_n2, ntoys)

    eff = np.divide(1.0, np.divide(nan_divide(trig_n1, trig_n2), 2.0) + np.full(ntoys, 1.0))
    nonan_eff = eff[~np.isnan(eff)]
    if nonan_eff.size > 0: 
        out_eff = np.median(nonan_eff)
        out_err = nonan_eff.std()
    else: 
        out_eff = 0.0
        out_err = 0.0

    return out_eff, out_err, eff, (trig_n1 + trig_n2)


def muon_toy_recoeff(matchos, matchss, nomatchos, nomatchss, ntoys=10000):
    matchos = np.random.poisson(matchos, ntoys)
    matchss = np.random.poisson(matchss, ntoys)
    nomatchos = np.random.poisson(nomatchos, ntoys)
    nomatchss = np.random.poisson(nomatchss, ntoys)

    numer = matchos - matchss
    denom = (matchos - matchss) + (nomatchos - nomatchss)

    eff = nan_divide(numer, denom)
    nonan_eff = eff[~np.isnan(eff)]
    if nonan_eff.size > 0: 
        out_eff = np.median(nonan_eff)
        out_err = nonan_eff.std()
    else: 
        out_eff = 0.0
        out_err = 0.0

    return out_eff, out_err, eff

def electron_toy_recoeff(matchos_peak, matchos_tail, matchss_tail, nomatchos_peak, nomatchos_tail, templateos_peak, templateos_tail, templatess_tail, ntoys=1000):
    matchos_peak = np.random.poisson(matchos_peak, ntoys)
    matchos_tail = np.random.poisson(matchos_tail, ntoys)
    matchss_tail = np.random.poisson(matchss_tail, ntoys)
    nomatchos_peak = np.random.poisson(nomatchos_peak, ntoys)
    nomatchos_tail = np.random.poisson(nomatchos_tail, ntoys)
    
    templateos_peak = np.random.poisson(templateos_peak, ntoys)
    templateos_tail = np.random.poisson(templateos_tail, ntoys)
    templatess_tail = np.random.poisson(templatess_tail, ntoys)
    
    totalos_peak = matchos_peak + nomatchos_peak
    totalos_tail = matchos_tail + nomatchos_tail

    n1 = matchos_peak
    n2 = np.multiply(templateos_peak, nan_divide(matchss_tail, templatess_tail))
    d1 = totalos_peak
    d2 = np.multiply(templateos_peak, nan_divide(nomatchos_tail, templateos_tail))
    iterative_eff = nan_divide((n1 - n2), (d1 - d2))

    d2 = np.multiply(templateos_peak, nan_divide((totalos_tail - nan_divide(matchos_tail, iterative_eff)), templateos_tail))
    eff = nan_divide((n1 - n2), (d1 - d2))
    nonan_eff = eff[~np.isnan(eff)]
    if nonan_eff.size > 0: 
        out_eff = np.median(nonan_eff)
        out_err = nonan_eff.std()
    else: 
        out_eff = 0.0
        out_err = 0.0

    return out_eff, out_err, eff
