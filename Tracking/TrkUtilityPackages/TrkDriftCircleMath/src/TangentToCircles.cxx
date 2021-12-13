/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkDriftCircleMath/TangentToCircles.h"

#include <iostream>

namespace TrkDriftCircleMath {

    TangentToCircles::LineVec TangentToCircles::tangentLines(const DriftCircle& dc1, const DriftCircle& dc2) {
        LineVec lines;
        lines.reserve(4);

        double DeltaX = dc2.x() - dc1.x();
        double DeltaY = dc2.y() - dc1.y();
        double DistanceOfCenters = std::hypot(DeltaX, DeltaY);
        if (DistanceOfCenters == 0.) {
            std::cout << " TangentToCircles::tangentLines >>> ERROR same tube on input " << std::endl;
            return lines;
        }
        double Alpha0 = std::atan2(DeltaY, DeltaX);

        // Case of 0 drift distances, only 1 line
        if (dc1.r() == 0. && dc2.r() == 0.) {
            lines.emplace_back(dc1.position(), Alpha0);
            return lines;
        }

        // Here are the first 2 "inner" lines ....
        double RSum = dc1.r() + dc2.r();

        double ratio = RSum / DistanceOfCenters;
        if (std::abs(ratio) > 1.) return lines;

        double Alpha1 = std::asin(ratio);

        double line_phi = Alpha0 + Alpha1;
        LocVec2D pos1(dc1.x() + dc1.r() * std::sin(line_phi), dc1.y() - dc1.r() * std::cos(line_phi));
        lines.emplace_back(pos1, line_phi);

        line_phi = Alpha0 - Alpha1;
        LocVec2D pos2(dc1.x() - dc1.r() * std::sin(line_phi), dc1.y() + dc1.r() * std::cos(line_phi));
        lines.emplace_back(pos2, line_phi);

        // Case where one of the drifts is 0 ==> Only two lines
        if (dc1.r() == 0. || dc2.r() == 0.) return lines;

        // ... and here are the other 2 "outer" lines
        double DeltaR = std::abs(dc2.r() - dc1.r());

        ratio = DeltaR / DistanceOfCenters;
        if (std::abs(ratio) > 1.) return lines;

        double Alpha2 = asin(ratio);

        if (dc1.r() < dc2.r()) {
            line_phi = Alpha0 + Alpha2;
            LocVec2D pos3(dc1.x() - dc1.r() * sin(line_phi), dc1.y() + dc1.r() * cos(line_phi));
            lines.emplace_back(pos3, line_phi);

            line_phi = Alpha0 - Alpha2;
            LocVec2D pos4(dc1.x() + dc1.r() * std::sin(line_phi), dc1.y() - dc1.r() * std::cos(line_phi));
            lines.emplace_back(pos4, line_phi);

        } else {
            line_phi = Alpha0 + Alpha2;
            LocVec2D pos3(dc1.x() + dc1.r() * std::sin(line_phi), dc1.y() - dc1.r() * std::cos(line_phi));
            lines.emplace_back(pos3, line_phi);

            line_phi = Alpha0 - Alpha2;
            LocVec2D pos4(dc1.x() - dc1.r() * std::sin(line_phi), dc1.y() + dc1.r() * std::cos(line_phi));
            lines.emplace_back(pos4, line_phi);
        }
        return lines;
    }

}  // namespace TrkDriftCircleMath
