# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ROOT import TH1F, TH2F

#
#  Parameter Ranges
#

ptMin  = -6e-4
ptMax  =  6e-4
phiMin = -3.2
phiMax = 3.2
d0Min  = -3
d0Max  = 3
etaMin  = -3.4
etaMax  = 3.4
z0Min  = -110
z0Max  = 110
z0stMin  = -110
z0stMax  = 110


class TPHists:
    def __init__(self,name):    

        self.curv      = TH1F(name+"_curv",    "All tracks;1/p_{T};Freq.",     50, ptMin , ptMax)
        self.pt        = TH1F(name+"_pt",      "All tracks;p_{T};Freq.",       50, 0 ,     100*1000)
        self.phi       = TH1F(name+"_phi",     "All tracks;#phi;Freq.",        50, phiMin, phiMax)
        self.d0        = TH1F(name+"_d0",      "All tracks;d_{0} [mm];Freq.",  50, d0Min,  d0Max)
        self.eta       = TH1F(name+"_eta",     "All tracks;#eta;Freq.",        50, etaMin, etaMax) 
        self.z0        = TH1F(name+"_z0",      "All tracks;z_{0} [mm];Freq.",  50, z0Min,  z0Max) 
        self.z0st      = TH1F(name+"_z0st",    "All tracks;z_{0}sin(#theta) [mm];Freq.",  50, z0stMin,  z0stMax) 
        self.phieta    = TH2F(name+"_phieta",  "All tracks;#phi;#eta;Freq.",   50, phiMin, phiMax,50,etaMin,etaMax)

        self.curv      .Sumw2()
        self.pt        .Sumw2()
        self.phi       .Sumw2()
        self.d0        .Sumw2()
        self.eta       .Sumw2()
        self.z0        .Sumw2()
        self.z0st      .Sumw2()
        self.phieta    .Sumw2()



    def Fill(self,Ipt,phi,d0,eta,z0,z0st):    
        self.curv   .Fill(Ipt)
        if Ipt:
            self.pt   .Fill(.5/abs(Ipt))

        self.phi    .Fill(phi)
        self.d0     .Fill(d0)
        self.eta    .Fill(eta)
        self.z0     .Fill(z0)
        self.z0st     .Fill(z0st)
        self.phieta .Fill(phi, eta)

    def Write(self):    
        self.curv   .Write()
        self.pt     .Write()
        self.phi    .Write()
        self.d0     .Write()
        self.eta    .Write()
        self.z0     .Write()
        self.z0st     .Write()
        self.phieta .Write()

#
#  Resolution vs TP
#

class ResVsTPs:

    def __init__(self,param,title,min,max):

        #
        #  With BLayer Hits
        #
        self.res      = TH1F("histo_res"+param,          "Res "+title,           50, min,  max)
        self.vsInvPt  = TH2F("histo_res"+param+"_InvPt", "Res "+title+"_InvPt",  30, min,  max,  30,  ptMin,    ptMax)
        self.vsEta    = TH2F("histo_res"+param+"_eta",   "Res "+title+"_eta",   300, min,  max,  30,  etaMin,   etaMax)
        self.vsPhi    = TH2F("histo_res"+param+"_phi",   "Res "+title+"_phi",    30, min,  max,  30,  phiMin,   phiMax)

        #
        #  Without BLayer Hits
        #
        self.res_nb      = TH1F("histo_res"+param+"_nb",      "Res "+title,             50, min,  max)
        self.vsInvPt_nb  = TH2F("histo_res"+param+"_InvPt_nb","Res "+title+"_InvPt_nb", 30, min,  max,  30, ptMin,   ptMax)
        self.vsEta_nb    = TH2F("histo_res"+param+"_eta_nb",  "Res "+title+"_eta_nb",   30, min,  max,  30, etaMin,  etaMax)
        self.vsPhi_nb    = TH2F("histo_res"+param+"_phi_nb",  "Res "+title+"_phi_nb",   30, min,  max,  30, phiMin,  phiMax)

    def Fill(self,res,Ipt,eta,phi,hasBL):

        if hasBL:
            self.res        .Fill(res)
            self.vsInvPt    .Fill(res,  Ipt)
            self.vsEta      .Fill(res,  eta)
            self.vsPhi      .Fill(res,  phi)
        else:
            self.res_nb     .Fill(res)
            self.vsInvPt_nb .Fill(res,  Ipt)
            self.vsEta_nb   .Fill(res,  eta)
            self.vsPhi_nb   .Fill(res,  phi)

    def Write(self):
        self.res        .Write()
        self.vsInvPt    .Write()
        self.vsEta      .Write()
        self.vsPhi      .Write()

        self.res_nb     .Write()
        self.vsInvPt_nb .Write()
        self.vsEta_nb   .Write()
        self.vsPhi_nb   .Write()

