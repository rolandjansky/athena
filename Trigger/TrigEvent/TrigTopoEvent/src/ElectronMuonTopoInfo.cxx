/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigEvent/TrigTopoEvent/ElectronMuonTopoInfo.cxx
 **
 **   Description: -Class for description of combined electron-muon object for 
 ** 		algorithms in TrigEgammaMuonCombHypo package
 **
 **   Author: Pavel Jez <pavel.jez@cern.ch>
 **
 **   Created:   Apr 9, 2011
 **
 **
 **************************************************************************/ 

#include "TrigTopoEvent/ElectronMuonTopoInfo.h"
#include <sstream>
#include <iostream>
#ifdef __APPLE__
#include "CxxUtils/sincos.h"
#endif
#include "EventPrimitives/EventPrimitivesHelpers.h"
// constructors

ElectronMuonTopoInfo::ElectronMuonTopoInfo():   m_roiWord(-1),
						m_DeltaPhi(-1),
						m_DeltaR(-1),
						m_InvMass(-1),
						m_electronValid(false),
						m_oppositeCharge(false),
						m_vertexState(0)
  
{}

ElectronMuonTopoInfo::ElectronMuonTopoInfo(int roiWord, float deltaPhi, float deltaR, float invMass, bool el_valid, 
					   bool oppositeCharge, unsigned short vertexState): m_roiWord(roiWord), 
											     m_DeltaPhi(deltaPhi),
											     m_DeltaR(deltaR),
											     m_InvMass(invMass),
											     m_electronValid(el_valid),
											     m_oppositeCharge(oppositeCharge),
											     m_vertexState(vertexState)
  
{}

ElectronMuonTopoInfo::ElectronMuonTopoInfo(const ElectronMuonTopoInfo& emuTopoInfo)
{
  if(this != &emuTopoInfo){
    m_roiWord   = emuTopoInfo.m_roiWord;
    m_DeltaPhi 	= emuTopoInfo.m_DeltaPhi;
    m_DeltaR	= emuTopoInfo.m_DeltaR;
    m_InvMass	= emuTopoInfo.m_InvMass;
    m_electronValid = emuTopoInfo.m_electronValid;
    m_oppositeCharge = emuTopoInfo.m_oppositeCharge;
    m_vertexState = emuTopoInfo.m_vertexState;
  }
}

// destructor    
    ElectronMuonTopoInfo::~ElectronMuonTopoInfo() {}
    

// set methods 
  void 	ElectronMuonTopoInfo::SetRoiWord(int RoiWord){m_roiWord = RoiWord;}
  void 	ElectronMuonTopoInfo::SetDeltaPhi(float DeltaPhi){m_DeltaPhi= DeltaPhi;}
  void 	ElectronMuonTopoInfo::SetDeltaR(float DeltaR){m_DeltaR= DeltaR;}
  void 	ElectronMuonTopoInfo::SetInvMass(float InvMass){m_InvMass = InvMass;}
  void  ElectronMuonTopoInfo::SetElecValid(bool ElecValid){m_electronValid = ElecValid;}
  void  ElectronMuonTopoInfo::SetOppositeCharge(bool OppositeCharge){m_oppositeCharge = OppositeCharge;} 
  void 	ElectronMuonTopoInfo::SetVertexState(unsigned short vertexState){m_vertexState = vertexState;}

