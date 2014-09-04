/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "EventKernel/INavigable4Momentum.h"
#include "NavFourMom/I4MomentumContainer.h"
#include "FourMomUtils/P4Helpers.h"

#include "EventShapeTools/EEC.h"

#include <cmath>

EEC::EEC(const std::string& type,
	 const std::string& name,
	 const IInterface* pParent)
  : EventShapeToolBase(type, name, pParent), n_Events(0)
{
  declareInterface<IEventShapeToolBase>( this );
}

StatusCode
EEC::calculate(const EventShapeTypes::es_4vect_vect_t* theParticles,
	       EventShapeStore* eventShapeStore )
{
  if (msgLvl(MSG::VERBOSE))
    msg(MSG::VERBOSE) << "Initial collection has size " << theParticles->size() << endreq;
  
  double EnergySum=0;
  double ETSum=0;
  
  for ( EventShapeTypes::es_4vect_vect_t::const_iterator itr=theParticles->begin();
        itr!=theParticles->end(); itr++)
    {
      if(fabs((*itr)->eta())<m_cutCentral)
        {
	  EnergySum += (*itr)->e();
	  ETSum +=     (*itr)->et();
	}
    }
  
  // double dAngle;
  int bin;
  std::vector<double> eec_phi(100,0);
  std::vector<double> eec_eta(100,0);
  std::vector<double> eec_rad(100,0);
  float max_phi(3.2);
  float max_eta(8);
  float max_rad(8);
  
  CLHEP::Hep3Vector ci;
  CLHEP::Hep3Vector cj;
  
  for ( EventShapeTypes::es_4vect_vect_t::const_iterator itr=theParticles->begin();
        itr!=theParticles->end(); itr++)
    {
      ci.set((*itr)->px(), (*itr)->py(), (*itr)->pz());
      double Ei=(*itr)->e();
      if(fabs((*itr)->eta())<m_cutCentral)
        {
	  for ( EventShapeTypes::es_4vect_vect_t::const_iterator jtr=theParticles->begin();
		jtr!=theParticles->end(); jtr++)
	    {
	      cj.set((*jtr)->px(), (*jtr)->py(), (*jtr)->pz());
	      double Ej=(*jtr)->e();
	      if(fabs((*jtr)->eta())<m_cutCentral)
		{
		  bin = int(100.0 * fabs(P4Helpers::deltaPhi(*itr,*jtr)) / max_phi);
		  if ( bin >= 100 ) bin=100;
		  if ( bin <    0 ) bin=0;
		  eec_phi[bin] = Ei*Ej/EnergySum/EnergySum;
		  
		  bin = int(100.0 * fabs((*itr)->eta()-(*jtr)->eta()) / max_eta);
		  if ( bin >= 100 ) bin=100;
		  if ( bin <    0 ) bin=0;
		  eec_eta[bin]=Ei*Ej/EnergySum/EnergySum;
		  
		  bin = int(100.0 * fabs(P4Helpers::deltaR(*itr,*jtr)) / max_rad);
		  if ( bin >= 100 ) bin=100;
		  if ( bin <    0 ) bin=0;
		  eec_rad[bin]=Ei*Ej/EnergySum/EnergySum;
		}
	    }
	}
    }
  eventShapeStore->insert(EventShapes::EEC_PHI, eec_phi, true );
  eventShapeStore->insert(EventShapes::EEC_ETA, eec_eta, true );
  eventShapeStore->insert(EventShapes::EEC_RAD, eec_rad, true );
  
  return StatusCode::SUCCESS;
}
