#! /usr/bin/python 

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# standard libraries
import sys, os, os.path, glob
from math import *
from optparse import OptionParser

# load ROOT libraries
print "loading ROOT library",
import ROOT
# load FTK ROOT extensions
print "loading FTK library",
ROOT.gSystem.Load("libftk_classes.so")
# load truth wrapper
import ftkutils
print "environment ready"


class HistoGroupNRoads :
    """This class stores in the output file the histograms needed to perform timing studies on the AM chip"""
    def __init__(self,suffix) :
        """initialize a group of histograms related to the number of roads"""
        print "Build Load histograms' group:", suffix,
        self.dname = "nroads_"+suffix
        self.directory = ROOT.gDirectory.mkdir(self.dname,"Histogram group: "+suffix);
        ROOT.gDirectory.cd(self.dname)
        self.histo_nroadsAM = ROOT.TH1F("histo_nroadsAM","# roads AM",100,0,0)
        self.histo_nroadsRW = ROOT.TH1F("histo_nroadsRW","# roads RW",100,0,0)

        # downstairs
        ROOT.gDirectory.cd("..")
        return None

    def Fill(self,nroadsAM,nroadsRW) :
        self.histo_nroadsAM.Fill(nroadsAM)
        self.histo_nroadsRW.Fill(nroadsRW)
        return None


class HistoGroupNFits :
    """This class stores in the output file histograms needed to do studies on the TF load"""
    def __init__(self,suffix) :
        """initialize a group of histograms related to the number of roads"""
        print "Build Load histograms' group:", suffix,
        self.dname = "nfits_"+suffix
        self.directory = ROOT.gDirectory.mkdir(self.dname,"Histogram group: "+suffix);
        ROOT.gDirectory.cd(self.dname)
        # number of combination, if too high for an event the number il limited
        self.histo_ncombs = ROOT.TH1F("histo_ncombs","# combs [done]",100,0,0)
        # number of attempted fit
        self.histo_nfitstot = ROOT.TH1F("histo_nfitstot","# fits [tot]",100,0,0)
        # number of majority fits
        self.histo_nfitsmaj = ROOT.TH1F("histo_nfitsmaj","# fits [maj]",100,0,0)
        # number of full fits recovered 
        self.histo_nfitsrecA = ROOT.TH1F("histo_nfitsrecA","# fits [full recovered]",100,0,0)
        # numbero of recovery fits done, they cost as majority fits
        self.histo_nfitsrecB = ROOT.TH1F("histo_nfitsrecB","# fits [rec]",100,0,0)
        self.histo_nfitsbadall = ROOT.TH1F("histo_nfitsbadall","# fits [bad]",100,0,0)
        self.histo_nfitsbadmaj = ROOT.TH1F("histo_nfitsbadmaj","# fits [bad, maj]",100,0,0)
        self.histo_nfitshwrejall = ROOT.TH1F("histo_nfitshwrejall","# fits [rej]",100,0,0)
        self.histo_nfitshwrejmaj = ROOT.TH1F("histo_nfitshwrejmaj","# fits [rej, maj]",100,0,0)

        # number of combination, if too high for an event the number il limited
        self.histo_ncombsI = ROOT.TH1F("histo_ncombsI","# combs [done]",100,0,0)
        # number of attempted fit
        self.histo_nfitstotI = ROOT.TH1F("histo_nfitstotI","# fits [tot]",100,0,0)
        # number of majority fits
        self.histo_nfitsmajI = ROOT.TH1F("histo_nfitsmajI","# fits [maj]",100,0,0)
        # number of full fits recovered 
        self.histo_nfitsrecAI = ROOT.TH1F("histo_nfitsrecAI","# fits [full recovered]",100,0,0)
        # numbero of recovery fits done, they cost as majority fits
        self.histo_nfitsrecBI = ROOT.TH1F("histo_nfitsrecBI","# fits [rec]",100,0,0)
        self.histo_nfitsbadallI = ROOT.TH1F("histo_nfitsbadallI","# fits [bad]",100,0,0)
        self.histo_nfitsbadmajI = ROOT.TH1F("histo_nfitsbadmajI","# fits [bad, maj]",100,0,0)
        self.histo_nfitshwrejallI = ROOT.TH1F("histo_nfitshwrejallI","# fits [rej]",100,0,0)
        self.histo_nfitshwrejmajI = ROOT.TH1F("histo_nfitshwrejmajI","# fits [rej, maj]",100,0,0)

        # downstairs
        ROOT.gDirectory.cd("..")
        return None

    def Fill(self,ncombs,nfits,nfitsmaj,nfullrec,nrec,
             nbad,nbadmaj,nrej,nrejmaj) :
        self.histo_ncombs.Fill(ncombs)
        self.histo_nfitstot.Fill(nfits)
        self.histo_nfitsmaj.Fill(nfitsmaj)
        self.histo_nfitsrecA.Fill(nfullrec)
        self.histo_nfitsrecB.Fill(nrec)
        self.histo_nfitsbadall.Fill(nbad)
        self.histo_nfitsbadmaj.Fill(nbadmaj)
        self.histo_nfitshwrejall.Fill(nrej)
        self.histo_nfitshwrejmaj.Fill(nrejmaj)
        return None

    def FillIncomplete(self,ncombs,nfits,nfitsmaj,nfullrec,nrec,
             nbad,nbadmaj,nrej,nrejmaj) :
        self.histo_ncombsI.Fill(ncombs)
        self.histo_nfitstotI.Fill(nfits)
        self.histo_nfitsmajI.Fill(nfitsmaj)
        self.histo_nfitsrecAI.Fill(nfullrec)
        self.histo_nfitsrecBI.Fill(nrec)
        self.histo_nfitsbadallI.Fill(nbad)
        self.histo_nfitsbadmajI.Fill(nbadmaj)
        self.histo_nfitshwrejallI.Fill(nrej)
        self.histo_nfitshwrejmajI.Fill(nrejmaj)
        return None

class HistoGroupFTK :
    def __init__(self,suffix) :
        """initialize a group of histograms related to the FTK tracks"""
        print "Build FTK histograms' group:", suffix,
        self.dname = "group_"+suffix
        self.directory = ROOT.gDirectory.mkdir(self.dname,"Histogram group: "+suffix);
        ROOT.gDirectory.cd(self.dname)
        self.histo_phi = ROOT.TH1F("histo_phi","#phi",
                                   phi_bins[0],phi_bins[1],phi_bins[2])
        self.histo_d0 = ROOT.TH1F("histo_d0","d_{0}",
                                  d0_bins[0],d0_bins[1],d0_bins[2])
        self.histo_curv = ROOT.TH1F("histo_curv","Curvature",
                                    curv_bins[0],curv_bins[1],curv_bins[2])
        self.histo_pt = ROOT.TH1F("histo_pt","p_{T}",
                                  pt_bins[0],pt_bins[1],pt_bins[2])
        self.histo_eta = ROOT.TH1F("histo_eta","#eta",
                                   eta_bins[0],eta_bins[1],eta_bins[2])
        self.histo_z0 = ROOT.TH1F("histo_z0","z_{0}",
                                  z0_bins[0],z0_bins[1],z0_bins[2])
        self.histo_chi2 = ROOT.TH1F("histo_chi2","#chi^{2}",40,0,40)
        self.histo_ncoords = ROOT.TH1F("histo_ncoords","# coords",
                                       14,.5,14.5)
        self.histo_chi2N = ROOT.TH1F("histo_chi2N","#chi^{2}/ndof",
                                     50,0,10)
        self.histo_pchi2 = ROOT.TH1F("histo_pchi2","prob",
                                     50,0,1)
        self.histo_typemask = ROOT.TH1F("histo_typemask","Type mask",
                                        4,0,4)
        self.histo_bin = ROOT.TH1F("histo_bin","Bin frequency",14,0,14)
        self.histo_abin = ROOT.TH1F("histo_abin","Missing bin frequency",
                                   14,0,14)

        self.histo_bincorr = ROOT.TH2F("histo_bincorr",
                                       "Bin correlation frequency",
                                       15,-1,14,
                                       15,-1,14)

        # configure some value to print information at the end
        self.nendcap_binlim = [self.histo_eta.FindBin(x) for x in nendcap_lim]
        self.barrel_binlim = [self.histo_eta.FindBin(x) for x in barrel_lim]
        self.pendcap_binlim = [self.histo_eta.FindBin(x) for x in pendcap_lim]

        # downstairs
        ROOT.gDirectory.cd("..")
        return None

    def Fill(self,track,num=1) :
        self.histo_phi.Fill(track.getPhi(),num)
        self.histo_d0.Fill(track.getIP(),num)
        self.histo_curv.Fill(track.getHalfInvPt(),num)
        self.histo_pt.Fill(track.getPt(),num)
        self.histo_eta.Fill(track.getEta(),num)
        self.histo_z0.Fill(track.getZ0(),num)
        self.histo_chi2.Fill(track.getChi2(),num)        
        ncoords = track.getNCoords()-track.getNMissing()
        self.histo_chi2N.Fill(track.getChi2()/ncoords,num)
        self.histo_pchi2.Fill(ROOT.TMath.Prob(track.getChi2(),ncoords-5),num)
        self.histo_typemask.Fill(track.getTypeMask())
        # mark the couple of missing tracks
        imiss = 0
        xmiss = [-1,-1]
        for ix in xrange(track.getNCoords()) :
            if track.getBitmask() & 1<<ix  :
                self.histo_bin.Fill(ix)
            else :
                self.histo_abin.Fill(ix)
                if imiss<2 :
                    # it is a pixel coordinate
                    if ix<6 :
                        if imiss==0 :
                            xmiss[imiss] = ix
                            imiss += 1
                        elif xmiss[imiss-1]+1<ix :
                            xmiss[imiss] = ix
                            imiss += 1
                    # it is an SCT coordinate
                    elif ix>=6 :
                        xmiss[imiss] = ix
                        imiss += 1
                elif VERBOSE>0 :
                    print "*** imiss =", imiss
                    imiss += 1
        self.histo_bincorr.Fill(xmiss[1],xmiss[0])
        return None

    def PrintEff(self,fid_histos,title) :
        print
        print title+" summary"
        try :
            print title+" eff (all)", self.histo_eta.Integral()/fid_histos.histo_eta.Integral()
        except :
            print "Global error"
            return None
        try :
            print title+" eff (neg EC)", self.histo_eta.Integral(self.nendcap_binlim[0],self.nendcap_binlim[1])/fid_histos.histo_eta.Integral(self.nendcap_binlim[0],self.nendcap_binlim[1])
        except :
            print "Impossible evalute neg EC efficiency"

        try :
            print title+" eff (Barrel)", self.histo_eta.Integral(self.barrel_binlim[0],self.barrel_binlim[1])/fid_histos.histo_eta.Integral(self.barrel_binlim[0],self.barrel_binlim[1])
        except :
            print "Impossible evalute BRL efficiency"

        try :
            print title+" eff (pos EC)", self.histo_eta.Integral(self.pendcap_binlim[0],self.pendcap_binlim[1])/fid_histos.histo_eta.Integral(self.pendcap_binlim[0],self.pendcap_binlim[1])
        except :
            print "Impossible evalute pos EC efficiency"
            


    