// text output  
   std::string str( const ElectronMuonTopoInfo& d )
   {
		std::stringstream ss;
		ss 	<< "ElectronMuonTopoInfo at address: " << &d
			<< " RoIWord: " 	<< d.RoiWord()
			<< " Delta Phi: " 	<< d.DeltaPhi()
			<< " Delta R: "		<< d.DeltaR()
			<< " Invariant mass: " 	<< d.InvMass()
			<< " electron valid: " 	<< d.ElecValid()
			<< " opposite charge: " << d.OppositeCharge()
			<< " vertex state: " 	<< d.VertexState();
		return ss.str();
	}
	  
	MsgStream& operator<< ( MsgStream& m, const ElectronMuonTopoInfo& d )
	{
		return( m << str ( d ) );
	}
	
	bool operator==( const ElectronMuonTopoInfo& d1,  const ElectronMuonTopoInfo& d2 )
	{
		return(d1.DeltaPhi()==d2.DeltaPhi() && d1.DeltaR()==d2.DeltaR() && d1.InvMass()==d2.InvMass() 
		&& d1.OppositeCharge()==d2.OppositeCharge() && d1.VertexState() == d2.VertexState() );
	}
 // helper functions, overloaded separately for L2 and EF interface
 
 // Vextex matching @ L2
 ElectronMuonTopoInfo::Vertex ElectronMuonTopoInfo::commonVertex(const TrigElectron* electron1, 
									      const CombinedMuonFeature* muon1) {
  
	    
  double trk_e1_z0error = electron1->err_Zvtx();
  double trk_e2_z0error = muon1->IDTrack()->param()->ez0();
  	
  
  if ( trk_e1_z0error > 0 && trk_e2_z0error > 0 ) {
    double z0_distance = fabs(electron1->Zvtx() 
			      - muon1->IDTrack()->param()->z0());
    
    double z0_error = sqrt(trk_e1_z0error * trk_e1_z0error 
			   + trk_e2_z0error * trk_e2_z0error);
			   		   
    
    // reject combination if distance squared between perigee of 
    // tracks is greater than sqrt(6)*error = 2.45*error: this 
    // should accept 99% of good combinations (common vertex) 
    // Note: I changed 2.45 to 3 to be conservative => efficiency is
    // now 99.7%
    if (z0_distance > 3*z0_error) {
      return NotCommon;
        
    } else {
   
      return Common;
    }
  }
    
  return Unapplicable;
}

// Vertex matching @ EF

ElectronMuonTopoInfo::Vertex ElectronMuonTopoInfo::commonVertex(const Trk::Perigee* perigeeEL, 
								const Trk::Perigee* perigeeMU, 
								double& pull, bool debug) {
  
  if(debug) std::cout << "Now checking electron perigee at " << perigeeEL << " and muon perigee at " << perigeeMU << "." << std::endl;
  
  
	double electron_z0=0.;	
   double electron_z0_error=0.;	
   
   double muon_z0=0.;	
   double muon_z0_error=0.;	
   
	if(perigeeEL!=0)
	{
	  electron_z0 = perigeeEL->parameters()[Trk::z0];
	  electron_z0_error= Amg::error( *perigeeEL->covariance(), Trk::z0 );
	  //electron_z0_error= perigeeEL->localErrorMatrix().error(Trk::z0);
 	    delete perigeeEL;
	}
	
	if(perigeeMU!=0)
	{



 	    muon_z0 = perigeeMU->parameters()[Trk::z0];
	    muon_z0_error= Amg::error( *perigeeMU->covariance(), Trk::z0 );

	  //muon_z0_error= perigeeMU->localErrorMatrix().error(Trk::z0);
 	    delete perigeeMU;
	}
  
  if(debug) std::cout << "Electron z0 = " << electron_z0 << "+-" << electron_z0_error << "; muon_z0 = " << muon_z0 << "+-" << muon_z0_error << std::endl;	
  
  pull = -999.;
  
  if ( electron_z0_error > 0 && muon_z0_error > 0 ) {
    double z0_distance = fabs(electron_z0 
			      - muon_z0);
    
    double z0_error = sqrt(electron_z0_error * electron_z0_error 
			   + muon_z0_error * muon_z0_error);
    
	pull = z0_distance/z0_error;		   		   
    
    if(debug) std::cout << "z-distance is " << z0_distance << "+-" << z0_error << ". And the pull is " << pull << "." << std::endl;
    
    // reject combination if distance squared between perigee of 
    // tracks is greater than sqrt(6)*error = 2.45*error: this 
    // should accept 99% of good combinations (common vertex) 
    // Note: I changed 2.45 to 3 to be conservative => efficiency is
    // now 99.7%
    if (z0_distance > 3*z0_error) {
      return ElectronMuonTopoInfo::NotCommon;
        
    } else {
   
      return ElectronMuonTopoInfo::Common;
    }
  }
    
  return ElectronMuonTopoInfo::Unapplicable;
}


