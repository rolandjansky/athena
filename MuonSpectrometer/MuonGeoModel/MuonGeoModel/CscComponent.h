/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CscComponent_H
#define CscComponent_H

#include "MuonGeoModel/StandardComponent.h"
namespace MuonGM {

    class CscComponent : public StandardComponent {
      public:
        double maxwdy; // length from bottom to the max width of the CSL
        // for CSC it is = dy
    };
} // namespace MuonGM

#endif
