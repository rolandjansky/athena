#!/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import os
CWD = os.getcwd()
from ROOT import gSystem

## Import the ROOT library for reading han results
gSystem.Load('libDataQualityUtils')
from ROOT import dqutils
os.chdir(CWD)

def ExtractMean( mean_rms ):
    global mean
    mean=""
    h=mean_rms.split('Mean:  ')
    if (mean_rms.find('Error')>-1):
        m=h[1].split('MeanError: ')
        mean=m[0]
    elif (mean_rms.find('Error')<0):    
        m=h[1].split('RMS: ')
        if (m[0].find('NBins')>-1):
            n=m[0].split('NBins: ')
            mean=n[0]
        else:
            mean=m[0]
    return mean

def ExtractEMean( mean_rms ):
    global EMean
    EMean=""
    h=mean_rms.split('Mean:  ')
    if (mean_rms.find('Error')>-1):
        m=h[1].split('MeanError: ')
        if (mean_rms.find('NBins')>-1):
            n=m[1].split('NBins: ')
        elif (mean_rms.find('NBins')<0):    
            n=m[1].split('RMS: ')
        EMean=n[0]
    return EMean
                                                
def ExtractRMS( mean_rms ):
    global RMS
    RMS=""
    h=mean_rms.split('Mean:  ')
    if (mean_rms.find('Error')>-1):
        m=h[1].split('MeanError: ')
        if (mean_rms.find('NBins')>-1):
            n=m[1].split('NBins: ')
            j = n[1].split('RMS: ')
            jj = j[1].split('RMSError: ')
        elif (mean_rms.find('NBins')<0):
            n=m[1].split('RMS: ')
            jj=n[1].split('RMSError: ')
        RMS=jj[0]
    elif (mean_rms.find('Error')<0):
        m=h[1].split('RMS: ')
        RMS=m[1]
    return RMS

def ExtractERMS( mean_rms ):
    global ERMS
    ERMS=""
    h=mean_rms.split('Mean:  ')
    if (mean_rms.find('Error')>-1):
        m=h[1].split('MeanError: ')
        if (mean_rms.find('NBins')>-1):
            n=m[1].split('NBins: ')
            j = n[1].split('RMS: ')
            jj = j[1].split('RMSError: ')
        elif (mean_rms.find('NBins')<0):
            n=m[1].split('RMS: ')
            jj=n[1].split('RMSError: ')
        ERMS=jj[1]
    return ERMS
                                                                    
def ExtractAverage( mean_rms ):
    global Average
    h=mean_rms.split('Average:  ')
    m=h[1].split('NBins: ')
    Average=m[0]
    return Average

def ExtractNBins( mean_rms ):
    global NBins
    h=mean_rms.split('Average:  ')
    m=h[1].split('NBins: ')
    NBins=m[1]
    return NBins

# CheckHisto_RMS 
def ExtractXRMS( mean_rms ):
    global xrms
    h=mean_rms.split('XRMS:  ')
    m=h[1].split('YRMS: ')
    xrms=m[0]
    return xrms

def ExtractYRMS( mean_rms ):
    global yrms
    h=mean_rms.split('XRMS:  ')
    m=h[1].split('YRMS: ')
    yrms=m[1]
    return yrms

# CheckHisto_Mean 
def ExtractXMean( mean_rms ):
    global xmean
    if (mean_rms.find('XMean: ')>-1):
        h=mean_rms.split('XMean:  ')
        m=h[1].split('YMean: ')
    else:
        h=mean_rms 
        m=h.split('YMean: ')
    xmean=m[0]
    return xmean

def ExtractYMean( mean_rms ):
    global ymean
    if (mean_rms.find('XMean:  ')>-1):
        h=mean_rms.split('XMean:  ')
        m=h[1].split('YMean: ')
    else:
        h=mean_rms
        m=h.split('YMean: ')
    ymean=m[1]        
    return ymean

# Sidebands_Absolute, Sidebands_Relative 
def ExtractSideBands( mean_rms ):
    global sideb
    h=mean_rms.split('SideBands:  ')
    m=h[1].split('TotalIntegral: ')
    sideb=m[0]
    return sideb