//L2
bool ElectronMuonTopoInfo::opositeCharge(const TrigElectron* electron1, 
					 const CombinedMuonFeature* muon1) {
  if ( electron1->charge() * muon1->ptq() > 0 )
    return false;
  return true;
}

//EF
bool ElectronMuonTopoInfo::opositeCharge(const egamma* electron1, const Trk::Perigee* muon1){
  if( (electron1->charge() * muon1->charge()) > 0 )
    return false;
  return true; 	
}

//L2
double ElectronMuonTopoInfo::deltaPhi(const TrigElectron* electron1, 
				      const CombinedMuonFeature* muon1) {
  double dPhi = electron1->phi()-muon1->IDTrack()->param()->phi0();
  if (dPhi < -M_PI) dPhi += 2*M_PI;
  if (dPhi > M_PI) dPhi -= 2*M_PI;
  double distEmu = fabs(dPhi);
  
  return distEmu;
}

//L2 tau tau 
double ElectronMuonTopoInfo::deltaPhi(const TrigTau* tau1, 
				      const TrigTau* tau2) {
  double dPhi = tau1->phi()-tau2->phi();
  if (dPhi < -M_PI) dPhi += 2*M_PI;
  if (dPhi > M_PI) dPhi -= 2*M_PI;
  double distEmu = fabs(dPhi);
  
  return distEmu;
}

//L2 tau e 
double ElectronMuonTopoInfo::deltaPhi(const TrigTau* tau1, 
				      const TrigElectron* electron1) {
  double dPhi = tau1->phi()-electron1->phi();
  if (dPhi < -M_PI) dPhi += 2*M_PI;
  if (dPhi > M_PI) dPhi -= 2*M_PI;
  double distEmu = fabs(dPhi);
  
  return distEmu;
}

//L2 tau mu
double ElectronMuonTopoInfo::deltaPhi(const TrigTau* tau1, 
				      const CombinedMuonFeature* muon1) {
  double dPhi = tau1->phi()-muon1->IDTrack()->param()->phi0();
  if (dPhi < -M_PI) dPhi += 2*M_PI;
  if (dPhi > M_PI) dPhi -= 2*M_PI;
  double distEmu = fabs(dPhi);
  
  return distEmu;
}




//L2
double ElectronMuonTopoInfo::deltaR(const TrigElectron* electron1, 
				    const CombinedMuonFeature* muon1) {
  double dPhi = electron1->phi()-muon1->IDTrack()->param()->phi0();
  if (dPhi < -M_PI) dPhi += 2*M_PI;
  if (dPhi > M_PI) dPhi -= 2*M_PI;
  double dEta = electron1->eta() - muon1->IDTrack()->param()->eta();
  double distEmu = sqrt(dPhi*dPhi+dEta*dEta);
  
  return distEmu;
}

//L2 tautau
double ElectronMuonTopoInfo::deltaR( const TrigTau* tau1, 
				     const TrigTau* tau2) {
  double dPhi = tau1->phi()-tau2->phi();
  if (dPhi < -M_PI) dPhi += 2*M_PI;
  if (dPhi > M_PI) dPhi -= 2*M_PI;
  double dEta = tau1->eta() - tau2->eta();
  double distEmu = sqrt(dPhi*dPhi+dEta*dEta);
  
  return distEmu;
}

//L2 tau e
double ElectronMuonTopoInfo::deltaR( const TrigTau* tau1, 
				     const TrigElectron* electron1) {
  double dPhi = tau1->phi()-electron1->phi();
  if (dPhi < -M_PI) dPhi += 2*M_PI;
  if (dPhi > M_PI) dPhi -= 2*M_PI;
  double dEta = tau1->eta() - electron1->eta();
  double distEmu = sqrt(dPhi*dPhi+dEta*dEta);
  
  return distEmu;
}

//L2 tau mu
double ElectronMuonTopoInfo::deltaR( const TrigTau* tau1, 
				     const CombinedMuonFeature* muon1) {
  double dPhi = tau1->phi()-muon1->IDTrack()->param()->phi0();
  if (dPhi < -M_PI) dPhi += 2*M_PI;
  if (dPhi > M_PI) dPhi -= 2*M_PI;
  double dEta = tau1->eta() - muon1->IDTrack()->param()->eta();
  double distEmu = sqrt(dPhi*dPhi+dEta*dEta);
  
  return distEmu;
}

