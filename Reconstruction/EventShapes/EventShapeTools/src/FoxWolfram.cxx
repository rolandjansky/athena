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
#include "FourMomUtils/FoxWolfram.h"

#include "EventShapeTools/FoxWolfram.h"

#include <cmath>

FoxWolfram::FoxWolfram(const std::string& type,
		       const std::string& name,
		       const IInterface* pParent)
  : EventShapeToolBase(type, name, pParent)
  , m_order(5)
  , m_calc3D(true)
  , m_modified(true)
{
  declareInterface<IEventShapeToolBase>( this );
  declareProperty("CalcOrders",     m_order);
  declareProperty("CalculateIn3D",  m_calc3D);
  declareProperty("CalcModifiedFW", m_modified);
}

StatusCode
FoxWolfram::initialize()
{
  msg(MSG::INFO) << "Calculating first " << m_order << " FoxWolfram moments." << endreq;
  msg(MSG::INFO) << " Calculating FoxWolfram moments in "
		 << ( m_calc3D ? "three" : "two" )
		 << " dimensions. Default : three-D"
		 << endreq;
  msg(MSG::INFO) << " Calculating " << ( m_modified ? "modified" : "original" )
		 << " FoxWolfram moments a la Rick Field. Default : modified." << endreq;
  
  return StatusCode::SUCCESS;
}

StatusCode
FoxWolfram::calculate(const EventShapeTypes::es_4vect_vect_t* theParticles,
		      EventShapeStore *eventShapeStore )
{
  if (msgLvl(MSG::VERBOSE))
    msg(MSG::VERBOSE) << "Initial collection has size " << theParticles->size() << endreq;
  
  std::vector<double> H;
  
  if(FourMomUtils::foxWolfram( theParticles, H, m_order /* , m_modified,  m_calc3D */ ))
    {
      if (msgLvl(MSG::DEBUG))
	{
	  msg(MSG::DEBUG) << "Calculation of first " << m_order
			  << " FoxWolfram moments gave: " << endreq;
	  for ( unsigned int i=0; i<m_order; i++ )
	    msg(MSG::DEBUG) << "  H" << i << " = " << H[i] << endreq;
	}
      eventShapeStore->insert(EventShapes::FOXWOLFRAM,H,true);
    }
  else
    {
      msg(MSG::WARNING) << "Calculation of FoxWolfram failed!" << endreq;
    }
  
  return StatusCode::SUCCESS;
}
