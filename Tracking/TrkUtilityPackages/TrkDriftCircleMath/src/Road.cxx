/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkDriftCircleMath/Road.h"

namespace TrkDriftCircleMath {

    Road::Road(const LocVec2D& roadPos, double roadAngle, double chamberAngle, double roadAngularWidth) :
        m_pos(roadPos), m_angle(roadAngle), m_chamberAngle(chamberAngle), m_width(roadAngularWidth) {}

    

}  // namespace TrkDriftCircleMath
