/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CbmComponent_H
#define CbmComponent_H

#include "MuonGeoModel/StandardComponent.h"

namespace MuonGM {

    class CbmComponent : public StandardComponent {

      public:
        double lb_height; // height of LB long beam
        double lb_width;  // width
        double hole_pos1; // location of 1st hole in cross beam for LB
        double hole_pos2; // location of 2nd hole in cross beam for LB
    };

} // namespace MuonGM

#endif