//EF
double ElectronMuonTopoInfo::deltaPhi(const egamma* electron1, 
				      const Trk::Perigee* muon1) {
  float mu_phi = muon1->parameters()[Trk::phi];

  double dPhi = electron1->trackParticle()->phi()-mu_phi;
  if (dPhi < -M_PI) dPhi += 2*M_PI;
  if (dPhi > M_PI) dPhi -= 2*M_PI;
  double distEmu = fabs(dPhi);
  
  return distEmu;
}
//EF tau tau 
double ElectronMuonTopoInfo::deltaPhi(const Analysis::TauJet* tau1, 
				      const Analysis::TauJet* tau2) {
  double dPhi = tau1->phi()-tau2->phi();
  if (dPhi < -M_PI) dPhi += 2*M_PI;
  if (dPhi > M_PI) dPhi -= 2*M_PI;
  double distEmu = fabs(dPhi);
  
  return distEmu;
}

//EF tau e 
double ElectronMuonTopoInfo::deltaPhi(const Analysis::TauJet* tau1, 
				      const egamma* electron1) {
  double dPhi = tau1->phi()-electron1->trackParticle()->phi();
  if (dPhi < -M_PI) dPhi += 2*M_PI;
  if (dPhi > M_PI) dPhi -= 2*M_PI;
  double distEmu = fabs(dPhi);
  
  return distEmu;
}

//EF tau mu
double ElectronMuonTopoInfo::deltaPhi(const Analysis::TauJet* tau1, 
				      const Trk::Perigee* muon1) {
  float mu_phi = muon1->parameters()[Trk::phi];

  double dPhi = tau1->phi()-mu_phi;
  if (dPhi < -M_PI) dPhi += 2*M_PI;
  if (dPhi > M_PI) dPhi -= 2*M_PI;
  double distEmu = fabs(dPhi);
  
  return distEmu;
}


//EF
double ElectronMuonTopoInfo::deltaR(const egamma* electron1, 
				    const Trk::Perigee* muon1) {
  float mu_phi =muon1->parameters()[Trk::phi];

  double dPhi = electron1->trackParticle()->phi()-mu_phi;
  if (dPhi < -M_PI) dPhi += 2*M_PI;
  if (dPhi > M_PI) dPhi -= 2*M_PI;
  double dEta = electron1->trackParticle()->eta() - muon1->eta();
  double distEmu = sqrt(dPhi*dPhi+dEta*dEta);
  
  return distEmu;
}

//EF tautau
double ElectronMuonTopoInfo::deltaR( const Analysis::TauJet* tau1, 
				     const Analysis::TauJet* tau2) {
  double dPhi = tau1->phi()-tau2->phi();
  if (dPhi < -M_PI) dPhi += 2*M_PI;
  if (dPhi > M_PI) dPhi -= 2*M_PI;
  double dEta = tau1->eta() - tau2->eta();
  double distEmu = sqrt(dPhi*dPhi+dEta*dEta);
  
  return distEmu;
}

//EF tau e
double ElectronMuonTopoInfo::deltaR( const Analysis::TauJet* tau1, 
				     const egamma* electron1) {
  double dPhi = tau1->phi()-electron1->trackParticle()->phi();
  if (dPhi < -M_PI) dPhi += 2*M_PI;
  if (dPhi > M_PI) dPhi -= 2*M_PI;
  double dEta = tau1->eta() - electron1->trackParticle()->eta();
  double distEmu = sqrt(dPhi*dPhi+dEta*dEta);
  
  return distEmu;
}

//EF tau mu
double ElectronMuonTopoInfo::deltaR( const Analysis::TauJet* tau1, 
				     const Trk::Perigee* muon1) {
  float mu_phi =muon1->parameters()[Trk::phi];

  double dPhi = tau1->phi()-mu_phi;
  if (dPhi < -M_PI) dPhi += 2*M_PI;
  if (dPhi > M_PI) dPhi -= 2*M_PI;
  double dEta = tau1->eta() - muon1->eta();
  double distEmu = sqrt(dPhi*dPhi+dEta*dEta);
  
  return distEmu;
}


