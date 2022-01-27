/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkDriftCircleMath/RotPhi.h"

TrkDriftCircleMath::LocVec2D operator*(const TrkDriftCircleMath::RotPhi& rot, const TrkDriftCircleMath::LocVec2D& lv) {
    return {rot.cosphi() * lv.x() + rot.sinphi() * lv.y(), -rot.sinphi() * lv.x() + rot.cosphi() * lv.y()};
}
