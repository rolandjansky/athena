/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "EventKernel/INavigable4Momentum.h"
#include "NavFourMom/I4MomentumContainer.h"
#include "FourMomUtils/Thrust.h"

#include "EventShapeTools/Thrust.h"

#include <cmath>

Thrust::Thrust(const std::string& type,
	       const std::string& name,
	       const IInterface* pParent)
  : EventShapeToolBase(type, name, pParent)
  , m_calc3D(false)
{
  declareInterface<IEventShapeToolBase>( this );
  declareProperty("CalculateIn3D", m_calc3D);
}

StatusCode
Thrust::initialize()
{
  msg(MSG::INFO) << "Calulating Thrust in "
		 << ( m_calc3D ? "three" : "two" )
		 << " dimensions. Default : two-D"
		 << endreq;
  return StatusCode::SUCCESS;
}

StatusCode
Thrust::calculate(const EventShapeTypes::es_4vect_vect_t* theParticles,
		  EventShapeStore* eventShapeStore )
{
  if (msgLvl(MSG::VERBOSE))
    msg(MSG::VERBOSE) << "Initial collection has size " << theParticles->size() << endreq;
  
  double thrust_major=0;
  double thrust_minor=0;
  CLHEP::Hep3Vector thrust = FourMomUtils::thrust( theParticles, thrust_major, thrust_minor, m_calc3D );
  
  double thrust_phi = atan2(thrust.y(),thrust.x());
  double thrust_eta = 0;
  if ( m_calc3D )
    thrust_eta = 0.5*log((thrust_major+thrust.z())/(thrust_major-thrust.z()));
  
  if (msgLvl(MSG::DEBUG))
    {
      msg(MSG::DEBUG)
	<< "Calculation of Thrust gave: ( " << thrust.x()
	<< " | " << thrust.y();
      if ( m_calc3D )
	msg(MSG::DEBUG)
	  << " | " << thrust.z();
      msg(MSG::DEBUG)
	<< " ) coresponding to ( ";
      if ( m_calc3D )
	msg(MSG::DEBUG)
	  << thrust.eta() << " | ";
      msg(MSG::DEBUG)
	<< thrust.phi() << " )"
	<< endreq;
    }
  
  eventShapeStore->insert(EventShapes::THRUST, thrust_major);
  eventShapeStore->insert(EventShapes::THRUST_MINOR, thrust_minor);
  eventShapeStore->insert(EventShapes::THRUST_PHI, thrust_phi );
  if ( m_calc3D )
    eventShapeStore->insert(EventShapes::THRUST_ETA, thrust_eta );
  
  return StatusCode::SUCCESS;
}

