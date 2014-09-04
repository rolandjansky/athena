/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"

#include "GaudiKernel/ListItem.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Service.h"

#include "EventKernel/INavigable4Momentum.h"
#include "NavFourMom/I4MomentumContainer.h"

#include "EventShapeTools/CParameter.h"

#include <cmath>

CParameter::CParameter(const std::string& type,
		       const std::string& name,
		       const IInterface* pParent)
  : EventShapeToolBase(type, name, pParent)
  , m_calc3D(false)
{
  declareInterface<IEventShapeToolBase>( this );
  declareProperty("CalculateIn3D", m_calc3D);
}

StatusCode
CParameter::initialize()
{
  msg(MSG::INFO) << "Calulating CParameter in "
		 << ( m_calc3D ? "three" : "two" )
		 << " dimensions. Default : two-D"
		 << endreq;
  return StatusCode::SUCCESS;
}

StatusCode
CParameter::calculate(const EventShapeTypes::es_4vect_vect_t* theParticles,
		      EventShapeStore* eventShapeStore )
{
  if (msgLvl(MSG::VERBOSE))
    msg(MSG::VERBOSE) << "Initial collection has size " << theParticles->size() << endreq;
  
  double denominator = 0;
  double numerator   = 0;
  double CPar        = 0;
  
  CLHEP::Hep3Vector v1;
  CLHEP::Hep3Vector v2;
  
  double d(0);
  
  EventShapeTypes::es_4vect_vect_t::const_iterator it1;
  EventShapeTypes::es_4vect_vect_t::const_iterator it2;
  
  for ( it1 = theParticles->begin();
	it1 != theParticles->end(); ++it1 )
    {
      d = (*it1)->pz();
      if ( ! m_calc3D )
	d = 0;
      v1.set( (*it1)->px(), (*it1)->py(), d );
      
      numerator += v1.mag();
      
      // self correlation is zero !
      // for ( it2 = theParticles->begin();
      for ( it2 = it1 + 1; it2 != theParticles->end(); ++it2 )
	{
	  d = (*it2)->pz();
	  if ( ! m_calc3D )
	    d = 0;
	  v2.set( (*it2)->px(), (*it2)->py(), d );
	  
	  d = v1.mag() * v2.mag();
	  
	  // additional factor two because we sum only upper triangle in matrix
	  if ( d>0 )
	    denominator += 2 * ( d - v1.dot(v2) * v1.dot(v2) / d );
	}
    }
  
  CPar = 1.5 * denominator / numerator / numerator;
  
  if (msgLvl(MSG::DEBUG))
    msg(MSG::DEBUG) << "Calculation of CParameter gave: " << CPar << endreq;
  
  eventShapeStore->insert(EventShapes::CPARAMETER, CPar);
  
  return StatusCode::SUCCESS;
}