class HistoGroupTruth :
    pdgconv = {13:["#mu^{-}","#mu^{+}",1],
               11:["e^{-}","e^{+}",2],
               211:["#pi^{+}","#pi^{-}",3],
               321:["K^{+}","K^{-}",4],
               2212:["p","#bar{p}",5]}
    
    def __init__(self,suffix) :
        """Initialize a group of histogram. The histograms are created in a new directoy named as group_<suffix>. For group involving not truth tracks fake rates and resolution histogram are created. For any group 1-d or 2-d for many variables are created to be used in efficiency calculations"""

        print "Build truth histograms' group:", suffix,
        self.dname = "group_"+suffix
        self.directory = ROOT.gDirectory.mkdir(self.dname,"Histogram group: "+suffix);
        ROOT.gDirectory.cd(self.dname)
        self.histo_phi = ROOT.TH1F("histo_phi","#phi",
                                   phi_bins[0],phi_bins[1],phi_bins[2])
        self.histo_d0 = ROOT.TH1F("histo_d0","d_{0}",
                                  d0_bins[0],d0_bins[1],d0_bins[2])
        self.histo_curv = ROOT.TH1F("histo_curv","Curvature",
                                    curv_bins[0],curv_bins[1],curv_bins[2])
        self.histo_pt = ROOT.TH1F("histo_pt","p_{T}",
                                  pt_bins[0],pt_bins[1],pt_bins[2])
        self.histo_eta = ROOT.TH1F("histo_eta","#eta",
                                   eta_bins[0],eta_bins[1],eta_bins[2])
        self.histo_z0 = ROOT.TH1F("histo_z0","z_{0}",
                                  z0_bins[0],z0_bins[1],z0_bins[2])
        self.histo_barcode = ROOT.TH1F("histo_barcode","Barcode",
                                       1000,0,0); # auto-binning
        ntypes = len(self.pdgconv)
        self.histo_pdg = ROOT.TH1F("histo_pdg","PDG codes",
                                   2*ntypes+1,-ntypes,ntypes+1)
        self.histo_pdg.GetXaxis().SetBinLabel(ntypes+1,"other")
        for k, v in self.pdgconv.items() :
            nbin = self.histo_pdg.FindBin(v[2])
            self.histo_pdg.GetXaxis().SetBinLabel(nbin,v[0])
            nbinA = self.histo_pdg.FindBin(-v[2])
            self.histo_pdg.GetXaxis().SetBinLabel(nbinA,v[1])

        # z0 code as function of the other variables
        self.histo_etaz0 = ROOT.TH2F("histo_etaz0","z_{0} vs #eta",
                                     eta_bins[0]/2,eta_bins[1],eta_bins[2],
                                     z0_bins[0]/2,z0_bins[1],z0_bins[2])
        self.histo_etaphi0 = ROOT.TH2F("histo_etaphi0","#phi_{0} vs #eta",
                                       eta_bins[0]/2,eta_bins[1],eta_bins[2],
                                       phi_bins[0]/2,phi_bins[1],phi_bins[2])
        
        # phi as function of the other variables
        self.histo_phi0d0 = ROOT.TH2F("histo_phi0d0","#phi_{0} vs d_{0}",
                                      phi_bins[0]/2,phi_bins[1],phi_bins[2],
                                      d0_bins[0]/2,d0_bins[1],d0_bins[2])
        
        # pdg code as function of the other variables
        self.histo_pdgcurv = ROOT.TH2F("histo_pdgcurv","curv vs PDG",
                                       2*ntypes+1,-ntypes,ntypes+1,
                                       curv_bins[0]/2,curv_bins[1],curv_bins[2])
        self.histo_pdgpt = ROOT.TH2F("histo_pdgpt","p_{T} vs PDG",
                                       2*ntypes+1,-ntypes,ntypes+1,
                                       pt_bins[0]/2,pt_bins[1],pt_bins[2])
        self.histo_pdgphi0 = ROOT.TH2F("histo_pdgphi0","#phi vs PDG",
                                       2*ntypes+1,-ntypes,ntypes+1,
                                       phi_bins[0]/2,phi_bins[1],phi_bins[2])
        self.histo_pdgd0 = ROOT.TH2F("histo_pdgd0","d_{0} vs PDG",
                                     2*ntypes+1,-ntypes,ntypes+1,
                                     d0_bins[0]/2,d0_bins[1],d0_bins[2])
        self.histo_pdgeta = ROOT.TH2F("histo_pdgeta","#eta vs PDG",
                                      2*ntypes+1,-ntypes,ntypes+1,
                                      eta_bins[0]/2,eta_bins[1],eta_bins[2])
        self.histo_pdgz0 = ROOT.TH2F("histo_pdgz0","z_{0} vs PDG",
                                     2*ntypes+1,-ntypes,ntypes+1,
                                     z0_bins[0]/2,z0_bins[1],z0_bins[2])



        # configure some value to print information at the end
        self.nendcap_binlim = [self.histo_eta.FindBin(x) for x in nendcap_lim]
        self.barrel_binlim = [self.histo_eta.FindBin(x) for x in barrel_lim]
        self.pendcap_binlim = [self.histo_eta.FindBin(x) for x in pendcap_lim]

        # downstairs
        ROOT.gDirectory.cd("..")
        return None

    def Fill(self,track) :
        self.histo_phi.Fill(track.phi0)
        self.histo_d0.Fill(track.d0)
        self.histo_curv.Fill(track.curv)
        self.histo_pt.Fill(track.pt)
        self.histo_eta.Fill(track.eta)
        self.histo_z0.Fill(track.z0)

        self.histo_etaz0.Fill(track.eta,track.z0)

        self.histo_etaphi0.Fill(track.eta,track.phi0)

        self.histo_phi0d0.Fill(track.phi0,track.d0)
        
        self.histo_barcode.Fill(track.barcode)

        # pdg code block
        if abs(track.pdgcode) in self.pdgconv :
            sign = track.pdgcode/abs(track.pdgcode)
            pdg = sign*self.pdgconv[abs(track.pdgcode)][2]
        else :
            pdg = 0
        self.histo_pdg.Fill(pdg)
        self.histo_pdgcurv.Fill(pdg,track.curv)
        self.histo_pdgpt.Fill(pdg,track.pt)
        self.histo_pdgphi0.Fill(pdg,track.phi0)            
        self.histo_pdgd0.Fill(pdg,track.d0)
        self.histo_pdgeta.Fill(pdg,track.eta)            
        self.histo_pdgz0.Fill(pdg,track.z0)
        return None

    def PrintEff(self,fid_histos,title) :
        print
        print title+" summary"
        try :
            print title+" eff (all)", self.histo_eta.Integral()/fid_histos.histo_eta.Integral()
        except :
            print "Impossible to evaluate the efficiency"
        try :
            print title+" eff (neg EC)", self.histo_eta.Integral(self.nendcap_binlim[0],self.nendcap_binlim[1])/fid_histos.histo_eta.Integral(self.nendcap_binlim[0],self.nendcap_binlim[1])
        except :
            print "Impossible to evaluate the efficiency in negative EC"
        try :
            print title+" eff (Barrel)", self.histo_eta.Integral(self.barrel_binlim[0],self.barrel_binlim[1])/fid_histos.histo_eta.Integral(self.barrel_binlim[0],self.barrel_binlim[1])
        except :
            print "Impossible to evaluate the efficiency in the barrel"
        try :
            print title+" eff (pos EC)", self.histo_eta.Integral(self.pendcap_binlim[0],self.pendcap_binlim[1])/fid_histos.histo_eta.Integral(self.pendcap_binlim[0],self.pendcap_binlim[1])
        except :
            print "Impossible to evaluate the efficiency in positive EC"


class HistoGroupRes :
    """This class store the group of histograms used to evaluate the resolution of the tracks as function of the various parameters"""
    def __init__(self,suffix) :
        """initialize a group of histograms related to the FTK tracks"""
        print "Build FTK histograms' group:", suffix,
        self.dname = "resolution_"+suffix
        self.directory = ROOT.gDirectory.mkdir(self.dname,"Histogram group: "+suffix);
        ROOT.gDirectory.cd(self.dname)
        # simple resolution plots
        self.histo_phi = ROOT.TH1F("histo_phi","#Delta#phi",
                                   phires_bins[0],phires_bins[1],phires_bins[2])
        self.histo_curv = ROOT.TH1F("histo_curv","#Deltacurv",
                                    curvres_bins[0],curvres_bins[1],curvres_bins[2])
        self.histo_d0 = ROOT.TH1F("histo_d0","#Deltad_{0}",
                                  d0res_bins[0],d0res_bins[1],d0res_bins[2])
        self.histo_ctheta = ROOT.TH1F("histo_ctheta","#Deltacot(#theta)",
                                  cthetares_bins[0],cthetares_bins[1],cthetares_bins[2])
        self.histo_z0 = ROOT.TH1F("histo_z0","#Deltaz_{0}",
                                  z0res_bins[0],z0res_bins[1],z0res_bins[2])

        # linearity checks
        self.histo_philin = ROOT.TH2F("histo_philin","#Delta#phi vs #phi",
                                      50,0,0,
                                      phi_bins[0]/2,phi_bins[1],phi_bins[2])
        self.histo_curvlin = ROOT.TH2F("histo_curvlin","#Deltacurv vs curv",
                                       50,0,0,
                                       curv_bins[0]/2,curv_bins[1],curv_bins[2])
        self.histo_d0lin = ROOT.TH2F("histo_d0lin","#Deltad_{0} vs d_{0}",
                                     50,0,0,
                                     d0_bins[0]/2,d0_bins[1],d0_bins[2])
        self.histo_cthetalin = ROOT.TH2F("histo_cthetalin",
                                         "#Deltacot(#theta) vs cot(#theta)",
                                         50,0,0,
                                         ctheta_bins[0]/2,ctheta_bins[1],ctheta_bins[2])
        self.histo_z0lin = ROOT.TH2F("histo_z0lin","#Deltaz_{0} vs z_{0}",
                                     50,0,0,
                                     z0_bins[0]/2,z0_bins[1],z0_bins[2])

        # useful
        self.histo_d0pt = ROOT.TH2F("histo_d0pt","I.P. vs Pt",
                                    d0_bins[0]/2,d0_bins[1],d0_bins[2],
                                    25,0,50e3)

        # configure some value to print information at the end
        #self.nendcap_binlim = [self.histo_eta.FindBin(x) for x in nendcap_lim]
        #self.barrel_binlim = [self.histo_eta.FindBin(x) for x in barrel_lim]
        #self.pendcap_binlim = [self.histo_eta.FindBin(x) for x in pendcap_lim]

        # downstairs
        ROOT.gDirectory.cd("..")
        return  None

    def CompTruthFTK(self,ref_track,fit_track) :
        """Fill the histogram with the residual of the track parameters comparing a truth track with an FTK track"""
        dphi = fit_track.getPhi()-ref_track.phi0
        dcurv = fit_track.getHalfInvPt()-ref_track.curv
        dip = fit_track.getIP()-ref_track.d0
        dctheta = fit_track.getCotTheta()-ref_track.ctheta
        dz0 = fit_track.getZ0()-ref_track.z0

        # fill resolution histograms
        self.histo_phi.Fill(dphi)
        self.histo_curv.Fill(dcurv)
        self.histo_d0.Fill(dip)
        self.histo_ctheta.Fill(dctheta)
        self.histo_z0.Fill(dz0)

        # fill linearity check plots
        self.histo_philin.Fill(dphi,ref_track.phi0)
        self.histo_curvlin.Fill(dcurv,ref_track.curv)
        self.histo_d0lin.Fill(dip,ref_track.d0)
        self.histo_cthetalin.Fill(dctheta,ref_track.ctheta)
        self.histo_z0lin.Fill(dz0,ref_track.z0)
        
        # fill resolution plot vs useful variables
        self.histo_d0pt.Fill(dip,ref_track.pt)
        return None

