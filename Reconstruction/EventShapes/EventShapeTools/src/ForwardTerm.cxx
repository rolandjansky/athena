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
#include "FourMomUtils/ForwardTerm.h"

#include "EventShapeTools/ForwardTerm.h"

#include <cmath>

ForwardTerm::ForwardTerm(const std::string& type,
			 const std::string& name,
			 const IInterface* pParent)
  : EventShapeToolBase(type, name, pParent)
  , m_calc3D(false)
{
  declareInterface<IEventShapeToolBase>( this );
  declareProperty("CalculateIn3D", m_calc3D);
}

StatusCode
ForwardTerm::initialize()
{
  msg(MSG::INFO) << "Calulating ForwardTerm in "
		 << ( m_calc3D ? "three" : "two" )
		 << " dimensions. Default : two-D"
		 << endreq;
  return StatusCode::SUCCESS;
}

StatusCode
ForwardTerm::calculate(const EventShapeTypes::es_4vect_vect_t* theParticles,
		       EventShapeStore *eventShapeStore )
{
  if (msgLvl(MSG::VERBOSE))
    msg(MSG::VERBOSE) << "Initial collection has size " << theParticles->size() << endreq;
  
  double forward_term = FourMomUtils::forwardTerm( theParticles,
						   m_cutCentral,
						   m_calc3D );
  
  eventShapeStore->insert(EventShapes::FORWARD_TERM,forward_term);
  
  if (msgLvl(MSG::DEBUG))
    msg(MSG::DEBUG) << "Calculation of ForwardTerm gave: "
		    << forward_term << endreq;
  
  return StatusCode::SUCCESS;
}
