# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ROOT import TH1F, TH2F

#
#  Ranges
#

etaMin  = -3.4
etaMax  = 3.4


class PtStudy:
    
    class PtStudyHist:
        def __init__(self, param, title, min, max):
            self.vsEta    = TH2F("histo_res"+param, "Res "+title, 100, min,  max,  30,  etaMin,   etaMax)
            self.pt       = TH1F("pt_"+param,      param+"Tacks;p_{T};Freq.",       50, 0 ,     100*1000)

        def Fill(self, res, eta, Ipt):
            self.vsEta      .Fill(res,  eta)
            if Ipt:
                self.pt         .Fill(.5/abs(Ipt) )

        def Write(self):
            self.vsEta    .Write()
            self.pt       .Write()


    def __init__(self,param,title,min,max):

        self.vsEta01    = self.PtStudyHist(param+"_eta01", title+"_eta01", min,  max)
        self.vsEta02    = self.PtStudyHist(param+"_eta02", title+"_eta02", min,  max)
        self.vsEta03    = self.PtStudyHist(param+"_eta03", title+"_eta03", min,  max)
        self.vsEta04    = self.PtStudyHist(param+"_eta04", title+"_eta04", min,  max)
        self.vsEta05    = self.PtStudyHist(param+"_eta05", title+"_eta05", min,  max)

    def Fill(self,res,Ipt,eta,hasBL):

        if not hasBL:
            return 

        if abs(Ipt) < 0.1e-3:
            self.vsEta01      .Fill(res,  eta, Ipt)
        elif abs(Ipt) < 0.2e-3:
            self.vsEta02      .Fill(res,  eta, Ipt)
        elif abs(Ipt) < 0.3e-3:
            self.vsEta03      .Fill(res,  eta, Ipt)
        elif abs(Ipt) < 0.4e-3:
            self.vsEta04      .Fill(res,  eta, Ipt)
        else:
            self.vsEta05      .Fill(res,  eta, Ipt)

    def Write(self):

        self.vsEta01    .Write()
        self.vsEta02    .Write()
        self.vsEta03    .Write()
        self.vsEta04    .Write()
        self.vsEta05    .Write()
