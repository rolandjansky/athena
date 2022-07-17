#                                                                
#Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# 

import MMRawDataMonitoring.MMRawMonLabels as labels
from ROOT import TF1, TMath

def getMMLabel(x,y):
        labelx = getattr(labels, x)
        labely = getattr(labels, y)
        return labelx,labely

def getMMLabelY(y):
        labely = getattr(labels, y)
        return labely

def getMMLabelX(x):
        labelx = getattr(labels, x)
        return labelx

def get_MPV_charge(histo):
        flandau = TF1("flandau","landau",0,1200)
        histo.Fit("flandau","QRN")
        mpv = flandau.GetParameter(1)
        return mpv

def get_mean_and_sigma(histo, minval, maxval):
        fgaus = TF1("fgaus","gaus",minval,maxval)
        histo.Fit("fgaus","QRN")
        mean = fgaus.GetParameter(1)
        sigma = fgaus.GetParameter(2)
        return mean,sigma

def getXYbins(nPCB, iphi, start0, histoname, histo_list):
        ybin = iphi
        ind = 0
        if start0 is True:
                ind = histo_list[iphi].index(histoname)
        else:
                ind = histo_list[iphi-1].index(histoname)
        xbin = (ind+1) + (nPCB-1)*ind
        return xbin,ybin

def fitFermiDirac(x, par):
        fermiDirac= par[0]/( 1 + TMath.Exp( (par[1]-x[0])/par[2] ) + TMath.Exp( (x[0]-par[3])/par[4] ))
        return fermiDirac

tf1_fd = TF1("fd", fitFermiDirac, 0., 700., 5)

def get_time(histo):
        fd = tf1_fd
        p0=histo.GetMaximum()/4.
        p1=histo.GetBinLowEdge(histo.FindFirstBinAbove())
        p2=0.5
        p3=histo.GetBinLowEdge(histo.FindLastBinAbove())
        p4=0.5

        
        fd.SetParameters(p0, p1, p2, p3, p4)
        fd.SetParLimits(0, 0, histo.GetMaximum())
        fd.SetParLimits(1, histo.GetBinLowEdge(histo.FindFirstBinAbove())-5, histo.GetBinLowEdge(histo.FindFirstBinAbove())+10)
        fd.SetParLimits(2, 0, 10)
        fd.SetParLimits(3, histo.GetBinLowEdge(histo.FindFirstBinAbove()), histo.GetBinLowEdge(histo.FindLastBinAbove())+5)
        fd.SetParLimits(4, 0, 10)
        

        histo.Fit("fd","QRN")                                                                                                                                                              
        t0 = fd.GetParameter(1)     
        tmax = fd.GetParameter(3)     

        return [t0, tmax]


def poi(histo_name, side, eta):
        poi = ''
        if histo_name=='Charge_vs_PCB':
                poi = f'{side}: MPV of Landau Fit to Cluster charge {eta}'
        elif histo_name=='Charge_vs_PCB_ontrack':
                poi = f'{side}: MPV of Landau Fit to Cluster charge {eta} ontrack'
        elif histo_name=='Charge_vs_PCB_onseg':
                poi = f'{side}: MPV of Landau Fit to Cluster charge {eta} onseg'
        elif histo_name=='Cluster_size_vs_PCB':
                poi = f'Cluster size {side} {eta} per PCB'
        elif histo_name=='Cluster_size_vs_PCB_ontrack':
                poi = f'Cluster size {side} {eta} ontrack per PCB'
        elif histo_name=='Cluster_size_vs_PCB_onseg':
                poi = f'Cluster size {side} {eta} onseg per PCB'
        elif histo_name=='Cluster_time_vs_PCB':
                poi = f'Cluster time {side} {eta} per PCB'
        elif histo_name=='Cluster_time_ontrack_vs_PCB':
                poi = f'Cluster time {side} {eta} ontrack per PCB'
        elif histo_name=='Cluster_time_onseg_vs_PCB':
                poi = f'Cluster time {side} {eta} on seg per PCB'
        elif histo_name=='Strip_time_vs_PCB':
                poi = f'Strip time {side} {eta} per PCB'
        elif histo_name=='Strip_time_vs_PCB':
                poi = f'Strip time {side} {eta} per PCB'
        elif histo_name=='Strip_time_onseg_vs_PCB':
                poi = f'Strip time on seg {side} {eta} per PCB'
        elif histo_name=='Strip_time_ontrack_vs_PCB':
                poi = f'Strip time on track {side} {eta} per PCB'
        return(poi)
