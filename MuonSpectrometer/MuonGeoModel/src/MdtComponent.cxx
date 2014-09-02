/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGeoModel/MdtComponent.h"
#include "MuonGeoModel/MYSQL.h"
#include "MuonGeoModel/Technology.h"
#include "MuonGeoModel/CHV_Technology.h"
#include "MuonGeoModel/CMI_Technology.h"
#include "MuonGeoModel/CRO_Technology.h"
#include "MuonGeoModel/LBI_Technology.h"

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
