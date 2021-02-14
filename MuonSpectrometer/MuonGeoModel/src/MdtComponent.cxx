/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGeoModel/MdtComponent.h"

namespace MuonGM {

MdtComponent::MdtComponent():StandardComponent()
{
  tubelenStepSize=0.;
  cutoutTubeXShift=0.;
}

MdtComponent::MdtComponent(const MdtComponent& c):StandardComponent(c)
{
  tubelenStepSize = c.tubelenStepSize;
  cutoutTubeXShift = c.cutoutTubeXShift;
}

} // namespace MuonGM
