// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigEgammaHypo/TrigL2DielectronMassFex.h
 **
 **   Description: - Hypothesis algorithm: searc for electron pairs with 
 **                invariant mass in some interval; intended for Z->ee
 **                - Modified from TrigL2DimuonMassFex by C.Schiavi
 **
 **   Author: R.Goncalo  <r.goncalo@rhul.ac.uk>
 **
 **   Created:   Aug 13 2006
 **   Modified:  Jan 19 2007 Valeria Perez Reale Adapted to New Steering  
 **   Modified:  Apr 28 2007 Tomasz Bold major changes to run with new steering
 **   Modified:  May 25 2011 Toyonobu Okuyama major change to attach the calculated 
 **                          invariant mass to outputTE as TrigOperationalInfo
 **************************************************************************/ 

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigOperationalInfo.h"

#include "TrigEgammaHypo/TrigL2DielectronMassFex.h"
#include "CxxUtils/sincos.h"

#include <math.h>
#include <cstdio>

TrigL2DielectronMassFex::TrigL2DielectronMassFex(const std::string & name, ISvcLocator* pSvcLocator)
  : HLT::ComboAlgo(name, pSvcLocator) {

  // use cluster mode
  declareProperty("UseClusterAsProbe",    m_useClusterAsProbe=true);

  // electron mass definition
  declareProperty("ElectronMass",    m_electronMass=0.511);

  // Read cuts
  declareProperty("AcceptAll",    m_acceptAll);
  declareProperty("SameTrackAlgo",m_sameTrackAlgo=true);
  declareProperty("OppositeSign", m_oppositeCharge=true);
  declareProperty("CommonVertex", m_commonVertex=true);
  declareProperty("ValidElectron",m_electronValid=true);
  declareProperty("LowerMassCut", m_lowerMassCut=1000.0);//! for J/psi
  declareProperty("UpperMassCut", m_upperMassCut=15000.0);//! for J/psi

  
  // monitoring
  declareMonitoredVariable("massOfAccepted", m_monMassAccepted);
  declareMonitoredVariable("cut", m_monCut);
}

TrigL2DielectronMassFex::~TrigL2DielectronMassFex()
{ }

HLT::ErrorCode TrigL2DielectronMassFex::hltInitialize()
{
  
  if (msgLvl() <= MSG::VERBOSE) {
    msg() << MSG::DEBUG << "Initialization:" << endreq;
  }

  
  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Initialization completed successfully:" << endreq;
    msg() << MSG::DEBUG << "AcceptAll            = " 
	  << (m_acceptAll==true ? "True" : "False") << endreq; 
    msg() << MSG::DEBUG << "UseClusterAsProbe    = " 
	  << (m_useClusterAsProbe==true ? "True" : "False") << endreq; 
    msg() << MSG::DEBUG << "ElectronMass         = " << m_electronMass << endreq;
    msg() << MSG::DEBUG << "ValidElectron        = " 
	  << (m_electronValid==true ? "True" : "False") << endreq; 
    msg() << MSG::DEBUG << "SameTrackAlgo        = " 
	  << (m_sameTrackAlgo==true ? "True" : "False") << endreq; 
    msg() << MSG::DEBUG << "OppositeCharge       = " 
	  << (m_oppositeCharge==true ? "True" : "False") << endreq; 
    msg() << MSG::DEBUG << "CommonVertex         = " 
	  << (m_commonVertex==true ? "True" : "False") << endreq; 
    msg() << MSG::DEBUG << "LowerMassCut         = " << m_lowerMassCut << endreq;
    msg() << MSG::DEBUG << "UpperMassCut         = " << m_upperMassCut << endreq;
  }
  
  return HLT::OK;
}


HLT::ErrorCode TrigL2DielectronMassFex::hltFinalize()
{
  if ( msgLvl() <= MSG::INFO )
    msg() << MSG::INFO << "in finalize()" << endreq;

  return HLT::OK;
}


