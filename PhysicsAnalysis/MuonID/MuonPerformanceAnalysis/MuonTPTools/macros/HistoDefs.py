# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from Defs import *
import sys
import math, ROOT, itertools, os.path
from array import array 

# TH2Poly for fine eta-phi binning
def TPFineEtaPhiHist(name,title):
    histo = ROOT.TH2Poly()
    histo.SetName(name)
    histo.SetTitle(title)
    
    barrelECTrans = 1.19
    
    # etabins used in 2012 analysis:
    #etabins = [-2.75,-2.18,-1.95,-1.74,-1.52,-1.37,-1.05,-0.84,-0.63,-0.42,-0.21,0.,0.21,0.42,0.63,0.84,1.05,1.37,1.52,1.74,1.95,2.18,2.75]
    etabins = [0.15,0.3,0.45,0.6,0.75,0.9,1.05,barrelECTrans,1.35,1.5,1.65,1.8,1.95,2.1,2.25,2.5]
    etabins = sorted([0.]+etabins+[-binX for binX in etabins])
    
    phiSectorBarrel = [-2.905,-2.59,-2.12,-1.805,-1.335,-1.02,-0.55,-0.235,0.235,0.55,1.02,1.335,1.805,2.12,2.59,2.905]
    phiSectorEC = [-3.011,-2.487,-2.225,-1.702,-1.440,-0.916,-0.655,-0.131,0.131,0.655,0.916,1.440,1.702,2.225,2.487,3.011]
    
    # we go like this when adding the bins:
    # 
    # ^  ^  ^
    # |  | |
    # x------->
    
    #correct behavior for the bin around pi: one common bin at +pi and -pi instead of 2!
    for etaCurrent, etaNext in zip (etabins, etabins [1:] ):
        for phiBarrelCurrent, phiBarrelNext, phiECCurrent, phiECNext in zip (phiSectorBarrel, phiSectorBarrel [1:], phiSectorEC, phiSectorEC [1:] ):
            
            # if we pass the threshold, different treatment
            if (etaCurrent < -barrelECTrans) and (etaNext > -barrelECTrans):#first threshold passage
                #
                #
                #      here, we do:
                #           |1.19
                #       1   2
                #           3    4
                #           6    5
                #      8   |7
                #
                x = array('d',[etaCurrent, -barrelECTrans, -barrelECTrans, etaNext, etaNext, -barrelECTrans, -barrelECTrans, etaCurrent])
                y = array('d',[phiECNext, phiECNext, phiBarrelNext, phiBarrelNext, phiBarrelCurrent, phiBarrelCurrent, phiECCurrent, phiECCurrent])
                binNumber = histo.AddBin(8,x,y)
                #print '(etaCurrent < -barrelECTrans) and (etaNext > -barrelECTrans), etaCurrent: %s, etaNext: %s. bin: %s'%(etaCurrent, etaNext,binNumber)
            elif (etaCurrent < barrelECTrans) and (etaNext > barrelECTrans):# second threshold passage
                #
                #
                #      here, we do:
                #           |1.19
                #           |3    4
                #       1   2
                #       8   7
                #          |6     5
                #
                x = array('d',[etaCurrent, barrelECTrans, barrelECTrans, etaNext, etaNext, barrelECTrans, barrelECTrans, etaCurrent])
                y = array('d',[phiBarrelNext, phiBarrelNext, phiECNext, phiECNext, phiECCurrent, phiECCurrent, phiBarrelCurrent, phiBarrelCurrent])
                binNumber = histo.AddBin(8,x,y)
                #print '(etaCurrent < barrelECTrans) and (etaNext > barrelECTrans), etaCurrent: %s, etaNext: %s. bin: %s'%(etaCurrent, etaNext,binNumber)
            else:#normal case
                #
                #
                #      here, we do:
                #
                #        1     2
                #
                #        4     3
                #
                if (etaNext <= -barrelECTrans) or (etaCurrent >= barrelECTrans):
                    phiCurrent = phiECCurrent
                    phiNext = phiECNext
                else:
                    phiCurrent = phiBarrelCurrent
                    phiNext = phiBarrelNext
                
                x = array('d',[etaCurrent, etaNext, etaNext, etaCurrent])
                y = array('d',[phiNext, phiNext, phiCurrent, phiCurrent])
                binNumber = histo.AddBin(4,x,y)
                #print 'else, etaCurrent: %s, etaNext: %s. bin: %s'%(etaCurrent, etaNext,binNumber)

        # finally, the -pi / pi bin!
        phiBarrelLast = phiSectorBarrel[-1]
        phiECLast = phiSectorEC[-1]
        
        #      here, we fill like this:
        #      and of course if we have no threshold
        #      we don't have 6,7,10,11
        #
        #        4.........................3     <- +pi
        #        '                         '
        #        '                         '
        #        5.......6                 '
        #                7................8'  8,9 are actually *on* the vertical double line of course!
        #                                 ''  and exactly along the line 2-3.
        #                                 ''
        #                                 ''
        #                                 ''  this line has 0 width and is aligned with the other bin
        #                                 ''  edges, so we don't see it on the histo!
        #                                 ''  drawn twice here just to clarify that we go along that
        #                                 ''  way twice
        #                10...............9'
        #        12......11                '
        #        '                       array  '
        #        '                         '
        #        1.........................2      <- -pi
        if math.fabs(etaCurrent) > barrelECTrans:
            phiedge = phiECLast
        else:
            phiedge = phiBarrelLast
        if math.fabs(etaNext) > barrelECTrans:
            phiedger = phiECLast
        else:
            phiedger = phiBarrelLast

        if (etaCurrent <= -barrelECTrans) and (etaNext >= -barrelECTrans):
            xe = array('d',[etaCurrent, etaNext, etaNext, etaCurrent,etaCurrent,-barrelECTrans,-barrelECTrans,etaNext,etaNext,-barrelECTrans,-barrelECTrans,etaCurrent])
            ye = array('d',[-math.pi,-math.pi,math.pi,math.pi,phiedge,phiedge,phiedger,phiedger,-phiedger,-phiedger,-phiedge,-phiedge])
            binNumber = histo.AddBin(12,xe,ye)
            #print '(etaCurrent <= -barrelECTrans) and (etaNext >= -barrelECTrans), etaCurrent: %s, etaNext: %s. bin: %s'%(etaCurrent, etaNext,binNumber)
        elif (etaCurrent <= barrelECTrans) and (etaNext >= barrelECTrans):
            xe = array('d',[etaCurrent, etaNext, etaNext, etaCurrent,etaCurrent,barrelECTrans,barrelECTrans,etaNext,etaNext,barrelECTrans,barrelECTrans,etaCurrent])
            ye = array('d',[-math.pi,-math.pi,math.pi,math.pi,phiedge,phiedge,phiedger,phiedger,-phiedger,-phiedger,-phiedge,-phiedge])
            binNumber = histo.AddBin(12,xe,ye)
            #print '(etaCurrent <= barrelECTrans) and (etaNext >= barrelECTrans), etaCurrent: %s, etaNext: %s. bin: %s'%(etaCurrent, etaNext,binNumber)
        else:
            xe5 = array('d',[etaCurrent,etaNext,etaNext,etaCurrent,etaCurrent,etaNext,etaNext,etaCurrent])
            ye5 = array('d',[-math.pi,-math.pi,math.pi,math.pi,phiedge,phiedger,-phiedger,-phiedge])
            binNumber = histo.AddBin(8,xe5,ye5)
            #print 'else2, etaCurrent: %s, etaNext: %s. bin: %s'%(etaCurrent, etaNext,binNumber)
    return histo



        
