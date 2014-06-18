# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ROOT import TH1F, TLorentzVector

from math import cos, sin, acos, sqrt

class Track:
    def __init__(self,eta,phi,pt,d0,z0):
        self.fourVec = TLorentzVector()
        self.fourVec.SetPtEtaPhiM(pt, eta, phi, 0)
        self.eta = eta
        self.phi = phi
        self.pt  = pt
        self.d0  = d0
        self.z0  = z0


class TrackHists:
    def __init__(self,name):
        self.name = name
        self.d0 = TH1F(name+"_d0", "d0;d0[mm];tracks", 100, -2, 2)
        self.z0 = TH1F(name+"_z0", "z0;z0[mm];tracks", 100, -10, 10)
        self.pT = TH1F(name+"_pT", "pT;pT[GeV];tracks", 100,0, 200)
        self.phi = TH1F(name+"_phi", "phi;phi;tracks", 100,-3.2, 3.2)
        self.eta = TH1F(name+"_eta", "eta;eta;tracks", 100,-2.6, 2.6)

    # Takes a NtupleHelpers.Track object
    def Fill(self,trk):
        self.eta.Fill(trk.eta)
        self.phi.Fill(trk.phi)
        self.d0 .Fill(trk.d0)
        self.pT .Fill(trk.pt/1000)
        self.z0 .Fill(trk.z0)

    def Write(self):
        self.eta.Write()
        self.phi.Write()
        self.z0.Write()
        self.pT.Write()
        self.d0.Write()        


class TrackCompHists:
    def __init__(self,name):
        self.name = name
        self.d0  = TH1F(name+"_dd0", "dd0;#Detla d0[mm];tracks", 100, -5, 5)
        self.z0  = TH1F(name+"_dz0", "dz0;#Detla z0[mm];tracks", 100, -2, 2)
        self.pT  = TH1F(name+"_dpT", "dpT;#Delta pT[GeV];tracks", 100,-5, 5)
        self.phi = TH1F(name+"_dphi", "dphi;#Delta phi;tracks", 100,-0.5, 0.5)
        self.eta = TH1F(name+"_deta", "deta;eta;tracks", 100,-0.5, 0.5)
        self.dR = TH1F(name+"_dR", "dR;dR;tracks", 100,0, 3)

    # Takes a NtupleHelpers.Track object
    def Fill(self,trk1,trk2):
        self.eta.Fill(trk1.eta - trk2.eta)
        self.phi.Fill(trk1.fourVec.DeltaPhi(trk2.fourVec))
        self.d0 .Fill(trk1.d0 - trk2.d0)
        self.pT .Fill((trk1.pt-trk2.pt)/1000)
        self.z0 .Fill(trk1.z0-trk2.z0)
        self.dR .Fill(trk1.fourVec.DeltaR(trk2.fourVec))

    def Write(self):
        self.eta.Write()
        self.phi.Write()
        self.z0.Write()
        self.pT.Write()
        self.d0.Write()
        self.dR.Write()        
