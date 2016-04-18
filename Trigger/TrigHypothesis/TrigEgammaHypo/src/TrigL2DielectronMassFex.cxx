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
 **   Modified:  Apr    2016 use TrigComposite
 **************************************************************************/ 

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigOperationalInfo.h"
#include "TrigSteeringEvent/TrigPassBits.h"

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

    ATH_MSG_DEBUG("Initialization:");

    ATH_MSG_DEBUG("Initialization completed successfully:");
    ATH_MSG_DEBUG("AcceptAll            = " 
            << (m_acceptAll==true ? "True" : "False"));
    ATH_MSG_DEBUG("UseClusterAsProbe    = " 
            << (m_useClusterAsProbe==true ? "True" : "False"));
    ATH_MSG_DEBUG("ElectronMass         = " << m_electronMass);
    ATH_MSG_DEBUG("ValidElectron        = " 
            << (m_electronValid==true ? "True" : "False"));
    ATH_MSG_DEBUG("SameTrackAlgo        = " 
            << (m_sameTrackAlgo==true ? "True" : "False"));
    ATH_MSG_DEBUG("OppositeCharge       = " 
            << (m_oppositeCharge==true ? "True" : "False"));
    ATH_MSG_DEBUG("CommonVertex         = " 
            << (m_commonVertex==true ? "True" : "False"));
    ATH_MSG_DEBUG("LowerMassCut         = " << m_lowerMassCut);
    ATH_MSG_DEBUG("UpperMassCut         = " << m_upperMassCut);

    return HLT::OK;
}


HLT::ErrorCode TrigL2DielectronMassFex::hltFinalize()
{
    ATH_MSG_INFO("in finalize()");

  return HLT::OK;
}