def isGoodTruth(truth) :
    # fiducial region check
    if truth.barcode <= 0 or truth.barcode>=100000 :
        rejNumber[0] += 1
        if VERBOSE>2 :
            print "Invalid barcode"
        return False
    if abs(truth.curv)>CURVLIM  :
        rejNumber[1] += 1
        if VERBOSE>2 :
            print "Out curvature limits"
        return False
    if abs(truth.z0)>Z0LIM :
        rejNumber[2] += 1
        if VERBOSE>2 :
            print "Out z0 limits"
        return False
    if abs(truth.eta)>ETALIM :
        rejNumber[3] += 1
        if VERBOSE>2 :
            print "Out eta limits"
        return False
    if abs(truth.d0)>D0LIM :
        rejNumber[4] += 1
        if VERBOSE>2 :
            print "Out d0 limits"
        return False
    if truth.evtindex!=prog_options.pileupid :
        rejNumber[5] += 1
        if VERBOSE>2 :
            print "pileup tracks"
        return False
    if prog_options.primary and truth.parentID!=0 :
        rejNumber[6] += 1
        if VERBOSE>2 :
            print "not primary"
        return False

    # histos_check.Fill(truth)

    if truth.phi0 < PHI0MIN or truth.phi0 > PHI0MAX :
        rejNumber[7] += 1
        if VERBOSE>2 :
            print "not in phi boundaries"
        return False
    return True

def getMajorityType(ftk_track) :
    """Return a number that identifies the type of the majority algorithm used"""
    if ftk_track.getNMissing()>2 :
        return 3 # majority with 1 PXL + 1 SCT
    elif ftk_track.getNMissing()==2 and (ftk_track.getBitmask()|(~(~0<<6))) != ~(~0<<ftk_track.getNCoords()) :
        return 2 # majority with 2 missing SCT
    elif ftk_track.getNMissing()>=1 :
        return 1 # default majority with 1 PXL or 1 SCT missing
    else :
        return 0 # no majority

        
def isGoodFTK(ftk_track) :
    # fiducial region check
    if abs(ftk_track.getHalfInvPt())>CURVLIM  :
        rejNumberFTK[0] += 1
        if VERBOSE>2 :
            print "Out curvature limits"
        return False
    if abs(ftk_track.getZ0())>Z0LIM :
        rejNumberFTK[1] += 1
        if VERBOSE>2 :
            print "Out z0 limits"
        return False
    if abs(ftk_track.getEta())>ETALIM :
        rejNumberFTK[2] += 1
        if VERBOSE>2 :
            print "Out eta limits"
        return False
    if abs(ftk_track.getIP())>D0LIM :
        rejNumberFTK[3] += 1
        if VERBOSE>2 :
            print "Out d0 limits"
        return False
    if ftk_track.getPhi() < PHI0MIN or ftk_track.getPhi() > PHI0MAX :
        rejNumberFTK[4] += 1
        if VERBOSE>2 :
            print "not in phi boundaries"
        return False

    maj_type = getMajorityType(ftk_track)
    #print maj_type, prog_options.majoritycut, maj_type>prog_options.majoritycut
    if maj_type > prog_options.majoritycut :
        rejNumberFTK[5] += 1
        if VERBOSE>2 :
            print "double majority"
        return False # it is a dual majority track    
    return True



def MatchTruthFTK(truth_track,ftk_stream,match_list,useIncomplete=False) :
    """This function compares a truth track with all the tracks in an FTK track stream to find matches. The function returns the index of the track with the first match and the total number of matched tracks, it also changes the content of the 'match_list' list, in order to account for multiple matches"""
    # ID and G4 fraction of the best matching track
    IDmatch = -1
    IDfrac = 0
    # number of matchig tracks
    nmatch = 0
    # loop over the tracks
    if useIncomplete :
        ntracks = ftk_stream.getNTracksI()
    else :
        ntracks = ftk_stream.getNTracks()
    for iftk in xrange(ntracks) :
        if useIncomplete :
            ftk_track = ftk_stream.getTrackI(iftk)
        else :
            ftk_track = ftk_stream.getTrack(iftk)
            
        # check the first layer requirement
        if match_list[iftk]!=0:
            continue # skip matched
        if prog_options.requirefirst :
            if not ftk_track.getBitmask() & 1 :
                continue

        # check if the type of the track has a majority type
        maj_type = getMajorityType(ftk_track)
        # the check is skipped for incomplete tracks
        if maj_type > prog_options.majoritycut and not useIncomplete :
            continue 

        if VERBOSE>3 :
            ftk_track.Print()

        # verifies the match
        if ftk_track.getBarcode()==truth_track.barcode and ftk_track.getEventIndex()==truth_track.evtindex :
            if ftk_track.getBarcodeFrac()>=prog_options.matchfrac:
                # it is a match
                nmatch += 1
                if ftk_track.getBarcodeFrac()>IDfrac :
                    # if IDmatch>=0 there was a previous best track
                    if IDmatch>=0 :
                        # mark the previous best as duplicate
                        match_list[IDmatch] = 2
                    match_list[iftk] = 1
                    IDmatch = iftk
                else :
                    # this is a duplicated track
                    match_list[iftk] = 2
                if VERBOSE>3 :
                    print "Match"
            elif ftk_track.getBarcodeFrac()>IDfrac :
                # here the track doesn't match but it is close to the original track
                # if there isn't a match during the loop the function will return the
                # closest FTK track
                IDmatch = iftk
                IDfrac = ftk_track.getBarcodeFrac()
    # ended the track loop
    return (IDmatch,nmatch)


def MatchFTKTruth(ftk_track,truth_tracks,match_list) :
    """The function compare a FTK track with all the truth tracks, looking for match in both hard scattering and  pileup events. It returns the values: 0 if no match is found, 1 if the track matches and it is the first, 2 if it is a duplicate"""
    # to loop over all truth tracks to ensure there is a
    # track, in the fiducial region matching this track
    matchres = 0
    matchid = -1
    itruth = 0
    for truth in truth_tracks :
        if VERBOSE>3 :
            truth.Print()

        if ftk_track.getBarcode()==truth.barcode and ftk_track.getEventIndex()==truth.evtindex and ftk_track.getBarcodeFrac()>=prog_options.matchfrac :
            if not match_list[itruth] :
                # assumes just one truth track can match
                # if not already matched this track has a match
                matchres = 1
                matchid = itruth
                match_list[itruth] = True
                if VERBOSE>3 :
                    print "Match"
                break
            else :
                # if the matching truth track is already matched
                # this one is a duplicate
                if matchres==0 :
                    matchres = 2
                if VERBOSE>3 :
                    print "Duplicate"
                break
        itruth += 1
    #end truth loop, return the result
    return (matchres,matchid)


def PrintListTruth() :
    print ">>> List of valid truth tracks"
    itruth = 0
    for truth in truthwrapper.FTKEvent.truth_tracks :
        if lstGoodTruth[itruth] == 1 :
            print "%06d" % itruth,
            truth.Print()
        itruth += 1
    return None

def PrintListMatched() :
    print ">>> List of valid merged FTK tracks"
    for iftk in xrange(trackM_stream.getNTracks()) :
        if lstGoodM[iftk] == 1 :
            ftk_track = trackM_stream.getTrack(iftk)
            print "%06d" % iftk,
            ftk_track.Print()
            if hasMatchM[iftk] == 0 :
                print "Not Matched"
            elif hasMatchM[iftk] == 1 :
                print "Matched"
            elif hasMatchM[iftk] == 2 :
                print "Duplicated"
    return None


# Configurable options for the programs
options_parser = OptionParser(usage="%prog [options] input_list ftk_output_dir",version="%prog 0.3")
options_parser.add_option("-r","--num-region",dest="numregions",
                          help="Number of regions",default=1,
                          metavar="NUM", type="int")
options_parser.add_option("-s","--num-subregions",dest="numsubregions",
                          help="Number of subregions",default=1,
                          metavar="NUM",type="int")
options_parser.add_option("-n","--num-events",dest="numevents",
                          help="Maximum number of events to read",default=1000,
                          metavar="NUM",type="int")
options_parser.add_option("-m","--merge-scheme",dest="mergescheme",
                          help="Merge scheme: 0 or 1",default=0,
                          metavar="MODE",type="int")
options_parser.add_option("-o","--output",dest="output",
                          help="ROOT output file",default="ftk_plots.root",
                          metavar="FILE")
options_parser.add_option("-v","--verbose",dest="verbose",
                          help="Increment the verbosity level",default=0,
                          action="count")
options_parser.add_option("-I","--interactive",dest="interactive",
                          help="Stop the loop after each event and open a prompt to spy the content",
                          default=False,action="store_true")
options_parser.add_option("","--disable-unmerged-roads",dest="unmergedroads",
                          help="Disable unmerged roads scan",
                          action="store_false")
options_parser.add_option("","--disable-merged-roads",dest="mergedroads",
                          help="Disable merged roads scan",
                          action="store_false")
options_parser.add_option("","--disable-unmerged-tracks",dest="unmergedtracks",
                          help="Disable unmerged tracks scan",
                          action="store_false")
options_parser.add_option("","--disable-merged-tracks",dest="mergedtracks",
                          help="Disable merged trackss scan",
                          action="store_false")
options_parser.add_option("","--disable-merged-tracksI",dest="mergedtracksI",
                          help="Disable merged tracks scan, in the 1st step collection",action="store_false")
options_parser.add_option("","--disable-roads-link",dest="roadslink711",
                          help="Enable road 7L with 11L link scan",
                          action="store_false")
options_parser.add_option("","--enable-unmerged-roads",dest="unmergedroads",
                          help="Enable unmerged roads scan",action="store_true")
options_parser.add_option("","--enable-merged-roads",dest="mergedroads",
                          help="Enable merged roads scan",action="store_true")
options_parser.add_option("","--enable-unmerged-tracks",dest="unmergedtracks",
                          help="Enable unmerged tracks scan",action="store_true")
options_parser.add_option("","--enable-merged-tracks",dest="mergedtracks",
                          help="Enable merged tracks scan",action="store_true")
