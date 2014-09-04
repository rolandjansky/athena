/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "EventKernel/INavigable4Momentum.h"
#include "NavFourMom/I4MomentumContainer.h"

#include "EventShapeTools/TransverseSphericity.h"

#include <cmath>

TransverseSphericity::TransverseSphericity(const std::string& type,
					   const std::string& name,
					   const IInterface* pParent)
  : EventShapeToolBase(type, name, pParent)
{
  declareInterface<IEventShapeToolBase>( this );
}

StatusCode
TransverseSphericity::calculate(const EventShapeTypes::es_4vect_vect_t* theParticles,
				EventShapeStore* eventShapeStore )
{
  if (msgLvl(MSG::VERBOSE))
    msg(MSG::VERBOSE) << "Initial collection has size " << theParticles->size() << endreq;
  
  double lambda1=0;
  double lambda2=0;
  
  double pxsq = 0;
  double pysq = 0;
  double pxpy = 0;
  for ( EventShapeTypes::es_4vect_vect_t::const_iterator it = theParticles->begin();
        it != theParticles->end(); ++it )
    {
      pxsq += (*it)->px() * (*it)->px();
      pysq += (*it)->py() * (*it)->py();
      pxpy += (*it)->px() * (*it)->py();
    }
  lambda1 = (pxsq+pysq)/2 + sqrt(4*pxpy*pxpy + ( pxsq - pysq ) * ( pxsq - pysq ) ) / 2;
  lambda2 = (pxsq+pysq)/2 - sqrt(4*pxpy*pxpy + ( pxsq - pysq ) * ( pxsq - pysq ) ) / 2;
  
  if (msgLvl(MSG::DEBUG))
    msg(MSG::DEBUG) << "Transverse Sphericity calculation gave "
		    << 2.0*lambda2 / ( lambda1 + lambda2 ) << " with eigenvalues of "
		    << lambda1 << ", " << lambda2 << endreq;
  
  eventShapeStore->insert( EventShapes::TRANSVERSESPHERICITY, 2.0*lambda2 / ( lambda1 + lambda2 ) );
  
  return StatusCode::SUCCESS;
}

