# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import math, ROOT
import inspect
from array import array 


## use PrintFrame() for debugging
def PrintFrame():   
    callerframerecord = inspect.stack()[1]      # 0 represents this line
                                                # 1 represents line at caller
    frame = callerframerecord[0]
    info = inspect.getframeinfo(frame)
    #print info.filename                       # __FILE__     -> Test.py
    #print info.function                       # __FUNCTION__ -> Main
    print info.lineno

## Some utility methods used for the Z TP
def CalcEff (probes,matches):
    t = probes[0]
    dt = probes[1]
    h = matches[0]
    dh = matches[1]
    #print "++++++++++++++++++ got %.4f +/- %.4f trials, %.5f / %.4f matches"%(t,dt,h,dh)
    if t == 0:
        return [1.0,1.0]
        #t = 1
    m = t - h
    #print "dt = %.4f, dh = %.4f"%(dt,dh)
    dm = math.sqrt(abs(dt*dt-dh*dh))
    #print "        ++++++++++++++++++ and got %.4f +/- %.4f misses"%(m,dm)
    eff = h/t
    deff = math.sqrt(abs(((h*dm)**2+(m*dh)**2)/(t**4)))
    #print "        ++++++++++++++++++ Eff is %.4f +/- %.4f"%(eff,deff)
    #print "%.0f / %.0f -> %.4f"%(h,t,eff)
    return [eff,deff]

def CalcEffAsym (probes,matches):
    t = probes[0]
    dt = probes[1]
    h = matches[0]
    dh = matches[1]
    #print "++++++++++++++++++ got %.4f +/- %.4f trials, %.5f / %.4f matches"%(t,dt,h,dh)
    if t == 0:
        return [1.0,0.0,1.0]
        #t = 1
    m = t - h
    #print "dt = %.4f, dh = %.4f"%(dt,dh)
    dm = math.sqrt(abs(dt*dt-dh*dh))
    #print "        ++++++++++++++++++ and got %.4f +/- %.4f misses"%(m,dm)
    eff = h/t
    deff = math.sqrt(abs(((h*dm)**2+(m*dh)**2)/(t**4)))
    
    # use equation (11) from page 49 of https://cds.cern.ch/record/1247341/files/ATL-COM-PHYS-2010-124.pdf
    N = t
    if N == 0.:
        N = 1.
    sigmaPos = (2.*eff+1./N+math.sqrt(4.*eff*(1.-eff)/N+1./N**2))/(2.+2./N)-eff
    sigmaNeg = (2.*eff+1./N-math.sqrt(4.*eff*(1.-eff)/N+1./N**2))/(2.+2./N)-eff

    #print "        ++++++++++++++++++ asym Eff is %.4f + %.4f %.4f"%(eff,sigmaPos,sigmaNeg)
    return [eff,sigmaPos,abs(sigmaNeg)]

def EffDivide (num,den,eff):
    nbinsNum = GetNbins(num)
    #print 'in EffDivide: numerator has %s bins'%nbinsNum
    #print num.GetBinContent(0)
    for i in range (0, nbinsNum):
        #print "bin %s: num: %s, den: %s"%(i, num.GetBinContent(i), den.GetBinContent(i))
        if (num.GetBinContent(i) > den.GetBinContent(i)):
            num.SetBinContent(i,den.GetBinContent(i))
            
    if eff.InheritsFrom("TGraphAsymmErrors"):
        for i in range (0, nbinsNum):
            effiAsym = CalcEffAsym([den.GetBinContent(i),den.GetBinError(i)], [num.GetBinContent(i),num.GetBinError(i)])
            eff.SetPoint(eff.GetN(),num.GetBinCenter(i),effiAsym[0])
            #print "bin %s: up: %s, down: %s"%(i, effiAsym[1], effiAsym[2])
            eff.SetPointEYhigh(eff.GetN()-1, effiAsym[1])
            eff.SetPointEYlow(eff.GetN()-1, effiAsym[2])
            eff.SetPointEXhigh(eff.GetN()-1, num.GetBinWidth(i)/2.)
            eff.SetPointEXlow(eff.GetN()-1, num.GetBinWidth(i)/2.)
    else:
        for i in range (0, nbinsNum):
            effi = CalcEff([den.GetBinContent(i),den.GetBinError(i)], [num.GetBinContent(i),num.GetBinError(i)])
            eff.SetBinContent(i,effi[0])
            eff.SetBinError(i,effi[1])