options_parser.add_option("","--enable-merged-tracksI",dest="mergedtracksI",
                          help="Enable merged tracks scan, in the 1st step collection",action="store_true")
options_parser.add_option("","--enable-roads-link",dest="roadslink711",
                          help="Enable road 7L with 11L link scan",
                          action="store_true")

options_parser.add_option("-f","--match-fraction",dest="matchfrac",
                          help="Minimum matching fraction to match",
                          type="float",default=.7)
options_parser.add_option("","--max-missing",dest="maxmissing",
                          help="Maximum number of missing layers",
                          default=1,type="int")
options_parser.add_option("","--curv-lim",dest="curvlim",
                          help="Maximum absolute value of curvature",
                          default=0.5e-3,type="float")
options_parser.add_option("","--d0-lim",dest="d0lim",
                          help="Maximum absolute value of d0",
                          default=2,type="float")
options_parser.add_option("","--eta-lim",dest="etalim",
                          help="Maximum absolute value of eta",
                          default=2.5,type="float")
options_parser.add_option("","--z0-lim",dest="z0lim",
                          help="Maximum absolute value of z0",
                          default=120.0,type="float")
options_parser.add_option("","--phi0-min",dest="phi0min",
                          help="Minum value of phi0",
                          default=-3.1,type="float")
options_parser.add_option("","--phi0-max",dest="phi0max",
                          help="Maximum value of phi0",
                          default=3.1,type="float")
options_parser.add_option("-S","--skip-region",dest="toskip",
                          help="Skip this region in the analysis, multiple regions can be skipped",
                          action="append")
options_parser.add_option("-O","--only-region",dest="oneregion",
                          help="Make analysis chain for a single region",
                          action="store", default=-1,type="int")
options_parser.add_option("","--require-first",dest="requirefirst",
                          help="Require first pixel layer",action="store_true",
                          default=False)
options_parser.add_option("","--primary",dest="primary",
                          help="Study only primary tracks",action="store_true",
                          default=False)
options_parser.add_option("","--pileup-id",dest="pileupid",
                          help="Select just a specific pileup ID",
                          default=0,action="store",type="int")
options_parser.add_option("","--evtlist-file",dest="eventlistfile",
                          help="Set a file where the list of matched tracks is saved",
                          type="int",action="store",default=None)
options_parser.add_option("","--majority-cut",dest="majoritycut",
                          help="The value defines the maximum accepted majority type: 0 no majority, 1 accept all majority, 2 skip double majority, 3 cut only when PXL+SCT layers are missing",
                          type="int",action="store",default=2)
options_parser.add_option("","--recovery-cut",dest="recoverycut",
                          help="The value of maximum accepted recovery: 0 no recovery, 1 recovery at 1st stage, 2 recovery at 2nd stage, 3 recovery in both stages ",
                          type="int",action="store",default=3)

options_parser.add_option("","--timing-study",dest="dotiming",
                          help="Do timing plots", action="store_true",
                          default=False)
options_parser.add_option("","--no-efficiency-study",dest="doefficiency",
                          help="Disable the loop over tracks/roads to do efficiency plots",
                          action="store_false",default=True)

# set some default values with possible collision
options_parser.set_defaults(unmergedroads=False,mergedroads=False,
                            unmergedtracks=False,mergedtracks=True,
                            mergedtracksI=False,roadslink711=False)
(prog_options, prog_args) = options_parser.parse_args()



if len(prog_args)!=2 :
    options_parser.print_help()
    sys.exit(-1)

if prog_options.mergedroads and prog_options.mergescheme!=0 :
    print "ERROR: merged roads analysis is possible only in merge-scheme 0"
    sys.exit(-1)
    

# set some general running variable
VERBOSE = prog_options.verbose
NREGIONS = prog_options.numregions
NSUBREGIONS = prog_options.numsubregions
NUMEVENTS = prog_options.numevents
MERGE_SCHEME = prog_options.mergescheme # 0: road-merge-tracks-merge, 1: road-tracks-merge

# actions to fill the related histograms
DO_UNMERGEDROADS = prog_options.unmergedroads
DO_MERGEDROADS = prog_options.mergedroads
DO_UNMERGEDTRACKS = prog_options.unmergedtracks
DO_MERGEDTRACKS = prog_options.mergedtracks
DO_MERGEDTRACKSI = prog_options.mergedtracksI
if DO_MERGEDTRACKSI and not DO_MERGEDTRACKS :
    print "!!! Incomplete merged tracks loop it is possible only if standard merged loop enabled"
    DO_MERGEDTRACKSI = False
DO_ROADSLINK711 = prog_options.roadslink711

# set the output filename
ROOTNAME = prog_options.output

# set the fiduacial volume of the truth tracks
CURVLIM = prog_options.curvlim
D0LIM = prog_options.d0lim
ETALIM = prog_options.etalim
Z0LIM = prog_options.z0lim
MAXMISSING = prog_options.maxmissing
if prog_options.oneregion < 0:
    PHI0MIN = prog_options.phi0min
    PHI0MAX = prog_options.phi0max
    try :
        TOSKIP = [int(el) for el in prog_options.toskip]
    except :
        TOSKIP = []
else :
    if NREGIONS == 8 :
        limits = [(-.6,-.2),(0.2,0.7),(1.0,1.4),(1.7,2.3),
                  (2.5,2.9),(-pi,pi),(-2.1,-1.7),(-1.4,-.95)]
    else :
        limits = [(-pi,pi) for ireg in xrange(NREGIONS)]
        
    TOSKIP = range(NREGIONS)
    del TOSKIP[prog_options.oneregion]
    PHI0MIN = limits[prog_options.oneregion][0]
    PHI0MAX = limits[prog_options.oneregion][1]

# the flag set if a list of matched tracks is dumped
PRINTMATCHES = False
if  prog_options.eventlistfile != None :
    try :
        matchedlist = open(prog_options.eventlistfile,"w")
        PRINTMATCHES = True
    except :
        print "*** it's not possible to open:", prog_options.eventlistfile
    
# set input files directory
input_dir = prog_args[0]
# set ftk data
ftk_dir = prog_args[1]

files_input = []
if os.path.isdir(input_dir) :
    print "Take input in directory:", input_dir
    # find input files, using shell expansion-like function
    files_input = glob.glob(input_dir+"/*.dat*")
    files_input.sort()
elif input_dir[-4:] == "list" :
    print "Take input from list file:", input_dir
    # read the file, each line is a path
    flist = open(input_dir,"r")
    for line in flist :
        fname = line.rstrip()
        if VERBOSE>3 :
            print "Adding:", fname,
        if os.path.isfile(fname) :
            files_input.append(fname)
            if VERBOSE>3 :
                print "Done"
        elif VERBOSE>3 :
            print "Not found"
elif os.path.isfile(input_dir) :
    print "Single input file:", input_dir
    files_input.append(input_dir)
else :
    print "*** Error: no valid input files"
    option_parser.print_help()
    sys.exit(-1)
    
# extract the file names, without directory name
files_basename = []
for f in files_input :
    newname = os.path.basename(f)
    # remove the extensions of the wrapper files
    newname = newname.replace(".dat","").replace(".bz2","").replace(".gz","")
    # append the base name in the list
    files_basename.append(newname)

# finds file in the ftk output structure
ninput_files = len(files_basename)
files_road = []
files_road_check = [[[True]*ninput_files]*NSUBREGIONS]*NREGIONS
files_roadM = []
files_track = []
if MERGE_SCHEME==0 :
    files_roadM_check = [[True]*ninput_files]*NREGIONS
    files_track_check = [[True]*ninput_files]*NREGIONS
elif MERGE_SCHEME==1 :
    files_track_check = [[[True]*ninput_files]*NSUBREGIONS]*NREGIONS
files_trackM = []
files_trackM_check = [True]*ninput_files


# after this point the code use the basename of the input file to
# prepare the the list of the output files at different stage.
# In parallel are set the right value for the the files, to remove files
# that don't have a correspondence between wrapped and ftk files

# create the list of the files in the directory of merged tracks
ifile = 0
for fname in files_basename :   
    track_file = ftk_dir+"/tracks_merge/%d/ftktracks_%s.root" % (NREGIONS,fname)
    if os.path.isfile(track_file) :
        files_trackM.append(track_file)
    else :
        files_trackM_check[ifile] = False
        if VERBOSE>2 :
            print "!!!", track_file, "doesn't exist"

    ifile += 1


# fill the list of flags to ensure the existing
for ireg in xrange(NREGIONS) :
    if ireg in TOSKIP :
        files_road.append(None)
        files_track.append(None)
        continue

    # a list of road for each region
    files_road.append([])
    if MERGE_SCHEME == 1:
        # prepare the list of tracks for each region
        files_track.append([])
    elif MERGE_SCHEME == 0 :
        files_roadM.append([])
        # in this scheme exists on road file and one track file for each region
        ifile = 0
        for fname in files_basename : 
            road_file = ftk_dir+"/tracks/%d/ftktracks_%s.root" % (ireg,fname)
            if not os.path.isfile(road_file) :
                files_roadM_check[ireg][ifile] = False
                if VERBOSE>2 :
                    print "!!!", road_file, "doesn't exist"

            ifile += 1
        ifile = 0
        for fname in files_basename : 
            track_file = ftk_dir+"/tracks/%d/ftktracks_%s.root" % (ireg,fname)
            if not os.path.isfile(track_file) :
                files_valid[ifile] = False
                if VERBOSE>2 :
                    print "!!!", track_file, "doesn't exist"

            ifile += 1

    for isub in xrange(NSUBREGIONS) :
        # each sub region has a list of roads
        files_road[ireg].append([])
        ifile = 0
        for fname in files_basename :            
            road_file = ftk_dir+"/roads/%d/%d/ftkroads_%s.root" % (ireg,isub,fname)
            if not os.path.isfile(road_file) :
                files_road_check[ireg][isub][ifile] = False
                if VERBOSE>2 :
                    print "!!!", road_file, "doesn't exist"

            ifile += 1

        if MERGE_SCHEME == 1 :
            files_track[ireg].append([])
            ifile = 0
            for fname in files_basename :
                track_file = ftk_dir+"/tracks/%d/%d/ftktracks_%s.root" % (ireg,isub,fname)
                if not os.path.isfile(track_file) :
                    files_track_check[ireg][isub][ifile] = False
                    if VERBOSE>2 :
                        print "!!!", track_file, "doesn't exist"

                ifile += 1

