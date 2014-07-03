/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <vector>
#include "AthLinks/ElementLink.h"

#include "xAODBase/IParticleContainer.h"

#include "xAODPFlow/PFOContainer.h"

namespace xAOD{

static void convertLinks(std::vector<ElementLink<PFOContainer> >& pfoContainer, std::vector<ElementLink<IParticleContainer> >& particleContainer){

  std::vector<ElementLink<PFOContainer>>::iterator firstPFO = pfoContainer.begin();
  std::vector<ElementLink<PFOContainer>>::iterator lastPFO = pfoContainer.end();

  for (; firstPFO != lastPFO; ++firstPFO){
    ElementLink<PFOContainer> thePFOLink = *firstPFO;
    thePFOLink.toPersistent();
    ElementLink< xAOD::IParticleContainer > myLink;
    myLink.resetWithKeyAndIndex( thePFOLink.persKey(), thePFOLink.persIndex() );
    particleContainer.push_back(myLink);
  }

}

}