## Some utility methods for algebra in root
def PolyDivide (num,den,ratio):
    nbins = GetNbins(num)
    for i in range (1, nbins):
        if 0.!=den.GetBinContent(i):
            ratio.SetBinContent(i,num.GetBinContent(i)/den.GetBinContent(i))
            ratio.SetBinError(i,math.sqrt((num.GetBinError(i)/den.GetBinContent(i))**2.+(den.GetBinError(i)*num.GetBinContent(i)/den.GetBinContent(i)**2.)**2.))
        else:
            ratio.SetBinContent(i,0.)
            ratio.SetBinError(i,0.)

def PolyMultiply (fac1,fac2,res):
    nbins = GetNbins(fac1)
    for i in range (1, nbins):
        res.SetBinContent(i,fac1.GetBinContent(i)*fac2.GetBinContent(i))
        res.SetBinError(i,math.sqrt((fac2.GetBinError(i)*fac1.GetBinContent(i))**2.+(fac1.GetBinError(i)*fac2.GetBinContent(i))**2.))
        
def GraphDivide (num,den,ratio):
    nPoints = GetNbins(num)
    for i in range (0, nPoints):
        if 0.!=den.GetY()[i]:
            ratio.SetPoint(i,num.GetX()[i],num.GetY()[i]/den.GetY()[i])
            ratio.SetPointEYhigh(i,math.sqrt((num.GetErrorYhigh(i)/den.GetY()[i])**2.+(den.GetErrorYhigh(i)*num.GetY()[i]/den.GetY()[i]**2.)**2.))
            ratio.SetPointEYlow(i,math.sqrt((num.GetErrorYlow(i)/den.GetY()[i])**2.+(den.GetErrorYlow(i)*num.GetY()[i]/den.GetY()[i]**2.)**2.))
        else:
            ratio.SetPoint(i,num.GetX()[i],0.)
            ratio.SetPointEYhigh(i,0.)
            ratio.SetPointEYlow(i,0.)
    
def GraphMultiply (fac1,fac2,res):
    nPoints = GetNbins(fac1)
    for i in range (0, nPoints):
        res.SetPoint(i,fac1.GetX()[i],fac1.GetY()[i]*fac2.GetY()[i])
        res.SetPointEYhigh(i,math.sqrt((fac2.GetErrorYhigh(i)*fac1.GetY()[i])**2.+(fac1.GetErrorYhigh(i)*fac2.GetY()[i])**2.))
        res.SetPointEYlow(i,math.sqrt((fac2.GetErrorYlow(i)*fac1.GetY()[i])**2.+(fac1.GetErrorYlow(i)*fac2.GetY()[i])**2.))

def GetNbins(histo):
    nbins = 0
    if histo.InheritsFrom("TH2Poly"):
        nbins = histo.GetNumberOfBins()+1
    elif histo.InheritsFrom("TH2"):
        nbins = (histo.GetNbinsX()+2)*(histo.GetNbinsY()+2)
    elif histo.InheritsFrom("TH1"):
        nbins = histo.GetNbinsX()+2
    elif histo.InheritsFrom("TGraphAsymmErrors"):
        nbins = histo.GetN()
    else:
        print "GetNbins(): no supported histogram type!"
        return -1

    return nbins

def ConvertTGraphAsymmErrors(graph):
    nPoints = GetNbins(graph)
    binCenters = []
    for i in range (0, nPoints):
        binCenters.append(graph.GetX()[i])
        
    histo = ROOT.TH1F(graph.GetName(),graph.GetName(),len(binCenters)-1,array("f",binCenters))
    histo.SetDirectory(0)
    
    for i in range (0, nPoints):
        histo.SetBinContent(i,graph.GetY()[i])
        maxError = max(graph.GetErrorYhigh(i),graph.GetErrorYlow(i))
        histo.SetBinError(i,maxError)
    return histo
    

## Some utility methods for systematics evaluation
def fill_sys_bin (binNumber, h_nom, h_up, h_down, h_sys, weight):
    if h_nom.InheritsFrom("TGraphAsymmErrors"):
        nom_val = h_nom.GetY()[binNumber]
        up_val = h_up.GetY()[binNumber]
        down_val = h_down.GetY()[binNumber]
    else:
        nom_val = h_nom.GetBinContent(binNumber)
        up_val = h_up.GetBinContent(binNumber)
        down_val = h_down.GetBinContent(binNumber)

    sup = math.fabs(up_val - nom_val)
    sdown = math.fabs(down_val - nom_val)
