# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ROOT import TH2F

#
#  Resolution Ranges
#

resPtMin  = -5e-5
resPtMax  =  5e-5
resPhiMin = -0.01
resPhiMax = 0.01
resD0Min  = -0.5
resD0Max  = 0.5
resEtaMin = -0.01
resEtaMax = 0.01
resZ0Min  = -1
resZ0Max  = 1


class CovStudy:

    def __init__(self,param,title,min,max):

        #
        #  With BLayer Hits
        #
        self.vsResInvPt  = TH2F("histo_res"+param+"_vsResPt",  "Res "+title+"_resInvPt; #Delta Ipt; #Delta"+param,  30, min,  max,  30,  resPtMin/2,  resPtMax/2)
        self.vsResEta    = TH2F("histo_res"+param+"_vsResEta", "Res "+title+"_resEta; #Delta #eta; #Delta"+param,    30, min,  max,  30,  resEtaMin,   resEtaMax)
        self.vsResPhi    = TH2F("histo_res"+param+"_vsResPhi", "Res "+title+"_resPhi; #Delta #phi; #Delta"+param,    30, min,  max,  30,  resPhiMin,   resPhiMax)
        self.vsResD0     = TH2F("histo_res"+param+"_vsResD0",  "Res "+title+"_resD0; #Delta d0; #Delta"+param,    30, min,  max,  30,  resD0Min,   resD0Max)
        self.vsResZ0     = TH2F("histo_res"+param+"_vsResZ0",  "Res "+title+"_resZ0; #Delta z0; #Delta"+param,    30, min,  max,  30,  resZ0Min,   resZ0Max)


    def Fill(self,res,resIpt,resEta,resPhi,resD0,resZ0):

            self.vsResInvPt    .Fill(res,  resIpt)
            self.vsResEta      .Fill(res,  resEta)
            self.vsResPhi      .Fill(res,  resPhi)
            self.vsResD0       .Fill(res,  resD0 )
            self.vsResZ0       .Fill(res,  resZ0 )

    def printCor(self,hist):
        print hist.GetName(),hist.GetCorrelationFactor()

    def Write(self):
        self.vsResInvPt    .Write()
        self.vsResEta      .Write()
        self.vsResPhi      .Write()
        self.vsResD0       .Write()
        self.vsResZ0       .Write()

        self.printCor(self.vsResInvPt)
        self.printCor(self.vsResEta)
        self.printCor(self.vsResPhi)
        self.printCor(self.vsResD0)
        self.printCor(self.vsResZ0)
