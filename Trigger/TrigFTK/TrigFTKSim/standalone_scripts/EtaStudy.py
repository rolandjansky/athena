# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ROOT import TH1F, TH2F

#
#  Parameter Ranges
#

ptMin  = -6e-4
ptMax  =  6e-4

class EtaStudy:
    
    class EtaStudyHist:
        def __init__(self, param, title, min, max):
            self.vsPt    = TH2F("histo_res"+param, "Res "+title, 100, min,  max,  30,  ptMin,   ptMax)
            self.eta     = TH1F("eta_"+param,      param+"Tacks;|#eta|;Freq.",       50, -0.1 ,     3.4)

        def Fill(self, res, eta, Ipt):
            self.vsPt      .Fill(res,  Ipt)
            if Ipt:
                self.eta         .Fill( abs(eta) )

        def Write(self):
            self.vsPt    .Write()
            self.eta     .Write()


    def __init__(self,param,title,min,max):

        self.vsPt0    = self.EtaStudyHist(param+"_pt0", title+"_pt0", min,  max)
        self.vsPt1    = self.EtaStudyHist(param+"_pt1", title+"_pt1", min,  max)
        self.vsPt2    = self.EtaStudyHist(param+"_pt2", title+"_pt2", min,  max)
        self.vsPt3    = self.EtaStudyHist(param+"_pt3", title+"_pt3", min,  max)
        self.vsPt4    = self.EtaStudyHist(param+"_pt4", title+"_pt4", min,  max)

    def Fill(self,res,Ipt,eta,hasBL):

        if not hasBL:
            return 

        if abs(eta) < 0.5:
            self.vsPt0      .Fill(res,  eta, Ipt)
        elif abs(eta) < 1.0:
            self.vsPt1      .Fill(res,  eta, Ipt)
        elif abs(eta) < 1.5:
            self.vsPt2      .Fill(res,  eta, Ipt)
        elif abs(eta) < 2.0:
            self.vsPt3      .Fill(res,  eta, Ipt)
        else:
            self.vsPt4      .Fill(res,  eta, Ipt)

    def Write(self):

        self.vsPt0    .Write()
        self.vsPt1    .Write()
        self.vsPt2    .Write()
        self.vsPt3    .Write()
        self.vsPt4    .Write()