HLT::ErrorCode TrigL2DielectronMassFex::acceptInputs(HLT::TEConstVec& inputTE, bool& pass ) {
  m_massCont.clear();
  m_monMassAccepted = -1.;
  m_monCut=0;
  // sanity checks
  if ( msgLvl() <= MSG::DEBUG )
    msg() << MSG::DEBUG << "Running TrigL2DielectronMassFex::acceptInputs" << endreq;

  if ( inputTE.size() != 2 ) {
    msg() << MSG::ERROR << "Got diferent than 2 number of input TEs: " <<  inputTE.size() << " job badly configured" << endreq;
    return HLT::BAD_JOB_SETUP;
  }



  // Accept-All mode: temporary patch; should be done with force-accept 
  if (m_acceptAll) {
    if ( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << "AcceptAll property is set: taking all events" 
	    << endreq;
    
    pass = true;
    return HLT::OK;
  } 
  else {
    if ( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << "AcceptAll property not set: applying selection" 
	    << endreq;
  }

  // this are 2 TEs which we eventually will combine
  const HLT::TriggerElement* te1 = inputTE[0];
  const HLT::TriggerElement* te2 = inputTE[1];

  // for debugging purpose look into RoIDescriptors
  if ( msgLvl() <= MSG::DEBUG ){
    const TrigRoiDescriptor* roiDescriptor1 = 0;
    const TrigRoiDescriptor* roiDescriptor2 = 0;
    if ( getFeature(te1, roiDescriptor1) != HLT::OK || getFeature(te2, roiDescriptor2) != HLT::OK ) {
      if ( msgLvl() <= MSG::WARNING) {
	msg() <<  MSG::WARNING << "No RoIDescriptors for this Trigger Elements! " << endreq;
      }  
    } else {
      if ( msgLvl() <= MSG::DEBUG )
	msg() << MSG::DEBUG  << "Trying to combine 2 RoIs: " << *roiDescriptor1 << " & " << *roiDescriptor2 << endreq;
    }
  }
  
  // retrieve TrigElectronContainers(orTrigEMCluster) from this TE
  const xAOD::TrigElectronContainer*  electronContainer1(0);
  const xAOD::TrigElectronContainer*  electronContainer2(0);
  const xAOD::TrigEMCluster*          cluster2(0);//! index "2" means this container is used for probe
  if( m_useClusterAsProbe) {
    if ( getFeature(te1, electronContainer1) != HLT::OK || getFeature(te2, cluster2) != HLT::OK || 
	 electronContainer1 == 0 || cluster2 == 0 ) {
      if ( msgLvl() <= MSG::DEBUG) {
	msg() << MSG::DEBUG << "Failed to get TrigElectron or TrigEMCluster collections" << endreq;
      } 
      return HLT::MISSING_FEATURE;
    }
  } else if ( getFeature(te1, electronContainer1) != HLT::OK || getFeature(te2, electronContainer2) != HLT::OK || 
       electronContainer1 == 0 ||  electronContainer2 == 0 ) {
    if ( msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "Failed to get TrigElectron collections" << endreq;
    } 
    return HLT::MISSING_FEATURE;
  }


  pass=false;

  // now we have 2 bunches of electrons and we need to find out whether they can form topological combination
  // loop ver all combinations
  xAOD::TrigElectronContainer::const_iterator electron1;
  xAOD::TrigElectronContainer::const_iterator electron2;

  if( m_useClusterAsProbe ) {
    // Use Cluster Mode (TrigElectron - TrigEMCluster)
    //==================================================
    for ( electron1 = electronContainer1->begin(); electron1 != electronContainer1->end(); ++electron1 ) {
      m_monCut = 5;
      // evaluate mass
      double mass = TrigL2DielectronMassHelpers::invariantMass((*electron1), cluster2, m_electronMass);

      // apply cut on mass
      if(mass<m_lowerMassCut || mass>m_upperMassCut) {	
	if(msgLvl() <= MSG::VERBOSE) {
	  msg() << MSG::VERBOSE << "Combination failed mass cut: " 
		<< mass << " not in [" << m_lowerMassCut << "," 
		<< m_upperMassCut << "]" << endreq;
	}
	continue;
      } else {
	// good combination found
	pass = true;
	m_monCut = 6;
	m_monMassAccepted = mass;
	m_massCont.push_back(mass);
	if(msgLvl() <= MSG::VERBOSE) {
	  msg() << MSG::VERBOSE << "Combination passed mass cut: " 
		<< m_lowerMassCut << " < " << mass << " < " 
		<< m_upperMassCut << endreq;	     
	  msg() << MSG::DEBUG << "Good combination found! Mee=" 
		<< mass << " MeV" << endreq;
	}
      }
    }
  } else {
    // Normal Mode (TrigElectron - TrigElectron)
    //==================================================
    for ( electron1 = electronContainer1->begin(); electron1 != electronContainer1->end(); ++electron1 ) {
      for ( electron2 = electronContainer2->begin(); electron2 != electronContainer2->end(); ++electron2 ) {
	m_monCut = 1;
	// selection is done here
	//
	// debug dump (both electrons have tracks now)
	
	if(msgLvl() <= MSG::VERBOSE) {
	  msg() << MSG::VERBOSE << "New combination:" << endreq; 
	  msg() << MSG::VERBOSE << "1st TrigElectron " 
		//<< ((*electron1)->isValid() ? "(valid" : "(not valid")
		<< ") RoI id="<< (*electron1)->roiWord()
		<< "; pt="    << (*electron1)->pt()  
		<< "; eta="   << (*electron1)->eta()// <<"+-"<< (*electron1)->err_eta() 
		<< "; phi="   << (*electron1)->phi()// <<"+-"<< (*electron1)->err_phi() 
//		<< "; Zvtx="  << (*electron1)->Zvtx()<<"+-"<< (*electron1)->err_Zvtx() 
		<< endreq;
	  msg() << MSG::VERBOSE << "2nd TrigElectron " 
		//<< ((*electron1)->isValid() ? "(valid" : "(not valid")
		<< ") RoI id="<< (*electron2)->roiWord()
		<< "; pt="    << (*electron2)->pt()  
		<< "; eta="   << (*electron2)->eta()// <<"+-"<< (*electron2)->err_eta() 
		<< "; phi="   << (*electron2)->phi()// <<"+-"<< (*electron2)->err_phi() 
//		<< "; Zvtx="  << (*electron2)->Zvtx()<<"+-"<< (*electron2)->err_Zvtx() 
		<< endreq;
	}
	
	// check we're starting from valid TrigElectrons (i.e. which 
	// passed all cuts)
	if (m_electronValid) {
	  
	  if ( ! TrigL2DielectronMassHelpers::validElectrons(*electron1, *electron2) ) {
	    
	    if(msgLvl() <= MSG::VERBOSE) {
	      msg() << MSG::VERBOSE << "One TrigElectron not valid: rejecting combination" << endreq;
	    }	    
	    continue;  // reject non-valid TrigElectrons if cut is on
	  }
	}
	m_monCut = 2;
	// tracking algorithm (sensible to separate things for now so 
	// that efficiency can be easily measured for each tracking algo)
	if (m_sameTrackAlgo) {
	  
#ifdef DONTDO
	  if ( (*electron1)->trackAlgo() != (*electron2)->trackAlgo() ) {
	    
	    if(msgLvl() <= MSG::VERBOSE) {
	      msg() << MSG::VERBOSE 
		    << "TrigElectrons from different tracking algos: ignoring combination" << endreq;
	    }    
	  } else {      
	    if(msgLvl() <= MSG::VERBOSE) {
	      msg() << MSG::VERBOSE << "TrigElectrons from same tracking algo" 
		    << endreq;
	    }
	  }
#endif
	}

	
	m_monCut = 3;
	// check charge: track pT is signed according to charge but not electron pT
	// should study how well charge can actually be estimated for high-pT tracks...
	if( m_oppositeCharge ) {
	  
	  // note: track charge can be zero (unknown) so must use > and not >=
	  if ( ! TrigL2DielectronMassHelpers::opositeCharge(*electron1, *electron2) ) {
	    if(msgLvl() <= MSG::VERBOSE) {
	      msg() << MSG::VERBOSE << "Combination failed opposite charge cut" << endreq;
	    }
	    continue;
	  } else {
	    if(msgLvl() <= MSG::VERBOSE) {
	      msg() << MSG::VERBOSE << "Combination passed opposite charge cut" 
		    << endreq;
	    }
	  }
	}     
	m_monCut = 4;
	
	
	// check vertex position: two electrons should come from common 
	// vertex check only Z of vertex, and only ef Z error is not zero
	// take track z at perigee as estimate of vertex z, since lateral 
	// coordinates are small this should be done better with a vertexing
	// algorithm...
	if( m_commonVertex ) {
	  
	  const TrigL2DielectronMassHelpers::Vertex vx = TrigL2DielectronMassHelpers::commonVertex(*electron1, *electron2);
	  
	  if ( vx == TrigL2DielectronMassHelpers::NotCommon ) {
	    if (msgLvl() <= MSG::VERBOSE) {
	      msg() << MSG::VERBOSE 
		    << "Combination failed vertex cut" 
		    << endreq;
	    }
	    
	    continue;
	  } else  if ( vx == TrigL2DielectronMassHelpers::Common ) {
	    if (msgLvl() <= MSG::VERBOSE) {
	      msg() << MSG::VERBOSE 
		    << "Combination passed vertex cut" << endreq;
	    }
	  } else {
	    if(msgLvl() <= MSG::VERBOSE) {
	      msg() << MSG::VERBOSE 
		    << "Bad track vertex errors: not applying cut" << endreq;
	    }
	  }
	}
	m_monCut = 5;
	// evaluate mass
	double mass = TrigL2DielectronMassHelpers::invariantMass((*electron1),(*electron2), m_electronMass);
	
	// apply cut on mass
	if(mass<m_lowerMassCut || mass>m_upperMassCut) {	
	  if(msgLvl() <= MSG::VERBOSE) {
	    msg() << MSG::VERBOSE << "Combination failed mass cut: " 
		  << mass << " not in [" << m_lowerMassCut << "," 
		  << m_upperMassCut << "]" << endreq;
	  }
	  continue;
	} else {
	  // good combination found
	  pass = true;
	  m_monCut = 6;    
	  m_monMassAccepted = mass;
	  m_massCont.push_back(mass);
	  if(msgLvl() <= MSG::VERBOSE) {
	    msg() << MSG::VERBOSE << "Combination passed mass cut: " 
		<< m_lowerMassCut << " < " << mass << " < " 
		  << m_upperMassCut << endreq;
	  } else if(msgLvl() <= MSG::DEBUG) {
	    msg() << MSG::DEBUG << "Good combination found! Mee=" 
		  << mass << " MeV" << endreq;
	  }
	}
      } // electrons2 container loop end
    } // electrons1 container loop end
  }

  // set output TriggerElement true if good combination
  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "pass = " << pass << endreq;
  return HLT::OK;    
}  

HLT::ErrorCode TrigL2DielectronMassFex::hltExecute(HLT::TEConstVec& /*inputTE*/, HLT::TriggerElement* outputTE){
  msg() << MSG::DEBUG << "N mass accepted : " << m_massCont.size() << endreq;
  TrigOperationalInfo* trigInfo = new TrigOperationalInfo();
  for(size_t i=0; i<m_massCont.size(); ++i) {
    char buf[64];
    std::sprintf(buf, "mass%d", (int)i);
    msg() << MSG::DEBUG << "    " << buf << " : " << m_massCont[i] << endreq;    
    trigInfo->set(std::string(buf), m_massCont[i]);
  }
  attachFeature(outputTE, trigInfo, "L2DielectronInfo"); 
  msg() << MSG::DEBUG << "attached feature" << endreq;

  return HLT::OK;
}


bool TrigL2DielectronMassHelpers::validElectrons(const xAOD::TrigElectron* /*electron1*/, 
						 const xAOD::TrigElectron* /*electron2*/) {

/*
  if ( !(electron1->isValid() && electron2->isValid()) ) 
    return false;
*/
  return true;
}

TrigL2DielectronMassHelpers::Vertex TrigL2DielectronMassHelpers::commonVertex(const xAOD::TrigElectron* /*electron1*/, 
									      const xAOD::TrigElectron* /*electron2*/) {
  
	    
#ifdef DONTDO
  double trk_e1_z0error = electron1->err_Zvtx();
  double trk_e2_z0error = electron2->err_Zvtx();
  
  if ( trk_e1_z0error > 0 && trk_e2_z0error > 0 ) {
    double z0_distance = fabs(electron1->Zvtx() 
			      - electron2->Zvtx());
    
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
#endif
  return Unapplicable;
}


bool TrigL2DielectronMassHelpers::opositeCharge(const xAOD::TrigElectron* electron1, 
						const xAOD::TrigElectron* electron2) {
  if ( electron1->charge() * electron2->charge() > 0 )
    return false;
  return true;
}

double TrigL2DielectronMassHelpers::invariantMass(const xAOD::TrigElectron* electron1, 
						  const xAOD::TrigElectron* electron2,
						  const double electronMass) {
  
  // get parameters: not electron pT no longer signed
  double eta1   = electron1->eta();
  double eta2   = electron2->eta();
  
  double Pt1    = electron1->pt() ; // IMPORTANT: pt() is the 4-momentum base class method and 
  double Pt2    = electron2->pt() ; // it returns the cluster pT; the track pT is given by Pt()
  
  double phi1   = electron1->phi();
  double phi2   = electron2->phi();
  
  double theta1 = 2*atan2((double)exp(-eta1),1.);
  double theta2 = 2*atan2((double)exp(-eta2),1.);
  double P1     = fabs(Pt1)/sin(theta1);
  double P2     = fabs(Pt2)/sin(theta2);
  
  double p1[3],p2[3];

  // Replace the following by sincos which calculates the sin and cos
  // of the same angle 40% faster (fwinkl)
#ifndef __APPLE__
  sincos(phi1,&p1[1],&p1[0]);
  p1[0] *= Pt1;
  p1[1] *= Pt1;
#else
  CxxUtils::sincos scphi1(phi1);
  p1[0] = Pt1*scphi1.cs;
  p1[1] = Pt1*scphi1.sn;
#endif

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
  double e1 = sqrt(electronMass + Ptot1*Ptot1);
  double e2 = sqrt(electronMass + Ptot2*Ptot2);
  double mass = sqrt(2*e1*e2 - 2*p1[0]*p2[0] - 2*p1[1]*p2[1] - 2*p1[2]*p2[2]); 
  
  return mass;
}

double TrigL2DielectronMassHelpers::invariantMass(const xAOD::TrigElectron* electron,
						  const xAOD::TrigEMCluster* cluster,
						  const double electronMass) {
  // get parameters: not electron pT no longer signed
  double eta1   = electron->eta();
  double eta2   = cluster->eta();
  
  double Pt1    = electron->pt(); // IMPORTANT: pt() is the 4-momentum base class method and 
  double Pt2    = cluster->et();  // it returns the cluster pT; the track pT is given by Pt()
  
  double phi1   = electron->phi();
  double phi2   = cluster->phi();
  
  double theta1 = 2*atan2((double)exp(-eta1),1.);
  double theta2 = 2*atan2((double)exp(-eta2),1.);
  double P1     = fabs(Pt1)/sin(theta1);
  double P2     = fabs(Pt2)/sin(theta2);
  
  double p1[3],p2[3];

  // Replace the following by sincos which calculates the sin and cos
  // of the same angle 40% faster (fwinkl)
#ifndef __APPLE__
  sincos(phi1,&p1[1],&p1[0]);
  p1[0] *= Pt1;
  p1[1] *= Pt1;
#else
  CxxUtils::sincos scphi1(phi1);
  p1[0] = Pt1*scphi1.cs;
  p1[1] = Pt1*scphi1.sn;
#endif

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
  double e1 = sqrt(electronMass + Ptot1*Ptot1);
  double e2 = sqrt(electronMass + Ptot2*Ptot2);
  double mass = sqrt(2*e1*e2 - 2*p1[0]*p2[0] - 2*p1[1]*p2[1] - 2*p1[2]*p2[2]); 
  
  return mass;
}