# list of already created histograms
HistoSetups = { "mll" : "mll" }

# return histogram definition in the following way:
# [ROOT.TH1F("pt_template","pt;p_{T} [GeV];Probes",len(PtBins)-1,array.array("f",PtBins)),"pt",""]
def initHisto(name):

    
    CaloTagEtaBins =  [-2.5,-2,-1.8,-1.6,-1.5,-1.1,-0.15,0.15,1.1,1.5,1.6,1.8,2,2.5]
    CaloTagPtBins =  [15,20,30,40,50,60,70,90,1e15]
    PtBins = [10,15,20,25,30,35,40,50,65,80,100]
    #etaBins = [-2.5, -1.1,-0.2,0.2,1.1,2.5] 
    etaBins = [-2.5, 2.5]
    
    # 1D
    if name == "DetRegions":
        histo = ROOT.TH1F("DetRegions_template","DetRegions;Detector Region;Probes",12,-0.5,11.5)
        histo.GetXaxis().SetBinLabel(1,"unknown")
        histo.GetXaxis().SetBinLabel(2,"Barrel large")
        histo.GetXaxis().SetBinLabel(3,"Barrel small")
        histo.GetXaxis().SetBinLabel(4,"Barrel overlap")
        histo.GetXaxis().SetBinLabel(5,"Feet")
        histo.GetXaxis().SetBinLabel(6,"Transition")
        histo.GetXaxis().SetBinLabel(7,"Endcap large")
        histo.GetXaxis().SetBinLabel(8,"Endcap small")
        histo.GetXaxis().SetBinLabel(9,"BEE")
        histo.GetXaxis().SetBinLabel(10,"Forward large")
        histo.GetXaxis().SetBinLabel(11,"Forward small")
        histo.GetXaxis().SetBinLabel(12,"Crack")
        histo.SetDirectory(0)
        HistoSetups[name] = name
        return [histo,"detRegion",""]
    elif name == "mll":
        HistoSetups[name] = name
        return [ROOT.TH1F("mll_template","mll;m_{ll} [GeV];TP Pairs",8,80000,100000),"mll",""]
    elif name == "pt":
        HistoSetups[name] = name
        return [ROOT.TH1F("pt_template","pt;p_{T} [GeV];Probes",len(PtBins)-1,array("f",PtBins)),"pt",""]
    elif name == "eta":
        HistoSetups[name] = name
        return [ROOT.TH1F("eta_template","eta;#eta;Probes",20,-2.5,2.5),"eta",""]
    elif name == "etaTemp":
        HistoSetups[name] = name
        return [ROOT.TH1F("etaTemp_template","eta;#eta;Probes",len(etaBins)-1,array("f",etaBins)),"eta",""]
    elif name == "phi":
        HistoSetups[name] = name
        return [ROOT.TH1F("phi_template","phi;#phi;Probes",20,-math.pi,math.pi),"phi",""]
    elif name == "d0":
        HistoSetups[name] = name
        return [ROOT.TH1F("d0_template","d0;d_{0} [mm];Probes",20,-0.2,0.2),"d0",""]
    elif name == "z0":
        HistoSetups[name] = name
        return [ROOT.TH1F("z0_template","z0;z_{0} [mm];Probes",20,-0.2,0.2),"z0",""]
    elif name == "z_pt":
        HistoSetups[name] = name
        return [ROOT.TH1F("z_pt_template","z_pt;p_{T}(Z) [GeV];TP Pairs",100,0,200),"1",""]
    elif name == "fineEtaPhi":
        HistoSetups[name] = name
        return [ROOT.TH1F("fineEtaPhi_template","fineEtaPhi;fine (#eta,#phi) bin;Probes",352,-175.5,176.5),"fineEtaPhi",""]
    elif name == "fineEtaPhi_negq":
        HistoSetups[name] = name
        return [ROOT.TH1F("fineEtaPhi_negq_template","fineEtaPhi_negq;fine (#eta,#phi) bin;Probes",352,-175.5,176.5),"fineEtaPhi_negq",""]
    elif name == "fineEtaPhi_posq":
        HistoSetups[name] = name
        return [ROOT.TH1F("fineEtaPhi_posq_template","fineEtaPhi_posq;fine (#eta,#phi) bin;Probes",352,-175.5,176.5),"fineEtaPhi_posq",""]
    elif name == "DetRegions_Aside":
        HistoSetups[name] = name
        return [ROOT.TH1F("DetRegions_Aside_template","DetRegions_Aside;Detector Region;Probes",14,-0.5,13.5),"detRegion","abs(eta) > 0"]
    elif name == "DetRegions_Cside":
        HistoSetups[name] = name
        return [ROOT.TH1F("DetRegions_Cside_template","DetRegions_Cside;Detector Region;Probes",14,-0.5,13.5),"detRegion","abs(eta) < 0"]

    # 2D
    elif name == "etaphi":
        HistoSetups[name] = name
        return [ROOT.TH2F("etaphi_template","etaphi;#eta;#phi",30,-2.5,2.5,32,-math.pi,math.pi),"phi : eta",""]
    elif name == "etapt":
        HistoSetups[name] = name
        return [ROOT.TH2F("etapt_template","etapt;#eta;p_{T} [GeV]",20,-2.5,2.5,10,10,120),"pt : eta",""]
    elif name == "CaloTag2D":
        HistoSetups[name] = name
        return [ROOT.TH2F("CaloTag2D_template","CaloTag2D;#eta;p_{T} [GeV]",len(CaloTagEtaBins)-1,array("f",CaloTagEtaBins), len(CaloTagPtBins)-1, array("f",CaloTagPtBins)),"pt : eta",""]
    elif name == "ptmll":
        HistoSetups[name] = name
        return [ROOT.TH2F("ptmll_template","ptmll;p_{T} [GeV];m_{ll} [GeV]",10,10,120,20,80000,100000),"mll : pt",""]
    elif name == "etaphiFine":
        HistoSetups[name] = name
        return [TPFineEtaPhiHist("etaphiFine_template","etaphiFine;#eta;#phi;Efficiency"),"phi : eta",""]
    
    else:
        print "Histogram not defined!"
        return None
