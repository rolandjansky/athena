# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import ROOT
import HistControl

class EventAnalyzer( ROOT.TNamed ):

    def __init__( self, name = "EventAnalyzer", title = "EventAnalyzer", histControlName='histControl', numberOfDarkPhotons=1 ):
        # Initialise the base class
        ROOT.TNamed.__init__( self, name, title )
        self.SetName( name )
        self.SetTitle( title )
        self.__nDarkPhotons = numberOfDarkPhotons
        self.__event = None
        self.__dark_photon_child_index_1 = None
        self.__dark_photon_child_index_2 = None
        self.__dark_photon_index         = None
        self.__scalar_index              = None
        histControlName = histControlName.replace('.root', '')
        self.__histControl = HistControl.HistControl( histControlName, doOutputFile=True )

        self.__doPolarizationPlots     = True
        self.__doPtBalancePlots        = True
        self.__doPtEtaPhiPlots         = True
        self.__doOpeningAnglesPlots    = True
        self.__doRZPlots               = True

        maxPt = 100.e3 

        if self.__doPolarizationPlots:
            self.__histControl.BookHist1D('Polarization', 'cos(#theta)', 200, -1.1, 1.1)
            pass
        
        if self.__doOpeningAnglesPlots:
            self.__histControl.BookHist1D('OpeningEtaElectrons', '#Delta #eta', 200, .0, .1)
            self.__histControl.BookHist1D('OpeningPhiElectrons', '#Delta #phi', 200, .0, .1)
            self.__histControl.BookHist1D('OpeningRElectrons',   '#Delta R',    200, .0, .1)
            
            self.__histControl.BookHist1D('OpeningEtaMuons', '#Delta #eta', 200, .0, .1)
            self.__histControl.BookHist1D('OpeningPhiMuons', '#Delta #phi', 200, .0, .1)
            self.__histControl.BookHist1D('OpeningRMuons',   '#Delta R',    200, .0, .1)

            self.__histControl.BookHist1D('OpeningEtaPions', '#Delta #eta', 200, .0, .1)
            self.__histControl.BookHist1D('OpeningPhiPions', '#Delta #phi', 200, .0, .1)
            self.__histControl.BookHist1D('OpeningRPions',   '#Delta R',    200, .0, .1)

            self.__histControl.BookHist1D('OpeningEtaDarkPhotons', '#Delta #eta', 200, .0, 4.)
            self.__histControl.BookHist1D('OpeningPhiDarkPhotons', '#Delta #phi', 200, .0, 4.)
            self.__histControl.BookHist1D('OpeningRDarkPhotons',   '#Delta R',    200, .0, 4.)
            pass
        
        if self.__doPtBalancePlots:
            self.__histControl.BookHist1D('leadingPtElectron',    'Leading p_{T};p_{T} [GeV];Entries',     200, .0, maxPt)
            self.__histControl.BookHist1D('subLeadingPtElectron', 'Sub-Leading p_{T};p_{T} [GeV];Entries', 200, .0, maxPt/2.)
            self.__histControl.BookHist1D('ptBalanceElectron',    'p_{T} Balance',                         200, -.1, 1.1)
            
            self.__histControl.BookHist1D('leadingPtMuon',        'Leading p_{T};p_{T} [GeV];Entries',         200, .0, maxPt)
            self.__histControl.BookHist1D('subLeadingPtMuon',     'Sub-Leading p_{T};p_{T} [GeV];Entries',     200, .0, maxPt/2.)
            self.__histControl.BookHist1D('ptBalanceMuon',        'p_{T} Balance',                             200, -.1, 1.1)
            
            self.__histControl.BookHist1D('leadingPtPion',        'Leading p_{T};p_{T} [GeV];Entries',         200, .0, maxPt)
            self.__histControl.BookHist1D('subLeadingPtPion',     'Sub-Leading p_{T};p_{T} [GeV];Entries',     200, .0, maxPt/2.)
            self.__histControl.BookHist1D('ptBalancePion',        'p_{T} Balance',                             200, -.1, 1.1)

            self.__histControl.BookHist1D('leadingPtDarkPhoton',        'Leading p_{T};p_{T} [GeV];Entries',         200, .0, maxPt)
            self.__histControl.BookHist1D('subLeadingPtDarkPhoton',     'Sub-Leading p_{T};p_{T} [GeV];Entries',     200, .0, maxPt/2.)
            self.__histControl.BookHist1D('ptBalanceDarkPhoton',        'p_{T} Balance',                             200, -.1, 1.1)
            pass
        
        if self.__doRZPlots:
            self.__histControl.BookHist1D('decayR1D',    'Dark Photon Decay Radius;R [mm];Entries',         200, 1., -1.)
            self.__histControl.BookHist1D('decayZ1D',    'Dark Photon Decay Length along Z;Z [mm];Entries', 200, 1., -1.)
            self.__histControl.BookHist2D('decayZR2D',   'Dark Photon Decay Distance;Z [mm];R [mm]',        150, 1., -1., 150, 1., -1.)
            self.__histControl.BookHist2D('decayEtaR2D', 'Dark Photon Decay Distance;#eta;R [mm]',          150, 1., -1., 150, 1., -1.)
            pass
        
        if self.__doPtEtaPhiPlots:
            self.__histControl.BookHist1D('darkPhotonEta', 'Dark Photon #eta',  200, 1., -1.)
            self.__histControl.BookHist1D('darkPhotonPhi', 'Dark Photon #phi',  200, 1., -1.)
            self.__histControl.BookHist1D('darkPhotonPt',  'Dark Photon p_{T}', 200, 1., -1.)
            self.__histControl.BookHist1D('scalarEta', 'Scalar #eta',  200, 1., -1.)
            self.__histControl.BookHist1D('scalarPhi', 'Scalar #phi',  200, 1., -1.)
            self.__histControl.BookHist1D('scalarPt',  'Scalar p_{T}', 200, 1., -1.)
            pass
        return

    def deltaPhi(self, phi1, phi2):
        dphi = phi1 - phi2
        while dphi < -ROOT.TMath.Pi():
            dphi = dphi + 2. * ROOT.TMath.Pi()
        while dphi > ROOT.TMath.Pi():
            dphi = dphi - 2. * ROOT.TMath.Pi()
        return dphi;

    def deltaR(self, eta1, phi1, eta2, phi2):
        dphi = self.deltaPhi(phi1, phi2)
        deta = ROOT.TMath.Abs(eta1-eta2)
        dr   = ROOT.TMath.Sqrt(dphi*dphi + deta*deta)
        return dr


    def finalize(self):
        self.__histControl.SaveAllToFile()

    def setParticleIndices(self):
        if self.__nDarkPhotons == 1:
            self.__dark_photon_child_index_1 = -999
            self.__dark_photon_child_index_2 = -999
            self.__dark_photon_index         = -999
            pass
        elif self.__nDarkPhotons == 2:
            self.__dark_photon_child_index_1 = []
            self.__dark_photon_child_index_2 = []
            self.__dark_photon_index         = []
            self.__scalar_index = -999
            pass
        else:
            print 'Cannot have', self.__nDarkPhotons, 'per event. Set to 1 or 2'
            exit(0)
            pass

        for index,(mc_child_index,pdgId) in enumerate( zip( self.__event.mc_child_index, self.__event.mc_pdgId ) ):
            if pdgId==700022:
                if self.__nDarkPhotons == 1:
                    self.__dark_photon_index = index
                else:
                    self.__dark_photon_index.append(index)
                    pass
                if len(mc_child_index) != 2:
                    print 'dark photon has ', len(mc_child_index), 'children instead of 2'
                    return False
                if self.__nDarkPhotons == 1:
                    self.__dark_photon_child_index_1 = mc_child_index[0]
                    self.__dark_photon_child_index_2 = mc_child_index[1]
                else:
                    self.__dark_photon_child_index_1.append(mc_child_index[0])
                    self.__dark_photon_child_index_2.append(mc_child_index[1])
                    pass
                pass
            if pdgId==700021:
                if self.__nDarkPhotons == 1:
                    print 'Found a scalar (pdgId==700021). But number of dark photons is set to 1. fix this'
                    exit(0)
                    pass
                self.__scalar_index = index
                pass
            if self.__scalar_index == -999 and self.__nDarkPhotons==2:
                print 'Didn\'t find a scalar (pdgId==700021). But number of dark photons is set to 2. fix this'
                exit(0)
                pass
        return True

    def processEvent(self, event):

        self.__event = event

        if not self.setParticleIndices():
            return False
        
        if self.__doPolarizationPlots:
            self.cosTheta()
            pass
        
        if self.__doOpeningAnglesPlots:
            self.openingAngles()
            pass

        if self.__doPtBalancePlots:
            self.ptBalance()
            pass

        if self.__doRZPlots:
            self.RZ()
            pass

        if self.__doPtEtaPhiPlots:
            self.etaPhi()
            pass

        return True

    def fillPolarization(self, indexDP, indexKid):
        '''Get cos(theta) between the dark photon vector in the lab frame
        and the decay product vector in the dark photon\'s rest frame.
        '''
        eta = self.__event.mc_eta[indexDP]
        phi = self.__event.mc_phi[indexDP]
        pt  = self.__event.mc_pt[indexDP]
        
        theta = 2.*ROOT.TMath.ATan(ROOT.TMath.Exp(-1.*eta))
        px  = pt*ROOT.TMath.Sin(phi)
        py  = pt*ROOT.TMath.Cos(phi)
        pz  = pt/ROOT.TMath.Tan(theta)
        edp = ROOT.TMath.Sqrt( px**2+py**2+pz**2+self.__event.mc_m[indexDP]**2 )
        
        eta_1 = self.__event.mc_eta[indexKid]
        phi_1 = self.__event.mc_phi[indexKid]
        pt_1  = self.__event.mc_pt[indexKid]
        theta_1 = 2.*ROOT.TMath.ATan(ROOT.TMath.Exp(-1.*eta_1))
        
        px_kid_1 = pt_1*ROOT.TMath.Sin(phi_1)
        py_kid_1 = pt_1*ROOT.TMath.Cos(phi_1)
        pz_kid_1 = pt_1/ROOT.TMath.Tan(theta_1)
        e_kid_1  = ROOT.TMath.Sqrt( px_kid_1**2+py_kid_1**2+pz_kid_1**2+self.__event.mc_m[indexKid]**2 )
        
        tlv_dp = ROOT.TLorentzVector(px, py, pz, edp)
        bv = tlv_dp.BoostVector()
        
        tlv_kid_1 = ROOT.TLorentzVector(px_kid_1, py_kid_1, pz_kid_1, e_kid_1)
        
        tlv_kid_1.Boost(-bv)
        
        self.__histControl.FillHist1D('Polarization', tlv_dp.Vect().Unit().Dot(tlv_kid_1.Vect().Unit()) )
        return

    def cosTheta(self):
        if self.__nDarkPhotons == 1:
            self.fillPolarization(self.__dark_photon_index, self.__dark_photon_child_index_1)
            pass
        else:
            for (indexDP,indexKid) in zip (self.__dark_photon_index,self.__dark_photon_child_index_1):
                self.fillPolarization(indexDP, indexKid)
                continue
                pass
            pass
        return

    def fillOpeningAngles(self,index1, index2):
        eta_1 = self.__event.mc_eta[index1]
        phi_1 = self.__event.mc_phi[index1]
        pt_1  = self.__event.mc_pt[index1]
        theta_1 = 2.*ROOT.TMath.ATan(ROOT.TMath.Exp(-1.*eta_1))
        
        px_kid_1 = pt_1*ROOT.TMath.Sin(phi_1)
        py_kid_1 = pt_1*ROOT.TMath.Cos(phi_1)
        pz_kid_1 = pt_1/ROOT.TMath.Tan(theta_1)
        e_kid_1  = ROOT.TMath.Sqrt( px_kid_1**2+py_kid_1**2+pz_kid_1**2+self.__event.mc_m[index1]**2 )

        eta_2 = self.__event.mc_eta[index2]
        phi_2 = self.__event.mc_phi[index2]
        pt_2  = self.__event.mc_pt[index2]
        theta_2 = 2.*ROOT.TMath.ATan(ROOT.TMath.Exp(-1.*eta_2))
        
        px_kid_2 = pt_2*ROOT.TMath.Sin(phi_2)
        py_kid_2 = pt_2*ROOT.TMath.Cos(phi_2)
        pz_kid_2 = pt_2/ROOT.TMath.Tan(theta_2)
        e_kid_2  = ROOT.TMath.Sqrt( px_kid_2**2+py_kid_2**2+pz_kid_2**2+self.__event.mc_m[index2]**2 )

        tlv_kid_1 = ROOT.TLorentzVector(px_kid_1, py_kid_1, pz_kid_1, e_kid_1)
        tlv_kid_2 = ROOT.TLorentzVector(px_kid_2, py_kid_2, pz_kid_2, e_kid_2)

        dPhi = abs( self.deltaPhi( tlv_kid_1.Phi(), tlv_kid_2.Phi() ) )
        dEta = abs( tlv_kid_1.Eta() - tlv_kid_2.Eta() )
        dR   = self.deltaR( tlv_kid_1.Eta(), tlv_kid_1.Phi(), tlv_kid_2.Eta(), tlv_kid_2.Phi() )

        pdgId = abs(self.__event.mc_pdgId[index1])

        if pdgId == 11:
            self.__histControl.FillHist1D('OpeningEtaElectrons', dEta )
            self.__histControl.FillHist1D('OpeningPhiElectrons', dPhi )
            self.__histControl.FillHist1D('OpeningRElectrons',   dR   )
            pass
        if pdgId == 13:
            self.__histControl.FillHist1D('OpeningEtaMuons', dEta )
            self.__histControl.FillHist1D('OpeningPhiMuons', dPhi )
            self.__histControl.FillHist1D('OpeningRMuons',   dR   )
            pass
        if pdgId == 211:
            self.__histControl.FillHist1D('OpeningEtaPions', dEta )
            self.__histControl.FillHist1D('OpeningPhiPions', dPhi )
            self.__histControl.FillHist1D('OpeningRPions',   dR   )
            pass
        if pdgId == 700022:
            self.__histControl.FillHist1D('OpeningEtaDarkPhotons', dEta )
            self.__histControl.FillHist1D('OpeningPhiDarkPhotons', dPhi )
            self.__histControl.FillHist1D('OpeningRDarkPhotons',   dR   )
            pass

    def openingAngles(self):
        if self.__nDarkPhotons == 1:
            self.fillOpeningAngles(self.__dark_photon_child_index_1, self.__dark_photon_child_index_2)
            pass
        else:
            for (index1,index2) in zip (self.__dark_photon_child_index_1,self.__dark_photon_child_index_2):
                self.fillOpeningAngles(index1, index2)
                pass
            self.fillOpeningAngles(self.__dark_photon_index[0], self.__dark_photon_index[1])
            pass

        return

    def fillPtBalance(self, index1, index2):
        pt_1  = self.__event.mc_pt[index1]
        pt_2  = self.__event.mc_pt[index2]
        pdgId = abs(self.__event.mc_pdgId[index1])
        if pdgId == 11:
            self.__histControl.FillHist1D('leadingPtElectron',    max(pt_1, pt_2) )
            self.__histControl.FillHist1D('subLeadingPtElectron', min(pt_1, pt_2) )
            self.__histControl.FillHist1D('ptBalanceElectron',    abs(pt_1 - pt_2)/(pt_1+pt_2) )
            pass
        if pdgId == 13:
            self.__histControl.FillHist1D('leadingPtMuon',        max(pt_1, pt_2) )
            self.__histControl.FillHist1D('subLeadingPtMuon',     min(pt_1, pt_2) )
            self.__histControl.FillHist1D('ptBalanceMuon',        abs(pt_1 - pt_2)/(pt_1+pt_2) )
            pass
        if pdgId == 211:
            self.__histControl.FillHist1D('leadingPtPion',        max(pt_1, pt_2) )
            self.__histControl.FillHist1D('subLeadingPtPion',     min(pt_1, pt_2) )
            self.__histControl.FillHist1D('ptBalancePion',        abs(pt_1 - pt_2)/(pt_1+pt_2) )
            pass
        if pdgId == 700022:
            self.__histControl.FillHist1D('leadingPtDarkPhoton',        max(pt_1, pt_2) )
            self.__histControl.FillHist1D('subLeadingPtDarkPhoton',     min(pt_1, pt_2) )
            self.__histControl.FillHist1D('ptBalanceDarkPhoton',        abs(pt_1 - pt_2)/(pt_1+pt_2) )
            pass
        return
    
    def ptBalance(self):
        if self.__nDarkPhotons == 1:
            self.fillPtBalance(self.__dark_photon_child_index_1, self.__dark_photon_child_index_2)
            pass
        else:
            for (index1,index2) in zip (self.__dark_photon_child_index_1,self.__dark_photon_child_index_2):
                self.fillPtBalance(index1, index2)
                pass
            self.fillPtBalance(self.__dark_photon_index[0], self.__dark_photon_index[1])
            pass
        return

    def fillRZ(self, indexDP, indexKid):
        decayR = ROOT.TMath.Sqrt( self.__event.mc_vx_x[indexKid]**2 + self.__event.mc_vx_y[indexKid]**2 )
        decayZ = self.__event.mc_vx_z[indexKid]
        self.__histControl.FillHist1D('decayR1D',    decayR)
        self.__histControl.FillHist1D('decayZ1D',    decayZ)
        self.__histControl.FillHist2D('decayZR2D',   decayZ, decayR)
        self.__histControl.FillHist2D('decayEtaR2D', self.__event.mc_eta[indexDP],  decayR)
        return
    
    def RZ(self):
        if self.__nDarkPhotons == 1:
            self.fillRZ(self.__dark_photon_index, self.__dark_photon_child_index_1)
            pass
        else:
            for (indexDP,indexKid) in zip (self.__dark_photon_index,self.__dark_photon_child_index_1):
                self.fillRZ(indexDP, indexKid)
                pass
            pass
        return

    def etaPhi(self):
        if self.__nDarkPhotons == 1:
            self.__histControl.FillHist1D('darkPhotonEta', self.__event.mc_eta[self.__dark_photon_index])
            self.__histControl.FillHist1D('darkPhotonPhi', self.__event.mc_phi[self.__dark_photon_index])
            self.__histControl.FillHist1D('darkPhotonPt',  self.__event.mc_pt[self.__dark_photon_index] )
        else:
            for index in self.__dark_photon_index:
                self.__histControl.FillHist1D('darkPhotonEta', self.__event.mc_eta[index])
                self.__histControl.FillHist1D('darkPhotonPhi', self.__event.mc_phi[index])
                self.__histControl.FillHist1D('darkPhotonPt',  self.__event.mc_pt[index] )
                pass
            self.__histControl.FillHist1D('scalarEta', self.__event.mc_eta[self.__scalar_index])
            self.__histControl.FillHist1D('scalarPhi', self.__event.mc_phi[self.__scalar_index])
            self.__histControl.FillHist1D('scalarPt',  self.__event.mc_pt[self.__scalar_index] )
            pass
        return