# double-check for files that are valid in all the schemes
files_input_clean = []
for ifile in xrange(ninput_files) :
    isValid = True
    if DO_MERGEDTRACKS and not files_trackM_check[ifile] :
        if VERBOSE>1 :
            print "!!! Missing merged tracks for file:", files_input[ifile]
        isValid = False

    for ireg in xrange(NREGIONS) :
        if ireg in TOSKIP :
            continue
        
        if MERGE_SCHEME==0 :
            if DO_UNMERGEDTRACKS and not files_track_check[ireg][ifile] :
                isValid = False
                break
            elif DO_MERGEDROADS and not files_roadM_check[ireg][ifile] :
                isValid = False
                break
            
        for isub in xrange(NSUBREGIONS) :
            if DO_UNMERGEDROADS and not files_road_check[ireg][isub][ifile] :
                if VERBOSE>1 :
                    print "!!! Missing unmerged roads for file:", files_input[ifile], ireg, isub, ifile

                isValid = False
                break
            elif MERGE_SCHEME==1 and DO_UNMERGEDTRACKS and not files_track_check[ireg][isub][ifile] :
                isValid = False
                break
        if not isValid :
            # if the check fails in a valid region is enough to skip
            break
        
    # final check to confirm the input file has ftk output
    # for each step in all the important regions
    if isValid :
        files_input_clean.append(files_input[ifile])
    elif VERBOSE>0 :
        print "Check failed for file:", files_input[ifile]
        
print "Number of checked files:", len(files_input_clean), "from:", len(files_input)
files_input = files_input_clean


# extract the file names, without directory name
files_basename = []
for f in files_input :
    newname = os.path.basename(f)
    # remove the extensions of the wrapper files
    newname = newname.replace(".dat","").replace(".bz2","").replace(".gz","")
    # append the base name in the list
    files_basename.append(newname)

# create the lists of the files for tasks that generate files in
# a different directory for each region and/or subregion
for ireg in xrange(NREGIONS) :
    if ireg in TOSKIP :
        continue
    
    # a list of road for each region
    files_road.append([])
    if MERGE_SCHEME == 1:
        # prepare the list of tracks for each region
        files_track.append([])
    elif MERGE_SCHEME == 0 :
        files_roadM.append([])
        # in this scheme exists on road file and one track file for each region
        ifile = 0
        for fname in files_basename : 
            road_file = ftk_dir+"/tracks/%d/ftktracks_%s.root" % (ireg,fname)
            if os.path.isfile(road_file) :
                files_roadM[ireg].append(road_file)
            ifile += 1
        ifile = 0
        for fname in files_basename : 
            track_file = ftk_dir+"/tracks/%d/ftktracks_%s.root" % (ireg,fname)
            if os.path.isfile(track_file) :
                files_track.append(track_file)
            ifile += 1

    for isub in xrange(NSUBREGIONS) :
        # each sub region has a list of roads
        files_road[ireg].append([])
        ifile = 0
        for fname in files_basename :
            road_file = ftk_dir+"/roads/%d/%d/ftkroads_%s.root" % (ireg,isub,fname)
            if os.path.isfile(road_file) :
                files_road[ireg][isub].append(road_file)
            ifile += 1

        if MERGE_SCHEME == 1 :
            files_track[ireg].append([])
            ifile = 0
            for fname in files_basename :
                track_file = ftk_dir+"/tracks/%d/%d/ftktracks_%s.root" % (ireg,isub,fname)
                if os.path.isfile(track_file) :
                    files_track[ireg][isub].append(track_file)
                ifile += 1


# print a summary to print the setup before to start
print "SETUP summary"
print "Num regions: %d, sub-region: %d" % (NREGIONS,NSUBREGIONS)
print "Skip:", TOSKIP
print "Merge scheme:", MERGE_SCHEME
print "Loop over", NUMEVENTS
print "Analysis on",
if prog_options.doefficiency :
    print "(efficiency)",
if prog_options.dotiming :
    print "(timing)",
print ":",
if DO_UNMERGEDROADS :
    print "[unmerged roads]",
    if DO_ROADSLINK711 :
        print "[road link]"
if DO_MERGEDROADS :
    print "[merged roads]",
if DO_UNMERGEDTRACKS :
    print "[unmerged tracks]",
if DO_MERGEDTRACKS :
    print "[merged tracks]",
if DO_MERGEDTRACKSI :
    print "[merged tracks (incomplete)]",
print
print "Fiducial region: |curv|<%4.2e, phi in [%4.2f,%4.2f], |d0|<%3.1f, |eta|<%3.1f, |z0|<%5.1f" %(CURVLIM,PHI0MIN,PHI0MAX,D0LIM,ETALIM,Z0LIM)
print "Input data:", input_dir, "Input files [%d]" % len(files_input)
if len(files_input)<5:
    print files_input
else :
    print files_input[0:4], "... and others"
    
print "FTK data:", ftk_dir
for ireg in xrange(NREGIONS) :
    if ireg in TOSKIP :
        continue
    
    print "Region", ireg
    if MERGE_SCHEME==0:
        print "Unmerged road files:",
        for isub in xrange(NSUBREGIONS) :
            print len(files_road[ireg][isub]),
        print
        print "Merged road files:", len(files_roadM[ireg])
        print "Unmerged track files:", len(files_track[ireg])
    elif MERGE_SCHEME==1 :
        print "Unmerged road files:",
        for isub in xrange(NSUBREGIONS) :
            print len(files_road[ireg][isub]),
        print
        print "Unmerged track files:",
        for isub in xrange(NSUBREGIONS) :
            print len(files_track[ireg][isub]),
        print
print "Merged track files:", len(files_trackM)

# use the truth wrapper to loop on the input ASCII files
truthwrapper = ftkutils.FTKFileChain(files_input)
truthwrapper.FTKEvent.ignoreHits = True
truthwrapper.FTKEvent.ignoreOffline = True
# TODO: count the events

# chain the FTK data
trackM_chain = ROOT.TChain("ftkdata","Merged tracks chain")
trackM_stream = ROOT.FTKTrackStream()
# set the FTK file chains in the standard merger scheme
if MERGE_SCHEME == 0 :
    track_chain = []
    track_stream = []
    road_chain = []
    road_stream = []
    roadM_chain = []
    roadM_stream = []
    for ireg in xrange(NREGIONS) :
        track_chain.append(ROOT.TChain("ftkdata","Tracks chain (R%d)"%(ireg)))
        track_stream.append(ROOT.FTKTrackStream())
        roadM_chain.append(ROOT.TChain("ftkdata","Merged roads chain (R%d)"%(ireg)))
        roadM_stream.append(ROOT.FTKRoadStream())
        road_chain.append([])
        road_strean.append([])
        for isub in xrange(NSUBREGIONS) :
            road_chain[ireg].append(ROOT.TChain("ftkdata","Road chain (R%dS%d)"%(ireg,isub)))
            road_stream[ireg].append(ROOT.FTKRoadStream())
# set the FTK file chains if roads are not merged
elif MERGE_SCHEME == 1 :
    road_chain = []
    road_stream = []
    track_chain = []
    track_stream = []
    for ireg in xrange(NREGIONS) :        
        road_chain.append([])
        road_stream.append([])
        track_chain.append([])
        track_stream.append([])
        for isub in xrange(NSUBREGIONS) :
            road_chain[ireg].append(ROOT.TChain("ftkdata","Road chain (R%dS%d)"%(ireg,isub)))
            road_stream[ireg].append(ROOT.FTKRoadStream())
            track_chain[ireg].append(ROOT.TChain("ftkdata","Tracks chain (R%dS%d)"%(ireg,isub)))
            track_stream[ireg].append(ROOT.FTKTrackStream())


# populate the merged tracks chain
for file in files_trackM :
    trackM_chain.Add(file)
trackM_chain.SetBranchAddress("FTKBankMerged",ROOT.AddressOf(trackM_stream))

# start the loops over regions and sub-regions for roads and tracks
for ireg in xrange(NREGIONS) :
    if ireg in TOSKIP :
        continue
    
    if MERGE_SCHEME == 0 :
        for file in files_track[ireg] :
            track_chain[ireg].Add(file)
        track_chain[ireg].SetBranchAddress("FTKBank%d."%ireg,ROOT.AddressOf(track_stream[ireg]))
    for isub in xrange(NSUBREGIONS) :
        for file in files_road[ireg][isub] :
            road_chain[ireg][isub].Add(file)
        road_chain[ireg][isub].SetBranchAddress("FTKBank%d."%ireg,ROOT.AddressOf(road_stream[ireg][isub]))
        if MERGE_SCHEME == 1 :
            for file in files_track[ireg][isub] :
                track_chain[ireg][isub].Add(file)
            track_chain[ireg][isub].SetBranchAddress("FTKBank%d."%ireg,ROOT.AddressOf(track_stream[ireg][isub]))

# check that the number of the events in each chain is the same
print "Check the chains have the same number of the events"
eventsInChain=-1
if DO_MERGEDTRACKS :
    print "In merged tracks chain found %d events" % trackM_chain.GetEntries()
    # this is the first check, can only set the reference value
    if eventsInChain<0 :
        eventsInChain = trackM_chain.GetEntries()
for ireg in xrange(NREGIONS) :
    if ireg in TOSKIP :
        continue
    if MERGE_SCHEME==0 :
        if DO_UNMERGEDTRACKS :
            print "In unmerged tracks chain (%d) found  %d events" % (ireg,track_chain[ireg].GetEntries())
            # set the reference value or check the consistency
            if eventsInChain<0 :
                eventsInChain = track_chain[ireg].GetEntries()
            elif eventsInChain!=track_chain[ireg].GetEntries() :
                print "Number of events in the track chains don't match (%d!=%d). exit" % (eventsInChain,track_chain[ireg].GetEntries())

        if DO_MERGEDROADS :
            print "In merged roads chain (%d) found  %d events" % (ireg,roadM_chain[ireg].GetEntries())
            # set the reference value or check the consistency
            if eventsInChain<0 :
                eventsInChain = roadM_chain[ireg].GetEntries()
            elif eventsInChain!=roadM_chain[ireg].GetEntries() :
                print "Number of events in the merged road chains don't match (%d!=%d). exit" % (eventsInChain,roadM_chain[ireg].GetEntries())


    for isub in xrange(NSUBREGIONS) :
        if MERGE_SCHEME==1 :
            if DO_UNMERGEDTRACKS :
                print "In unmerged tracks chain (%d,%d) found  %d events" % (ireg,isub,track_chain[ireg][isub].GetEntries())
                # set the reference value or check the consistency
                if eventsInChain<0 :
                    eventsInChain = track_chain[ireg][isub].GetEntries()
                elif eventsInChain!=track_chain[ireg][isub].GetEntries() :
                    print "Number of events in the unmerged track chains don't match (%d!=%d). exit" % (eventsInChain,track_chain[ireg][isub].GetEntries())

        if DO_UNMERGEDROADS :
            print "In unmerged roads chain (%d,%d) found  %d events" % (ireg,isub,road_chain[ireg][isub].GetEntries())
            # set the reference value or check the consistency
            if eventsInChain<0 :
                eventsInChain = road_chain[ireg][isub].GetEntries()
            elif eventsInChain!=road_chain[ireg][isub].GetEntries() :
                print "Number of events in the unmerged road chains don't match (%d!=%d). exit" % (eventsInChain,road_chain[ireg][isub].GetEntries())

print "Check passed"

if eventsInChain<=0 :
    print "!!! nothing to do, exit"
    sys.exit(-1)
else :
    print "Processing", eventsInChain, "events"
    
# open the output filename
outfile = ROOT.TFile.Open(ROOTNAME,"recreate")

