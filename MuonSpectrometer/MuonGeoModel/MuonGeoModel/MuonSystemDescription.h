/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonSystemDescription_H
#define MuonSystemDescription_H

#include <string>

namespace MuonGM {

    class MuonSystemDescription {
      public:
        double barrelInnerRadius; // Inner radius behind the idet
        double innerRadius;       // Inner radius at the beam pipe
        double outerRadius;
        double endcapFrontFace; // Z at the endcap front face
        double length;
        double barreLength;
        double barrelInterRadius;

        double extraZ;
        double extraR;

        std::string amdb;

        MuonSystemDescription(std::string n);

      private:
        std::string m_name;
    };
} // namespace MuonGM

#endif
