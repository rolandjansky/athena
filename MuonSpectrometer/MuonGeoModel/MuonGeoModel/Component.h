/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Component_H
#define Component_H
#include <string>

namespace MuonGM {

    class Component {
      public:
        Component();
        Component(const Component &c);
        Component &operator=(const Component &c);
        virtual ~Component(){};
        double GetThickness() const;
        std::string name;
        double dx1;
        double dx2;
        double dy;
    };
} // namespace MuonGM

#endif
