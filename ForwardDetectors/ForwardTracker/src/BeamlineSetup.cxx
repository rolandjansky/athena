/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ForwardTracker/Beamline.h"
#include "ForwardTracker/BeamlineSetup.h"
#include "ForwardTracker/CollimatorSet.h"
#include "ForwardTracker/MagnetSet.h"
#include "ForwardTracker/Plane.h"
#include "ForwardTracker/Spacer.h"
#include "ForwardTracker/STLHelpers.h"
#include "ForwardTracker/TwissFile.h"

namespace ForwardTracker {
  
  Beamline BeamlineSetup(const ConfigData& cData, const Side& side) {

    Magnet::Container_t     magnets     = MagnetSet    (cData, side);
    Collimator::Container_t collimators = CollimatorSet(cData, side);
    Plane::ConstPtr_t endPlane(new Plane(cData.endMarker, side));
    
    IBeamElement::List_t elements;
    
    elements.assign(magnets.begin(), magnets.end());
    elements.insert(elements.end(), collimators.begin(), collimators.end());
    elements.push_back(endPlane);
    elements.sort(absZGreater<IBeamElement::ConstPtr_t>);

    IBeamElement::ListIter_t current = elements.begin();
    IBeamElement::ListIter_t last    = elements.end();
    
    elements.push_front(Spacer::ConstPtr_t(new Spacer(0., (*current)->frontFace(), (*current)->side())));
    
    ++current;
    
    for(; current != last; ++current) {

      IBeamElement::ListIter_t previous = current;
      
      --previous;
      
      elements.insert(current, Spacer::ConstPtr_t(new Spacer(*previous, *current)));
    }
    
    Beamline beamline(elements.begin(), elements.end());
    
    return beamline;
  }
}