HLT::ErrorCode TrigL2DielectronMassFex::acceptInputs(HLT::TEConstVec& inputTE, bool& pass ) {
  m_massCont.clear();
  m_monMassAccepted = -1.;
  m_monCut=0;
  // sanity checks
  ATH_MSG_DEBUG("Running TrigL2DielectronMassFex::acceptInputs");

  if ( inputTE.size() != 2 ) {
    ATH_MSG_ERROR( "Got diferent than 2 number of input TEs: " <<  inputTE.size() << " job badly configured");
    return HLT::BAD_JOB_SETUP;
  }

  // Accept-All mode: temporary patch; should be done with force-accept 
  if (m_acceptAll) {
      ATH_MSG_DEBUG("AcceptAll property is set: taking all events");
      pass = true;
      return HLT::OK;
  } 
  else {
      ATH_MSG_DEBUG("AcceptAll property not set: applying selection");
  }

  // this are 2 TEs which we eventually will combine
  const HLT::TriggerElement* te1 = inputTE[0];
  const HLT::TriggerElement* te2 = inputTE[1];

  // for debugging purpose look into RoIDescriptors
  if ( msgLvl() <= MSG::DEBUG ){
      const TrigRoiDescriptor* roiDescriptor1 = 0;
      const TrigRoiDescriptor* roiDescriptor2 = 0;
      if ( getFeature(te1, roiDescriptor1) != HLT::OK || getFeature(te2, roiDescriptor2) != HLT::OK ) 
          ATH_MSG_WARNING("No RoIDescriptors for this Trigger Elements! ");
      else 
          ATH_MSG_DEBUG("Trying to combine 2 RoIs: " << *roiDescriptor1 << " & " << *roiDescriptor2);
  }
  
  // retrieve TrigElectronContainers(orTrigEMCluster) from this TE
  const xAOD::TrigElectronContainer*  electronContainer1(0);
  const xAOD::TrigElectronContainer*  electronContainer2(0);
  const xAOD::TrigEMCluster*          cluster2(0);//! index "2" means this container is used for probe
  if( m_useClusterAsProbe) {
    if ( getFeature(te1, electronContainer1) != HLT::OK || getFeature(te2, cluster2) != HLT::OK || 
	 electronContainer1 == 0 || cluster2 == 0 ) {
	ATH_MSG_WARNING("Failed to get TrigElectron or TrigEMCluster collections");
      return HLT::MISSING_FEATURE;
    }
  } else if ( getFeature(te1, electronContainer1) != HLT::OK || getFeature(te2, electronContainer2) != HLT::OK || 
       electronContainer1 == 0 ||  electronContainer2 == 0 ) {
      ATH_MSG_WARNING("Failed to get TrigElectron collections");
    return HLT::MISSING_FEATURE;
  }

  const TrigPassBits* bits(0);
  HLT::ErrorCode status = getFeature(te1, bits, "passbits");
  if (status != HLT::OK) {
    ATH_MSG_WARNING(" Failed to get TrigPassBits ");
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
        if(!HLT::isPassing( bits, (*electron1), electronContainer1 )) {
            ATH_MSG_DEBUG("Electron found not passing Hypo object");
            continue;
        }
      m_monCut = 5;
      // evaluate mass
      double mass = TrigL2DielectronMassHelpers::invariantMass((*electron1), cluster2, m_electronMass);

      // apply cut on mass
      if(mass<m_lowerMassCut || mass>m_upperMassCut) {	
	  ATH_MSG_DEBUG("Combination failed mass cut: " 
		<< mass << " not in [" << m_lowerMassCut << "," 
		<< m_upperMassCut << "]");
	continue;
      } else {
	// good combination found
	pass = true;
	m_monCut = 6;
	m_monMassAccepted = mass;
	m_massCont.push_back(mass);
        ATH_MSG_DEBUG("Good combination found! Mee="<< mass << " MeV"); 
            
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
	
	  ATH_MSG_DEBUG("New combination:");
	  ATH_MSG_DEBUG("1st TrigElectron " 
		<< ") RoI id="<< (*electron1)->roiWord()
		<< "; pt="    << (*electron1)->pt()  
		<< "; eta="   << (*electron1)->eta()
                << "; trkEta= " << (*electron1)->trackParticle()->eta()
		<< "; phi="   << (*electron1)->phi());
	  ATH_MSG_DEBUG("2nd TrigElectron " 
		<< ") RoI id="<< (*electron2)->roiWord()
		<< "; pt="    << (*electron2)->pt()  
		<< "; eta="   << (*electron2)->eta()
                << "; trkEta= " << (*electron1)->trackParticle()->eta() 
		<< "; phi="   << (*electron2)->phi());
          
          m_monCut = 2;
          //Ensure the tag electron passes hypo
          if(!HLT::isPassing( bits, (*electron1), electronContainer1 )) {
              ATH_MSG_DEBUG("Electron found not passing Hypo object");
              continue;
          }

	
	m_monCut = 3;
	// check charge: track pT is signed according to charge but not electron pT
	// should study how well charge can actually be estimated for high-pT tracks...
	if( m_oppositeCharge ) {
	  
	  // note: track charge can be zero (unknown) so must use > and not >=
	  if ( ! TrigL2DielectronMassHelpers::opositeCharge(*electron1, *electron2) ) {
	      ATH_MSG_DEBUG("Combination failed opposite charge cut");
	    continue;
	  } else {
	      ATH_MSG_DEBUG("Combination passed opposite charge cut");
	  }
	}     
	m_monCut = 4;
	
	
	// check vertex position: two electrons should come from common 
	// vertex check only Z of vertex, and only ef Z error is not zero
	// take track z at perigee as estimate of vertex z, since lateral 
	// coordinates are small this should be done better with a vertexing
	// algorithm...
	// Remove call since code disabled
        m_monCut = 5;
	// evaluate mass
	double mass = TrigL2DielectronMassHelpers::invariantMass((*electron1),(*electron2), m_electronMass);
	
	// apply cut on mass
        if(mass<m_lowerMassCut || mass>m_upperMassCut) {	
            ATH_MSG_DEBUG("Combination failed mass cut: " 
                    << mass << " not in [" << m_lowerMassCut << "," 
                    << m_upperMassCut << "]");
            continue;
        } else {
            // good combination found
            pass = true;
            m_monCut = 6;    
            m_monMassAccepted = mass;
            m_massCont.push_back(mass);
            ATH_MSG_DEBUG("Good combination found! Mee="<< mass << " MeV");

        }
      } // electrons2 container loop end
    } // electrons1 container loop end
  }

  // set output TriggerElement true if good combination
  ATH_MSG_DEBUG("pass = " << pass);
  return HLT::OK;    
}  

HLT::ErrorCode TrigL2DielectronMassFex::hltExecute(HLT::TEConstVec& /*inputTE*/, HLT::TriggerElement* outputTE){
  ATH_MSG_DEBUG("N mass accepted : " << m_massCont.size());
  m_cont=new xAOD::TrigCompositeContainer();
  xAOD::TrigCompositeAuxContainer contaux;
  m_cont->setStore(&contaux);
  for(const auto &mass:m_massCont) {
    xAOD::TrigComposite *comp=new xAOD::TrigComposite();;
    m_cont->push_back(comp);
    comp->setName("L2Dielectron");
    comp->setDetail("Mee",mass);
  }
  attachFeature(outputTE,m_cont,"L2DielectronInfo"); 
  ATH_MSG_DEBUG("attached feature");

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