# binning definition [nbins,min,max], number of bins should be even
curv_bins  =[50,-CURVLIM,CURVLIM]
pt_bins  =[50,1/(CURVLIM*2),20e3] 
phi_bins = [50,-pi,pi]
d0_bins = [50,-D0LIM,D0LIM]
eta_bins = [50,-ETALIM,ETALIM]
ctheta_bins = [50,-ROOT.TMath.SinH(ETALIM),ROOT.TMath.SinH(ETALIM)]
z0_bins = [50,-Z0LIM,Z0LIM]

phires_bins = [50,-.01,.01]
curvres_bins = [50,-5e-4,5e-4]
d0res_bins = [50,-.5,.5]
cthetares_bins = [50,-.03,.03]
z0res_bins = [50,-5,5]


# barrel and endcaps definitions
nendcap_lim = [-2.5,-1.1]
barrel_lim = [-.8,.8]
pendcap_lim = [1.1,2.5]


# create histogram grops
histos_fid = HistoGroupTruth("truth_fid")
histos_all = HistoGroupTruth("truth_all")
# histos_check = HistoGroup("truth_check",True)

#truth distribution of matches with merged tracks
if DO_MERGEDTRACKS :
    # group with only the FTK tracks matched to one turth track
    histos_trackeffM = HistoGroupTruth("ftk_effM")
    # residual for the a fake track, with barcode just under threshold
    histos_trackaeffM = HistoGroupTruth("ftk_trackaeffM")
    # residual of ftk tracks matching with the truth
    histos_trackresM = HistoGroupRes("ftk_resM")
    # residual of ftk, diveded by majority category
    histos_trackresMmaj = [HistoGroupRes("ftk_resMmaj%d" % im) for im in xrange(4)]
    # groups with all the FTK tracks
    histos_trackallM = HistoGroupFTK("ftk_allM")
    # ftk tracks matched to truth tracks, from hard scattering or pileup
    histos_trackfidM = HistoGroupFTK("ftk_fidM")
    # ftk tracks matching to any truth track
    histos_trackmatchedM = HistoGroupFTK("ftk_matchedM")
    # ftk tracks that are not matching to any truth track
    histos_trackfakeM = HistoGroupFTK("ftk_fakeM")
    # ftk tracks matched to a track already matched
    histos_trackduplM = HistoGroupFTK("ftk_duplM")

    
    ntracksM = 0

#truth distribution of matches with merged tracks
if DO_MERGEDTRACKSI :
    # group with only the FTK tracks matched to one turth track
    histos_trackeffIM = HistoGroupTruth("ftk_effIM")
    # residual for the a fake track, with barcode just under threshold
    histos_trackaeffIM = HistoGroupTruth("ftk_trackaeffIM")
    # residual of ftk tracks matching with the truth
    histos_trackresIM = HistoGroupRes("ftk_resIM")

    # groups with all the FTK tracks
    histos_trackallIM = HistoGroupFTK("ftk_allIM")
    # ftk tracks matched to truth tracks, from hard scattering or pileup
    histos_trackfidIM = HistoGroupFTK("ftk_fidIM")
    # ftk tracks that are not matching to any truth track
    histos_trackfakeIM = HistoGroupFTK("ftk_fakeIM")
    # ftk tracks matched to a track already matched
    histos_trackduplIM = HistoGroupFTK("ftk_duplIM")

    
    ntracksIM = 0

if DO_UNMERGEDROADS :
    if  prog_options.doefficiency :
        histos_roadeffA = HistoGroupTurth("road_effA")
        histos_roadeffR = []
        histos_roadeffS = []
        for ireg in xrange(NREGIONS) :
            if ireg in TOSKIP :
                histos_roadeffS.append([]) # empty structure
                histos_roadeffR.append([]) # empty structure
                continue

            histos_roadeffR.append(HistoGroupTruth("road_effR%d"%ireg))

            histos_roadeffS.append([])
            for isub in xrange(NSUBREGIONS) :
                histos_roadeffS[ireg].append(HistoGroupTruth("road_effR%dS%d"%(ireg,isub)))

        if DO_ROADSLINK711 :
            histos_linkeffA = HistoGroup("link_effA")
            histos_linkeffR = []
            histos_linkeffS = []
            for ireg in xrange(NREGIONS) :
                if ireg in TOSKIP :
                    histos_linkeffS.append([]) # empty structure
                    histos_linkeffR.append([]) # empty structure
                    continue

                histos_linkeffR.append(HistoGroupTruth("link_effR%d"%ireg))
                histos_linkeffS.append([])
                for isub in xrange(NSUBREGIONS) :
                    histos_roadeffS[ireg].append(HistoGroupTruth("link_effR%dS%d"%(ireg,isub)))

    if prog_options.dotiming :
        histos_nroadsA = HistoGroupNRoads("unmergedA")
        histos_nroadsR = []
        histos_nroadsS = []
        for ireg in xrange(NREGIONS) :
            if ireg in TOSKIP :
                histos_nroadsR.append([]) # empty structure
                histos_nroadsS.append([]) # empty structure
                continue

            histos_nroadsR.append(HistoGroupNRoads("unmergedR%d"%ireg))
            histos_nroadsS.append([])
            for isub in xrange(NSUBREGIONS) :
                histos_nroadsS[ireg].append(HistoGroupNRoads("unmergedR%dS%d"%(ireg,isub)))


if DO_UNMERGEDTRACKS :
    if prog_options.doefficiency : 
        histos_trackeffA = HistoGroupTruth("ftk_effA")
        histos_trackeffR = []
        if MERGE_SCHEME == 1:
            histos_trackeffS = []

        for ireg in xrange(NREGIONS) :
            if ireg in TOSKIP :
                histos_trackeffS.append([]) # empty structure
                histos_trackeffR.append([]) # empty structure
                continue

            histos_trackeffR.append(HistoGroupTruth("ftk_effR%d"%ireg))

            if MERGE_SCHEME == 1:
                histos_trackeffS.append([])
                for isub in xrange(NSUBREGIONS) :
                    histos_trackeffS[ireg].append(HistoGroupTruth("ftk_effR%dS%d"%(ireg,isub)))

    if prog_options.dotiming :
        histos_nfits_unmergedA = HistoGroupNFits("unmergedA")
        histos_nfits_unmergedR = []
        if MERGE_SCHEME == 1:
            histos_nfits_unmergedS = []
            histos_nfits_unmergedIS = []

        for ireg in xrange(NREGIONS) :
            if ireg in TOSKIP :
                histos_nfits_unmergedR.append([]) # empty structure
                histos_nfits_unmergedS.append([]) # empty structure
                continue

            histos_nfits_unmergedR.append(HistoGroupNFits("unmergedR%d"%ireg))

            if MERGE_SCHEME == 1:
                histos_nfits_unmergedS.append([])
                for isub in xrange(NSUBREGIONS) :
                    histos_nfits_unmergedS[ireg].append(HistoGroupNFits("unmergedR%dS%d"%(ireg,isub)))

# event loop
ievt = 0 # event counter
ievtstep = (NUMEVENTS+99)/100 # step to print a message
nalltracks = 0 # number of total processed tracks
nfidtracks = 0 # number of truth tracks in the fiducial cuts
rejNumber = [0]*10 # number of truth tracks rejected after each selection step
nalltracksFTK = 0 # total FTK tracks processed
nfidtracksFTK = 0 # total FTK tracks in the fiducial volume
rejNumberFTK = [0]*10 # total FTK tracks after any selection cut

