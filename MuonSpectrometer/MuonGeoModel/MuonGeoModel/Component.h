/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Component_H
#define Component_H
#include <string>

namespace MuonGM {

    class Component {
      public:
        Component();
        Component(const Component &c) = default;
        Component &operator=(const Component &c) = default;
        virtual ~Component(){};
        double GetThickness() const;
        std::string name;
        double dx1;
        double dx2;
        double dy;
    };
} // namespace MuonGM

#endif