#     print 'bin: %s nom_val: %s sup: %s, sdown: %s'%(binNumber,nom_val,sup,sdown)

    # for now, we symmetrize using the larger of the deviations
    if (nom_val == 0):
        nom_val = 0.001
    smax = ROOT.TMath.Max(sup,sdown)/nom_val * weight
    
    if h_nom.InheritsFrom("TGraphAsymmErrors"):
        statYhigh = h_nom.GetErrorYhigh(binNumber)/nom_val * weight
        statYlow = h_nom.GetErrorYlow(binNumber)/nom_val * weight
        
        h_sys.SetPoint(binNumber,h_sys.GetX()[binNumber],smax)
        h_sys.SetPointEYhigh(binNumber,statYhigh)
        h_sys.SetPointEYlow(binNumber,statYlow)
    else:
        stat = h_nom.GetBinError(binNumber)/nom_val * weight
        
        h_sys.SetBinContent(binNumber,smax)
        h_sys.SetBinError(binNumber,stat)

def sysCompare(h_nom, h_up, h_down, sysType, weight):
    sys = h_nom.Clone('%s_sys_%s'%(h_nom.GetName(),sysType))
    if not sys.InheritsFrom("TGraphAsymmErrors"):
        sys.SetDirectory(0)

    nbins = GetNbins(sys)

    for ibin in range(0,nbins):
        fill_sys_bin(ibin,h_nom,h_up,h_down,sys,weight)

    return sys

def add_in_quadrature(hist_add_to, hist_add_this):
    # loop over the bins - here we need the actual types
    nbins = GetNbins(hist_add_to)

    for k in range(0,nbins):
        if hist_add_to.InheritsFrom("TGraphAsymmErrors"):
            sys = hist_add_to.GetY()[k]
            add_sys = hist_add_this.GetY()[k]
            sys = ROOT.sqrt(sys*sys+add_sys*add_sys)
            hist_add_to.SetPoint(k,hist_add_to.GetX()[k],sys)
        else:
            sys = hist_add_to.GetBinContent(k)
            add_sys = hist_add_this.GetBinContent(k)
            sys = ROOT.sqrt(sys*sys+add_sys*add_sys)
            hist_add_to.SetBinContent(k,sys)
        
def CopyEtaBins(hist):
    nbins = GetNbins(hist)
    #print '\n\n\n\n\n\n\nin CopyEtaBins\n\n\n\n\n\n\n' 
    #print nbins
    
    if hist.InheritsFrom("TGraphAsymmErrors"):
        print 'TODO: CopyEtaBins for graphs'
    else:
        # fineEtaPhiBinning has 352 bins (22 eta bins, 16 phi bins each)
        # abseta >= 1.05 and abseta < 1.37 -> etabin = -6*16 (negative eta), etabin = 5*16 (positive eta)
        if nbins == 354:
            for k in range(0,nbins):
                # k=1 equals bin -11*16+1 = -175
                #print k-176
                # copy bins (from -95 to -80) to (81 to 96), this means 81-96 to 257-272
                if k in [257,258,259,260,261,262,263,264,265,266,267,268,269,270,271,272]:
                    hist.SetBinContent(k,hist.GetBinContent(k-176))
                    hist.SetBinError(k,hist.GetBinError(k-176))
                    #print k
       
        # TPFineEtaPhiHist binning
        if nbins == 513:
            for k in range(0,nbins):
                #print k
                ## copy bins (for 1.35<abs(eta)<1.5), this means 97-112 to 401-416
                # copy bins (for 1.19<abs(eta)<1.35), this means 113-128 to 385-400
                # copy bins (for 1.05<abs(eta)<1.19), this means 129-144 to 369-384
                #if (k > 400 and k < 417):
                    #hist.SetBinContent(k,hist.GetBinContent(k-304))
                    #hist.SetBinError(k,hist.GetBinError(k-304))
                if (k > 384 and k < 401):
                    hist.SetBinContent(k,hist.GetBinContent(k-272))
                    hist.SetBinError(k,hist.GetBinError(k-272))
                if (k > 368 and k < 385):
                    hist.SetBinContent(k,hist.GetBinContent(k-240))
                    hist.SetBinError(k,hist.GetBinError(k-240))
                    