// L2
double ElectronMuonTopoInfo::invariantMass(const TrigElectron* electron1, 
					   const CombinedMuonFeature* muon1) {
  // get parameters: not electron pT no longer signed
  double eta1   = electron1->eta();
  double eta2   = muon1->IDTrack()->param()->eta();

  
  double Pt1    = electron1->pt() ; // IMPORTANT: pt() is the 4-momentum base class method and 
  double Pt2    = muon1->pt(); // it returns the cluster pT; the track pT is given by Pt()
  
  double phi1   = electron1->phi();
  double phi2   = muon1->IDTrack()->param()->phi0();

  return invariantMass(Pt1, eta1, phi1, 0.511, Pt2, eta2, phi2, 105.6);
}

// L2 tautau
double ElectronMuonTopoInfo::invariantMass( const TrigTau* tau1, 
					    const TrigTau* tau2) {
  // get parameters: not electron pT no longer signed
  double eta1   = tau1->eta();
  double eta2   = tau2->eta();
  
  
  double Pt1    = tau1->pt() ; // IMPORTANT: pt() is the 4-momentum base class method and 
  double Pt2    = tau2->pt();  // it returns the cluster pT; the track pT is given by Pt()
  
  double phi1   = tau1->phi();
  double phi2   = tau2->phi();

  return invariantMass(Pt1, eta1, phi1, 1777., Pt2, eta2, phi2, 1777.);//tau mass used
}

// L2 tau e 
double ElectronMuonTopoInfo::invariantMass( const TrigTau* tau1, 
					    const TrigElectron* electron1) {
  // get parameters: not electron pT no longer signed
  double eta1   = tau1->eta();
  double eta2   = electron1->eta();
  
  
  double Pt1    = tau1->pt() ; // IMPORTANT: pt() is the 4-momentum base class method and 
  double Pt2    = electron1->pt();  // it returns the cluster pT; the track pT is given by Pt()
  
  double phi1   = tau1->phi();
  double phi2   = electron1->phi();

  return invariantMass(Pt1, eta1, phi1, 1777., Pt2, eta2, phi2, 0.511);//tau mass used
}

// L2 tau mu 
double ElectronMuonTopoInfo::invariantMass( const TrigTau* tau1, 
					    const CombinedMuonFeature* muon1) {
  // get parameters: not electron pT no longer signed
  double eta1   = tau1->eta();
  double eta2   = muon1->IDTrack()->param()->eta();

  
  double Pt1    = tau1->pt() ; // IMPORTANT: pt() is the 4-momentum base class method and 
  double Pt2    = muon1->pt();  // it returns the cluster pT; the track pT is given by Pt()
  
  double phi1   = tau1->phi();
  double phi2   = muon1->IDTrack()->param()->phi0();

  return invariantMass(Pt1, eta1, phi1, 1777., Pt2, eta2, phi2, 105.6);//tau mass used
}

// EF tautau
double ElectronMuonTopoInfo::invariantMass( const Analysis::TauJet* tau1, 
					    const Analysis::TauJet* tau2) {
  // get parameters: not electron pT no longer signed
  double eta1   = tau1->eta();
  double eta2   = tau2->eta();
  
  
  double Pt1    = tau1->pt() ; // IMPORTANT: pt() is the 4-momentum base class method and 
  double Pt2    = tau2->pt();  // it returns the cluster pT; the track pT is given by Pt()
  
  double phi1   = tau1->phi();
  double phi2   = tau2->phi();

  return invariantMass(Pt1, eta1, phi1, 1777., Pt2, eta2, phi2, 1777.);//tau mass used
}