while truthwrapper.nextevent() and ievt!=NUMEVENTS: # start the event loop
    if ievt != truthwrapper.event :
        print "Asynch read:", ievt, "-", truthwrapper.event
        break

    if ievt % ievtstep == 0 or VERBOSE>0:
        print "Event %6d of %6d (in file %6d)" %(ievt, NUMEVENTS, truthwrapper.localevent)
        print "Truth file:", truthwrapper.stream.name
        if DO_MERGEDTRACKS :
            print trackM_chain.GetCurrentFile().GetName()

    lstGoodTruth = [0]*len(truthwrapper.FTKEvent.truth_tracks)
    # print the list of truth tracks
    if VERBOSE==2 :
        truthwrapper.FTKEvent.PrintTruthTracks()


    # align the ROOT chain related to the FTK files
    if DO_MERGEDTRACKS :
        trackM_chain.GetEntry(ievt)
        # prepare the flag for matched tracks
        hasMatchM = [0]*trackM_stream.getNTracks()
        lstGoodM = [0]*trackM_stream.getNTracks()
        if DO_MERGEDTRACKSI :
            # prepare the flag incomplete matched tracks
            hasMatchIM = [0]*trackM_stream.getNTracksI()
            lstGoodIM = [0]*trackM_stream.getNTracksI()
        if VERBOSE==2 :
            trackM_chain.Show(ievt)
    if DO_UNMERGEDTRACKS :
        hasMatchUM = [None]*NREGIONS
    # align chain divided in regions
    for ireg in xrange(NREGIONS) :
        if ireg in TOSKIP :
            continue
        
        if MERGE_SCHEME == 0 :
            if DO_UNMERGEDTRACKS :
                track_chain[ireg].GetEntry(ievt)
                hasMatchUM[ireg] = [0]*track_chain[ireg].getNTracks()
                if VERBOSE==2 :
                    track_chain[ireg].Show(ievt)
            if DO_MERGEDROADS :
                roadM_chain[ireg].GetEntry(ievt)
                if VERBOSE==2 :
                    roadM_chain[ireg].Show(ievt)
        elif MERGE_SCHEME == 1 and DO_UNMERGEDTRACKS:
            hasMatchUM[ireg] = [None]*NSUBREGIONS

        # align chain divided in sub-regions
        for isub in xrange(NSUBREGIONS) :
            if DO_UNMERGEDROADS :
                road_chain[ireg][isub].GetEntry(ievt)
                if VERBOSE==2 :
                    road_chain[ireg][isub].Show(ievt)
            if MERGE_SCHEME == 1 :
                if DO_UNMERGEDTRACKS :
                    track_chain[ireg][isub].GetEntry(ievt)
                    hasMatchUM[ireg][isub] = [0]*track_stream[ireg][isub].getNTracks()
                    if VERBOSE==2 :
                        track_chain[ireg][isub].Show(ievt)

                            

        
    # loop over the truth tracks
    itruth = 0

    # block of variable used for event by event statistic information
    ESnTruth = 0 # number of used turth track
    ESnMatchMT = 0 # number of matched merged track
    ESnMatchMIT = 0 # number of matched incomplete merged track
    ESnMatchUT = 0 # number of matched unmerged track
    ESnMatchMR = 0 # number of matched merged roads
    ESnMatchUR = 0 # number of matched unmerged roads
    ESnMatchRL = 0 # number of matched road links


    if prog_options.dotiming : # do timing histograms
        if DO_UNMERGEDROADS :
            nroadsAM_A = 0
            nroadsRW_A = 0
            for ireg in xrange(NREGIONS) :
                if ireg in TOSKIP :
                    continue

                nroadsAM_R = 0
                nroadsRW_R = 0
                for isub in xrange(NSUBREGIONS) :
                    nroadsAM_S = road_stream[ireg][isub].naoGetNroadsAM()
                    nroadsRW_S = road_stream[ireg][isub].naoGetNroadsRW()
                    nroadsAM_R += nroadsAM_S
                    nroadsRW_R += nroadsRW_S
                    histos_nroadsS[ireg][isub].Fill(nroadsAM_S,nroadsRW_S)
                nroadsAM_A += nroadsAM_R
                nroadsRW_A += nroadsRW_R
                histos_nroadsR[ireg].Fill(nroadsAM_R,nroadsRW_R)
            histos_nroadsA.Fill(nroadsAM_A,nroadsRW_A)    


        if DO_UNMERGEDROADS :
            if MERGE_SCHEME==1 :
                # counters for all the region
                ncombsA = 0
                nfitsA = 0
                nfitsMajA = 0
                nfitsRecA = 0
                nfitsRecFitA = 0
                nfitsBadA  = 0
                nfitsBadMajA = 0
                nfitsRejA = 0
                nfitsRejMajA = 0
                
                ncombsIA = 0
                nfitsIA = 0
                nfitsMajIA = 0
                nfitsRecIA = 0
                nfitsRecFitIA = 0
                nfitsBadIA  = 0
                nfitsBadMajIA = 0
                nfitsRejIA = 0
                nfitsRejMajIA = 0

                for ireg in xrange(NREGIONS) :
                    if ireg in TOSKIP :
                        continue

                    # counters region based
                    ncombsR = 0
                    nfitsR = 0
                    nfitsMajR = 0
                    nfitsRecR = 0
                    nfitsRecFitR = 0
                    nfitsBadR = 0
                    nfitsBadMajR = 0
                    nfitsRejR = 0
                    nfitsRejMajR = 0

                    ncombsIR = 0
                    nfitsIR = 0
                    nfitsMajIR = 0
                    nfitsRecIR = 0
                    nfitsRecFitIR = 0
                    nfitsBadIR = 0
                    nfitsBadMajIR = 0
                    nfitsRejIR = 0
                    nfitsRejMajIR = 0

                    for isub in xrange(NSUBREGIONS) :
                        # retrieve the data on number of fits for this subregion
                        ncombsS = track_stream[ireg][isub].getNCombs()
                        nfitsS = track_stream[ireg][isub].getNFits()
                        nfitsMajS = track_stream[ireg][isub].getNFitsMajority()
                        nfitsRecS = track_stream[ireg][isub].getNFitsRecovery()
                        nfitsRecFitS = track_stream[ireg][isub].getNAddFitsRecovery()
                        nfitsBadS = track_stream[ireg][isub].getNFitsBad()
                        nfitsBadMajS = track_stream[ireg][isub].getNFitsBadMajority()
                        nfitsRejS = track_stream[ireg][isub].getNFitsHWRejected()
                        nfitsRejMajS = track_stream[ireg][isub].getNFitsHWRejectedMajority()
                        
                        ncombsIS = track_stream[ireg][isub].getNCombsI()
                        nfitsIS = track_stream[ireg][isub].getNFitsI()
                        nfitsMajIS = track_stream[ireg][isub].getNFitsMajorityI()
                        nfitsRecIS = track_stream[ireg][isub].getNFitsRecoveryI()
                        nfitsRecFitIS = track_stream[ireg][isub].getNAddFitsRecoveryI()
                        nfitsBadIS = track_stream[ireg][isub].getNFitsBadI()
                        nfitsBadMajIS = track_stream[ireg][isub].getNFitsBadMajorityI()
                        nfitsRejIS = track_stream[ireg][isub].getNFitsHWRejectedI()
                        nfitsRejMajIS = track_stream[ireg][isub].getNFitsHWRejectedMajorityI()
                        
                        # fill the sub-region histograms
                        histos_nfits_unmergedS[ireg][isub].Fill(ncombsS,nfitsS,nfitsMajS,
                                                                nfitsRecS,nfitsRecFitS,
                                                                nfitsBadS,nfitsBadMajS,
                                                                nfitsRejS,nfitsRejMajS)
                        
                        histos_nfits_unmergedS[ireg][isub].FillIncomplete(ncombsIS,nfitsIS,nfitsMajIS,
                                                                          nfitsRecIS,nfitsRecFitIS,
                                                                          nfitsBadIS,nfitsBadMajIS,
                                                                          nfitsRejIS,nfitsRejMajIS)
                            
                        # add the sub-region results to the region counters
                        ncombsR += ncombsS
                        nfitsR += nfitsS
                        nfitsMajR += nfitsMajS
                        nfitsRecR += nfitsRecS
                        nfitsRecFitR += nfitsRecFitS
                        nfitsBadR += nfitsBadS
                        nfitsBadMajR += nfitsBadMajS
                        nfitsRejR += nfitsRejS
                        nfitsRejMajR += nfitsRejMajS
                        
                        ncombsIR += ncombsIS
                        nfitsIR += nfitsIS
                        nfitsMajIR += nfitsMajIS
                        nfitsRecIR += nfitsRecIS
                        nfitsRecFitIR += nfitsRecFitIS
                        nfitsBadIR += nfitsBadIS
                        nfitsBadMajIR += nfitsBadMajIS
                        nfitsRejIR += nfitsRejIS
                        nfitsRejMajIR += nfitsRejMajIS
                        # end sub-region loop

                    # fill the region based histogram
                    histos_nfits_unmergedR[ireg].Fill(ncombsR,nfitsR,nfitsMajR,
                                                      nfitsRecR,nfitsRecFitR,
                                                      nfitsBadR,nfitsBadMajR,
                                                      nfitsRejR,nfitsRejMajR)
                    
                    histos_nfits_unmergedR[ireg].FillIncomplete(ncombsIR,nfitsIR,nfitsMajIR,
                                                                nfitsRecIR,nfitsRecFitIR,
                                                                nfitsBadIR,nfitsBadMajIR,
                                                                nfitsRejIR,nfitsRejMajIR)

                    # add the region results to the global counters
                    ncombsA += ncombsR
                    nfitsA += nfitsR
                    nfitsMajA += nfitsMajR
                    nfitsRecA += nfitsRecR
                    nfitsRecFitA += nfitsRecFitR
                    nfitsBadA += nfitsBadR
                    nfitsBadMajA += nfitsBadMajR
                    nfitsRejA += nfitsRejR
                    nfitsRejMajA += nfitsRejMajR
                    
                    ncombsIA += ncombsIR
                    nfitsIA += nfitsIR
                    nfitsMajIA += nfitsMajIR
                    nfitsRecIA += nfitsRecIR
                    nfitsRecFitIA += nfitsRecFitIR
                    nfitsBadIA += nfitsBadIR
                    nfitsBadMajIA += nfitsBadMajIR
                    nfitsRejIA += nfitsRejIR
                    nfitsRejMajIA += nfitsRejMajIR
                    # end region loop
                    
                # fill the global histograms
                histos_nfits_unmergedA.Fill(ncombsA,nfitsA,nfitsMajA,
                                            nfitsRecA,nfitsRecFitA,
                                            nfitsBadA,nfitsBadMajA,
                                            nfitsRejA,nfitsRejMajA)
                
                histos_nfits_unmergedA.FillIncomplete(ncombsIA,nfitsIA,nfitsMajIA,
                                                      nfitsRecIA,nfitsRecFitIA,
                                                      nfitsBadIA,nfitsBadMajIA,
                                                      nfitsRejIA,nfitsRejMajIA)
                
            elif MERGE_SCHEME==0 :
                print "not implemented"
            # end block used to fill timing histograms for unmerged tracks
        

    if prog_options.doefficiency :
        # truth loop to build the efficiency plots
        for truth in truthwrapper.FTKEvent.truth_tracks :
            nalltracks += 1

            if VERBOSE>2 :
                print "Truth track:",
                truth.Print()

            histos_all.Fill(truth)

            # check if the track is good, the block for the rejection is filled
            # within the function
            if not isGoodTruth(truth) :
                continue

            if VERBOSE>2 :
                print "Fiducial"
            nfidtracks += 1
            ESnTruth += 1
            lstGoodTruth[itruth] = 1

            # fill the reference histogram for the truth tracks in the
            # fiducial region
            histos_fid.Fill(truth)

            # check for matches in the merged tracks collection
            if DO_MERGEDTRACKS :
                if VERBOSE>3 :
                    print "Match with merged tracks"
                (iftk,nmatchM) = MatchTruthFTK(truth,trackM_stream,hasMatchM)

                # fill the histograms
                if nmatchM>0 :
                    # uses the ID of the matched tracks
                    ftk_track = trackM_stream.getTrack(iftk)
                    histos_trackresM.CompTruthFTK(truth,ftk_track)
                    maj_type = getMajorityType(ftk_track)
                    histos_trackresMmaj[maj_type].CompTruthFTK(truth,ftk_track)
                    histos_trackeffM.Fill(truth)
                    ESnMatchMT += 1

                    if PRINTMATCHES :
                        matchedlist.write("Merged-tracks %d %d\n" % (ievt, itruth))
                elif iftk>=0 :
                    # fill the plots of the "almost matching" FTK tracks
                    ftk_track = trackM_stream.getTrack(iftk)
                    #histos_trackresM.CompTruthFTK(truth,ftk_track)
                    #maj_type = getMajorityType(ftk_track)
                    #histos_trackresMmaj[maj_type].CompTruthFTK(truth,ftk_track)
                    histos_trackaeffM.Fill(truth)

                    # check for matches in the merged tracks collection
            if DO_MERGEDTRACKSI :
                if VERBOSE>3 :
                    print "Match with incomplete merged tracks"
                (iftk,nmatchIM) = MatchTruthFTK(truth,trackM_stream,hasMatchIM,useIncomplete=True)

                # fill the histograms
                if nmatchIM>0 :
                    # uses the ID of the matched tracks
                    ftk_track = trackM_stream.getTrackI(iftk)
                    histos_trackresIM.CompTruthFTK(truth,ftk_track)
                    histos_trackeffIM.Fill(truth)
                    ESnMatchMIT += 1

                    if PRINTMATCHES :
                        matchedlist.write("Merged-incomplete-tracks %d %d\n" % (ievt, itruth))
                elif iftk>=0 :
                    # fill the plots of the "almost matching" FTK tracks
                    ftk_track = trackM_stream.getTrackI(iftk)
                    #histos_trackresM.CompTruthFTK(truth,ftk_track)
                    #maj_type = getMajorityType(ftk_track)
                    #histos_trackresMmaj[maj_type].CompTruthFTK(truth,ftk_track)
                    histos_trackaeffIM.Fill(truth)


            # check for matched in roads collection
            if DO_UNMERGEDROADS :
                nmatchA = 0            
                nmatchALink = 0 # number of matched link
                # region loop
                for ireg in xrange(NREGIONS) :
                    if ireg in TOSKIP :
                        continue
                    nmatchR = 0
                    # sub-region loop
                    for isub in xrange(NSUBREGIONS) :
                        nmatchS = 0
                        if VERBOSE>3 :
                            print "Road in %d @ %d" % (ireg,isub)
                            road_stream[ireg][isub].Print(1)

                        nroads = road_stream[ireg][isub].getNRoads()
                        # if enabled compute the number of segments
                        if DO_ROADSLINK711 and nroads>0:
                            # get the number of unused layers with hits
                            nhits_seg = road_stream[ireg][isub].getNUnusedPlanes()
                            # evaluate the number of combinations of nhist_seg
                            # segments and return how many
                            ncombs_more = road_stream[ireg][isub].computeUnusedSegmentsTruthList()
                            if VERBOSE>3 :
                                print "Number of segments:", ncombs_more

                        # road loop
                        for iroad in xrange(nroads) :
                            road_stream[ireg][isub].attachHits(iroad)
                            ftk_road = road_stream[ireg][isub].getRoad(iroad)
                            if iroad == 0:
                                nplanes = ftk_road.getNPlanes()
                            nhits_road = ftk_road.getNHits()

                            # road quality check
                            if nplanes-nhits_road>MAXMISSING :
                                continue
                            elif prog_options.requirefirst :
                                if not ftk_road.getBitMask() & 1:
                                    continue
                            # build the list of hit combination and return their number
                            ncombs = ftk_road.computeTruthList()
                            if VERBOSE>3 :
                                print "Number combs in road %d =" % ftk_road.getPatternID(), ncombs
                            # loop over the hit combinations
                            for icomb in xrange(ncombs) :
                                evtidx = ftk_road.getEventIndex(icomb)
                                barcode = ftk_road.getBarcode(icomb)
                                barfrac = ftk_road.getBarcodeFrac(icomb)
                                if barcode==truth.barcode and barfrac>=prog_options.matchfrac :
                                    nmatchA += 1
                                    nmatchR += 1
                                    nmatchS += 1
                                    if VERBOSE>3 :
                                        print "Match with frac. (%f,%d)" % (barfrac,nhits_road), " event", evtidx

                                    if DO_ROADSLINK711 :
                                        # if the check over the 7/11 extrapolation is active check if it is possible
                                        # to the matched road can be seccefully extrapolated in the further layers
                                        for icomb_more in xrange(ncombs_more) :
                                            evtidx_seg  = road_stream[ireg][isub].getEventIndex(icomb_more)
                                            barcode_seg = road_stream[ireg][isub].getBarcode(icomb_more)
                                            barfrac_seg = road_stream[ireg][isub].getBarcodeFrac(icomb_more)
                                            if barfrac_seg*nhits_seg < 3 :
                                                continue

                                            if barfrac_seg==.75 and nhits_seg==4 :
                                                # suppose that in this case a 3 hit segment
                                                # is the one that the extrapolation procedure find
                                                barfrac_thisseg = 1.
                                                nhits_thisseg = 3
                                            else :
                                                nhits_thisseg = nhits_seg
                                                barfrac_thisseg = barfrac_seg
                                            if barcode == barcode_seg :
                                                frac_tot = (barfrac*nhits_road+barfrac_thisseg*nhits_thisseg)/(nhits_road+nhits_thisseg)
                                                if barcode==truth.barcode and frac_tot>=prog_options.matchfrac :
                                                    nmatchALink += 1
                                                    if VERBOSE>3 :
                                                        print "Further match with frac.", frac_tot, " event", evtidx
                                                        print "Fraction (%f,%f), from (%f,%f)" % (barfrac_seg,nhits_seg,barfrac_thisseg,nhits_thisseg)

                            road_stream[ireg][isub].detachHits(iroad)
                        # ended road loop
                    # ended sub-region loop
                # ended region loop
                if nmatchA>0 :
                    histos_roadeffA.Fill(truth)
                    ESnMatchUR += 1

                    if PRINTMATCHES :
                        matchedlist.write("Unmerged-roads %d %d whole\n" % (ievt, itruth))
                if nmatchALink>0 :
                    histos_linkeffA.Fill(truth)
                    ESnMatchRL += 1
                    if PRINTMATCHES :
                        matchedlist.write("Unmerged-roads link %d %d whole\n" % (ievt, itruth))


            # check for matches in the merged tracks collection
            if DO_UNMERGEDTRACKS :
                if MERGE_SCHEME==1 :
                    # number of tracks, in all sub regions that match
                    nmatchA = 0
                    # loop over the regions
                    for ireg in xrange(NREGIONS) :
                        if ireg in TOSKIP :
                            continue

                        # number of tracks, in this region, that match
                        nmatchR = 0
                        # sub-region loop
                        for isub in xrange(NSUBREGIONS) :
                            # number of matching tracks in this sub-region
                            if VERBOSE>3 :
                                print "Match with tracks in %d-%d" % (ireg,isub)
                            (iftk,nmatchS)  = MatchTruthFTK(truth,track_stream[ireg][isub],hasMatchUM[ireg][isub])

                            # fill sub-regions histograms
                            if nmatchS>0 :
                                # get the tracks
                                ftk_track = track_stream[ireg][isub].getTrack(iftk)
                                nmatchA += 1
                                nmatchR += 1
                                histos_trackeffS[ireg][isub].Fill(truth)
                                if PRINTMATCHES :
                                    matchedlist.write("Unmerged-tracks %d %d reg %d sub %d\n" % (ievt, itruth,ireg,isub))
                        # ended sub-region loop

                        # fill region histograms
                        if nmatchR>0 :
                            histos_trackeffR[ireg].Fill(truth)
                            if PRINTMATCHES :
                                matchedlist.write("Unmerged-tracks %d %d reg %d\n" % (ievt, itruth,ireg))
                    #ended the region loop

                    # fill global histograms
                    if nmatchA>0 :
                        histos_trackeffA.Fill(truth)
                        ESnMatchUT += 1
                        if PRINTMATCHES :
                            matchedlist.write("Unmerged-tracks %d %d whole\n" % (ievt, itruth))
                # end block for unmerged roads scheme
                elif MERGE_SCHEME==0 :
                    print "not implemented"
                # end block for merged roads scheme
            itruth += 1
            # end truth loop

        if DO_MERGEDTRACKS :
            nftktracks = trackM_stream.getNTracks()
            nalltracksFTK += nftktracks

            # build an array to keep track of already matched truth
            hasMatchTruth = [False]*len(truthwrapper.FTKEvent.truth_tracks)

            if VERBOSE>0 :
                print "Loop over FTK tracks", nftktracks
            for iftk in xrange(nftktracks) :
                ftk_track = trackM_stream.getTrack(iftk)
                histos_trackallM.Fill(ftk_track)

                # check if the track is fiducial
                if not isGoodFTK(ftk_track) :
                    continue

                lstGoodM[iftk] = 1
                nfidtracksFTK += 1
                histos_trackfidM.Fill(ftk_track)

                if VERBOSE>2 :
                    print "FTK track",iftk
                    ftk_track.Print()

                if hasMatchM[iftk] == 0 :
                    (hasMatchM[iftk], itruth) = MatchFTKTruth(ftk_track,truthwrapper.FTKEvent.truth_tracks,hasMatchTruth)
                    # if is still unmatched fill the fake histograms
                    if hasMatchM[iftk] == 0 :
                        histos_trackfakeM.Fill(ftk_track)
                        if VERBOSE>2 :
                            print "Fake"
                    elif hasMatchM[iftk] == 2 :
                        histos_trackduplM.Fill(ftk_track)
                        if VERBOSE>2 :
                            print "Fake Dupl"
                    else :
                        histos_trackmatchedM.Fill(ftk_track)
                        if VERBOSE>2 :
                            print "Matched to bad truth"

                elif hasMatchM[iftk] == 1:
                    histos_trackmatchedM.Fill(ftk_track)
                    if VERBOSE>2 :
                        print "Already matched"
                elif hasMatchM[iftk] == 2 :
                    histos_trackduplM.Fill(ftk_track)
                    if VERBOSE>2 :
                        print "Duplicate of a matched truth"
                
    # end of the event loop, increment the event counter
    # can print an event summary
    if VERBOSE>0 :
        print "Summary of event #", ievt,
        print "# Truth", ESnTruth,
        if DO_UNMERGEDROADS :
            print "# Merged Roads", ESnMatchUR,
        if DO_UNMERGEDROADS :
            print "# Merged Links", ESnMatchRL,
        if DO_MERGEDROADS :
            print "# Merged Roads", ESnMatchMR,
        if DO_UNMERGEDTRACKS :
            print "# Merged Roads", ESnMatchUT,
        if DO_MERGEDTRACKS :
            print "Merged", ESnMatchMT,
        print
    if VERBOSE>1 :
        PrintListTruth()
        PrintListMatched()


    if prog_options.interactive :
        spycmd = "first"
        spyexit = ["","exit"]
        while not spycmd in spyexit :
            spycmd = raw_input("CMD> ")
            if spycmd in spyexit :
                break
            try :
                compobj = compile(spycmd,"<string>","single")
                eval(compobj)
            except :
                print "Error evaluting command:", spycmd
    ievt += 1

