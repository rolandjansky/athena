/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MdtComponent_H
#define MdtComponent_H

#include "MuonGeoModel/StandardComponent.h"

namespace MuonGM {

    class MdtComponent : public StandardComponent {
      public:
        MdtComponent();
        MdtComponent(const MdtComponent &c);

        double tubelenStepSize;  // step size for tube length in endcap chambers
        double cutoutTubeXShift; // distance along tube length to shift tube center

      private:
        MdtComponent &operator=(const MdtComponent &right);
    };
} // namespace MuonGM

#endif