// EF tautau
double ElectronMuonTopoInfo::invariantMass( const Analysis::TauJet* tau1, 
					    const egamma* electron1) {
  // get parameters: not electron pT no longer signed
  double eta1   = tau1->eta();
  double eta2   = electron1->trackParticle()->eta();

  
  double Pt1    = tau1->pt() ; // IMPORTANT: pt() is the 4-momentum base class method and 
  double Pt2    = electron1->cluster()->e()/cosh(electron1->trackParticle()->eta()) ; //

  double phi1   = tau1->phi();
  double phi2   = electron1->trackParticle()->phi();

  return invariantMass(Pt1, eta1, phi1, 1777., Pt2, eta2, phi2, 0.511);//tau mass used
}
// EF tautau
double ElectronMuonTopoInfo::invariantMass( const Analysis::TauJet* tau1, 
					    const Trk::Perigee* muon1) {
  // get parameters: not electron pT no longer signed
  double eta1   = tau1->eta();
  double eta2   = muon1->eta();

  
  double Pt1    = tau1->pt() ; // IMPORTANT: pt() is the 4-momentum base class method and 
  double Pt2    = muon1->pT() ; //

  double phi1   = tau1->phi();
  double phi2 = muon1->parameters()[Trk::phi];

  return invariantMass(Pt1, eta1, phi1, 1777., Pt2, eta2, phi2, 105.6);//tau mass used
}

double ElectronMuonTopoInfo::invariantMass(double Pt1, double  eta1, double phi1, double m1,
					   double Pt2, double  eta2, double phi2,double m2 ) const {
  // protection in case upstream algorithm suplies negative pt

  Pt1 = fabs(Pt1);
  Pt2 = fabs(Pt2);
  
  double theta1 = 2*atan2((double)exp(-eta1),1.);
  double theta2 = 2*atan2((double)exp(-eta2),1.);
  double P1     = Pt1/sin(theta1);
  double P2     = Pt2/sin(theta2);
  double p1[3],p2[3];

  // Replace the following by sincos which calculates the sin and cos
  // of the same angle 40% faster (fwinkl)
  // p1[0]  = Pt1*cos(phi1);
  // p1[1]  = Pt1*sin(phi1);
#ifndef __APPLE__
  sincos(phi1,&p1[1],&p1[0]);
  p1[0] *= Pt1;
  p1[1] *= Pt1;
#else
  CxxUtils::sincos scphi1(phi1);
  p1[0] = Pt1*scphi1.cs;
  p1[1] = Pt1*scphi1.sn;
#endif

  // dito
  // p2[0]  = Pt2*cos(phi2);
  // p2[1]  = Pt2*sin(phi2);
#ifndef __APPLE__
  sincos(phi2,&p2[1],&p2[0]);
  p2[0] *= Pt2;
  p2[1] *= Pt2;
#else
  CxxUtils::sincos scphi2(phi2);
  p2[0] = Pt2*scphi2.cs;
  p2[1] = Pt2*scphi2.sn;
#endif
 
  p1[2]  = P1*cos(theta1);
  p2[2]  = P2*cos(theta2);
  
  // evaluate mass
  double Ptot1 = sqrt(std::pow(p1[0],2)+std::pow(p1[1],2)+std::pow(p1[2],2));
  double Ptot2 = sqrt(std::pow(p2[0],2)+std::pow(p2[1],2)+std::pow(p2[2],2));
  double e1 = sqrt(Ptot1*Ptot1 + m1*m1);
  double e2 = sqrt(Ptot2*Ptot2 + m2*m2);
  double mass = sqrt(m1*m1+m2*m2+ 2*e1*e2 - 2*p1[0]*p2[0] - 2*p1[1]*p2[1] - 2*p1[2]*p2[2]); 
  
  return mass;
}
// EF
double ElectronMuonTopoInfo::invariantMass(const egamma* electron1, 
					   const Trk::Perigee* muon1) {
    
  // get parameters: not electron pT no longer signed
  double eta1   = electron1->trackParticle()->eta();
  double eta2   = muon1->eta();

  
  double Pt1    = electron1->cluster()->e()/cosh(electron1->trackParticle()->eta()) ; // IMPORTANT: pt() is the 4-momentum base class method and 
  double Pt2    = muon1->pT(); // it returns the cluster pT; the track pT is given by Pt()
  
  double phi1   = electron1->trackParticle()->phi();
  double phi2 = muon1->parameters()[Trk::phi];

  return  invariantMass(Pt1, eta1, phi1, 0.511, Pt2,  eta2, phi2,  105.6);
}


