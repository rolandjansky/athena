/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MMSpacer_Technology_H
#define MMSpacer_Technology_H

#include "MuonGeoModel/Technology.h"

#include <vector>
namespace MuonGM {

    // Description class to build MicroMegas spacers

    class MMSpacer_Technology : public Technology {
      public:
        double thickness;

        // constructor
        inline MMSpacer_Technology(std::string s);
        inline double Thickness();

        int lowZCutOuts;
        double lowZCutOutWidth;
        double lowZCutOutDZ;
        int highZCutOuts;
        double highZCutOutWidth;
        double highZCutOutDZ;
    };

    MMSpacer_Technology::MMSpacer_Technology(std::string s)
        : Technology(s), thickness(0.), lowZCutOuts(0), lowZCutOutWidth(0.), lowZCutOutDZ(0.), highZCutOuts(0), highZCutOutWidth(0.), highZCutOutDZ(0.) {}

    double MMSpacer_Technology::Thickness() { return thickness; }

} // namespace MuonGM

#endif