def ExtractTotInt( mean_rms ):
    global totint
    h=mean_rms.split('SideBands:  ')
    m=h[1].split('TotalIntegral: ')
    totint=m[1]
    return totint

# simple_pol1_fit 
def ExtractPol0( mean_rms ):
    global pol0
    h=mean_rms.split('pol1[0]:  ')
    m=h[1].split('pol1[1]: ')
    pol0=m[0]
    return pol0

def ExtractPol1( mean_rms ):
    global pol1
    h=mean_rms.split('pol1[0]:  ')
    m=h[1].split('pol1[1]: ')
    pol1=m[1]
    return pol1

# simple_gaus_fit 
def ExtractConstG( mean_rms ):
    global constant
    h=mean_rms.split('Constant:  ')
    m=h[1].split('Mean: ')
    constant=m[0]
    return constant

def ExtractMeanG( mean_rms ):
    global meang
    h=mean_rms.split('Constant:  ')
    m=h[1].split('Mean: ')
    n = m[1].split('Sigma: ')
    meang=n[0]
    return meang

def ExtractRMSG( mean_rms ):
    global rmsg
    h=mean_rms.split('Constant:  ')
    m=h[1].split('Mean: ')
    n = m[1].split('Sigma: ')
    rmsg=n[1]
    return rmsg

# binContentComp, Bins_LessThan_Threshold, Bins_LessThanEqual_Threshold, Bins_GreaterThan_Threshold, Bins_GreaterThanEqual_Threshold
def ExtractOnlyNBins( histo_nameonly, mean_rms ):
    global contbins
    global bin_inf1
    global bin_inf2
    global bin_inf3
    global bin_inf4
    h=mean_rms.split('NBins: ')
    if (len(h)>1): 
        if (h[1].find('RMS')>-1):
            n=h[1].split('RMS: ')
            contbins=n[0]            
        elif (h[1].find(histo_nameonly.strip())>-1):
            reading = h[1].split(histo_nameonly.strip())
            contbins = reading[0]
            for i in range(1,len(reading)):
                if (reading[i].find('(0.0000')>-1):
                    bin_inf1 = reading[i].split(':')[1]
                    break
                else:
                    bin_inf1 = '-0.0001'
            for i in range(1,len(reading)):
                if (reading[i].find('(1.0000')>-1):
                    bin_inf2 = reading[i].split(':')[1]
                    break
                else:
                    bin_inf2 = '-0.0001'
            for i in range(1,len(reading)):
                if (reading[i].find('(2.0000')>-1):
                    bin_inf3 = reading[i].split(':')[1]
                    break
                else:
                    bin_inf3 = '-0.0001'
            for i in range(1,len(reading)):
                if (reading[i].find('(3.0000')>-1):
                    bin_inf4 = reading[i].split(':')[1]
                    break
                else:
                    bin_inf4 = '-0.0001'
        else:    
            contbins=h[1]
            bin_inf1 = '0'
            bin_inf2 = '0' 
            bin_inf3 = '0'
            bin_inf4 = '0'
            
    if (len(h)==1): 
        if not h[0]:
            contbins=0
            bin_inf1 = '0'
            bin_inf2 = '0' 
            bin_inf3 = '0'
            bin_inf4 = '0'
        else:    
            contbins=h[0]
            bin_inf1 = '0'
            bin_inf2 = '0' 
            bin_inf3 = '0'
            bin_inf4 = '0'

    return (contbins, bin_inf1, bin_inf2, bin_inf3, bin_inf4) 

def ExtractAlgo( file, rootFolder ):
    rootFolder =file+":"+rootFolder
    of = dqutils.HanOutputFile( file )
    algo = of.getAlgorithm(rootFolder)
    return algo

def ExtractNEntries( file, rootFolder , histo_string):
    nentries='0'
    rootFolder =file+":"+rootFolder
    of = dqutils.HanOutputFile( file )
    nentries = of.getNEntries(rootFolder, histo_string)
    return nentries

