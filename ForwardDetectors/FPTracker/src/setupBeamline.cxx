/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FPTracker/setupBeamline.h"
#include "FPTracker/STLHelpers.h"
#include "FPTracker/ConfigData.h"
#include "FPTracker/magnetSet.h"
#include "FPTracker/readConfigData.h"
#include "FPTracker/Magnet.h"
#include "FPTracker/EmptySpaceElement.h"
#include "FPTracker/CollimatorData.h"
#include "FPTracker/Collimator.h"
#include "FPTracker/collSet.h"
#include "FPTracker/Plane.h"
#include "FPTracker/Beamline.h"
#include "FPTracker/beamlineXPosition.h"
#include "FPTracker/IBeamElement.h"
#include "FPTracker/Particle.h"
#include <boost/shared_ptr.hpp>
#include <algorithm>
//#include <cassert>
#include <iostream>

namespace FPTracker{

  Beamline setupBeamline(
			 const ConfigData& configData,
			 const Side& side,
			 int magver,
			 boost::shared_ptr< std::ifstream> magfile 
			 )
  {
    
    Magnet::Container_t   magnets    = magnetSet(configData, side, magver, magfile);
    Plane::ConstPtr_t     endPlane( new Plane(configData.endMarker, side) );
    CollimatorData  collimatorData(configData);
    Collimator::Container_t collimators = collSet(collimatorData, side);

    IBeamElement::List_t elements;
    //elements.reserve( 2*(magnets.size()+collimators.size()+planes.size()) );
    elements.assign(magnets.begin(), magnets.end());
    elements.push_back(endPlane);
    elements.insert(elements.end(), collimators.begin(), collimators.end());
    //std::sort(elements.begin(), elements.end(), absZGreater<IBeamElement>);
    elements.sort(absZGreater< IBeamElement::ConstPtr_t >);


    IBeamElement::ListIter_t current  = elements.begin();
    IBeamElement::ListIter_t last     = elements.end();

    elements.push_front
      (
       EmptySpaceElement::ConstPtr_t(
				     new EmptySpaceElement( 
							   0., 
							   (*current)->frontFace(),
							   beamlineXPosition(0.),
							   (*current)->side()
							   ) 
				     )
       );
    ++current;

    for(; current != last; ++current){
      IBeamElement::ListIter_t previous = current;
      --previous;
      elements.insert(current, 
		      EmptySpaceElement::ConstPtr_t(
						    new  EmptySpaceElement(
									   *previous, 
									   *current
									   )
						    )
		      );
    }
      
    Beamline beamline(elements.begin(), elements.end());

    double pbeam = configData.pbeam0;
    boost::shared_ptr<IParticle> ip = (side ==  beam1) ? 
      boost::shared_ptr<IParticle>(new Particle(0., 0., 0., 0., 0.,     pbeam) ):
      boost::shared_ptr<IParticle>(new Particle(0., 0., 0., 0., 0., -1.*pbeam) );
    beamline.calibrate( *ip );


    return beamline;
  }
}