print "Summary of rejection for truth tracks: tot # tracks=%d (Fiducials %d)" % (nalltracks,nfidtracks)

print "Rejected by each step:", rejNumber
rejFrac = []
nres = nalltracks
for val in rejNumber :
    if nres>0 :
        rejFrac.append(float(val)/nres)
    else :
        rejFrac.append(-1)
    nres -= val
print "Rejection factors:", rejFrac

print "Summary of rejection for FTK tracks: tot # tracks=%d (Fiducials %d)" % (nalltracksFTK,nfidtracksFTK)

print "Rejected by each step:", rejNumberFTK
rejFracFTK = []
nres = nalltracksFTK
for val in rejNumberFTK :
    if nres>0 :
        rejFracFTK.append(float(val)/nres)
    else :
        rejFracFTK.append(-1)
    nres -= val
print "Rejection factors:", rejFrac

print "Saving results in",  ROOTNAME, "...",
outfile.Write()
print "re-open in read mode...",
outfile.ReOpen("read")
print "Done"

# print a short summary for the efficiency plots
if nfidtracks > 10 and prog_options.doefficiency:
    if DO_UNMERGEDROADS :
        histos_roadeffA.PrintEff(histos_fid,"Efficiency unmerged roads (all)")
        if DO_ROADSLINK711 :
            histos_linkeffA.PrintEff(histos_fid,"Efficiency unmerged roads link (all)")
    if DO_UNMERGEDTRACKS :
        histos_trackeffA.PrintEff(histos_fid,"Efficiency unmerged tracks (all)")
    if DO_MERGEDTRACKS:
        histos_trackeffM.PrintEff(histos_fid,"Efficiency merged tracks")        
elif prog_options.doefficiency :    
    print "Not enough tracks to evaluate reference table"

# print a short summary for the fake plots
if nfidtracksFTK > 10 and prog_options.doefficiency:
    if DO_MERGEDTRACKS:
        histos_trackfakeM.PrintEff(histos_trackfidM,"Fake merged tracks")
        histos_trackduplM.PrintEff(histos_trackfidM,"Dupl merged tracks")
elif prog_options.doefficiency :
    print "Not enough tracks to evaluate reference table"
