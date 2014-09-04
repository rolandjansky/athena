/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"

#include "GaudiKernel/ListItem.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Service.h"

#include "NavFourMom/I4MomentumContainer.h"
#include "FourMomUtils/JetMasses.h"

#include "EventShapeTools/JetMasses.h"

#include <cmath>

JetMasses::JetMasses(const std::string& type,
		     const std::string& name,
		     const IInterface* pParent)
  : EventShapeToolBase(type, name, pParent)
  , m_calc3D(true)
{
  declareInterface<IEventShapeToolBase>( this );
  declareProperty("CalculateIn3D", m_calc3D);
}

StatusCode
JetMasses::initialize()
{
  msg(MSG::INFO) << "Calculating JetMasses in "
		 << ( m_calc3D ? "three" : "two" )
		 << " dimensions. Default : three-D"
		 << endreq;
  return StatusCode::SUCCESS;
}

StatusCode
JetMasses::calculate(const EventShapeTypes::es_4vect_vect_t* theParticles,
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
  
  double heavy;
  double light;
  
  if (msgLvl(MSG::VERBOSE))
    msg(MSG::VERBOSE) << "Thrust retrieved as "
		    << thrust.mag() << " : ( "
		    << thrust.x() << " | "
		    << thrust.y() << " | "
		    << thrust.z() << " )" << endreq;
  
  if(FourMomUtils::jetMasses( theParticles, heavy, light, thrust /* , m_calc3D */ ))
    {
      eventShapeStore->insert(EventShapes::HEAVY_JETMASS, heavy);
      eventShapeStore->insert(EventShapes::LIGHT_JETMASS, light);
    }
  else
    {
      msg(MSG::WARNING) << "Calculation of JetMasses failed!" << endreq;
    }
  
  if (msgLvl(MSG::DEBUG))
    msg(MSG::DEBUG) << "Calculation of JetMasses gave: ( "
		    << heavy << " | " << light << " )" << endreq;
  
  return StatusCode::SUCCESS;
}

