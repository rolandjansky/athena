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
#include "FourMomUtils/JetBroadening.h"

#include "EventShapeTools/JetBroadening.h"

#include <cmath>

JetBroadening::JetBroadening(const std::string& type,
			     const std::string& name,
			     const IInterface* pParent)
  : EventShapeToolBase(type, name, pParent)
  , m_calc3D(false)
{
  declareInterface<IEventShapeToolBase>( this );
  declareProperty("CalculateIn3D", m_calc3D);
}

StatusCode
JetBroadening::initialize()
{
  msg(MSG::INFO) << "Calulating JetBroadening in "
		 << ( m_calc3D ? "three" : "two" )
		 << " dimensions. Default : two-D"
		 << endreq;
  return StatusCode::SUCCESS;
}

StatusCode
JetBroadening::calculate(const EventShapeTypes::es_4vect_vect_t* theParticles,
			 EventShapeStore *eventShapeStore )
{
  if (msgLvl(MSG::VERBOSE))
    msg(MSG::VERBOSE) << "Initial collection has size " << theParticles->size() << endreq;
  
  CLHEP::Hep3Vector thrust(1,0,0);
  double phi, eta;
  
  if(eventShapeStore->get( EventShapes::THRUST_PHI, phi ))
    thrust.setPhi(phi);
  
  if(eventShapeStore->get( EventShapes::THRUST_ETA, eta ))
    thrust.setEta(eta);
  
  double total;
  double wide;
  
  if(FourMomUtils::jetBroadening( theParticles, wide, total, thrust, m_calc3D ))
    {
      eventShapeStore->insert(EventShapes::TOTAL_BROADENING,total);
      eventShapeStore->insert(EventShapes::WIDE_BROADENING,wide);
    }
  else
    {
      msg(MSG::WARNING) << "Calculation of JetMasses failed!" << endreq;
    }
  if (msgLvl(MSG::DEBUG))
    msg(MSG::DEBUG) << "Calculation of JetBroadenings gave: ( "
		    << wide << " | " << total << " )" << endreq;
  
  return StatusCode::SUCCESS;
}

