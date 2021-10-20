/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MMSpacer_Technology_H
#define MMSpacer_Technology_H

#include "MuonGeoModel/Technology.h"

#include <vector>
namespace MuonGM {

    // Description class to build MicroMegas spacers

    class MMSpacer_Technology : public Technology {
      public:
        // constructor
        inline MMSpacer_Technology(MYSQL& mysql, const std::string& s);
        inline double Thickness() const;

        int lowZCutOuts;
        double lowZCutOutWidth;
        double lowZCutOutDZ;
        int highZCutOuts;
        double highZCutOutWidth;
        double highZCutOutDZ;
    };

    MMSpacer_Technology::MMSpacer_Technology(MYSQL& mysql, const std::string& s)
        : Technology(mysql, s), lowZCutOuts(0), lowZCutOutWidth(0.), lowZCutOutDZ(0.), highZCutOuts(0), highZCutOutWidth(0.), highZCutOutDZ(0.) {}

    double MMSpacer_Technology::Thickness() const { return thickness; }

} // namespace MuonGM

#endif
