/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TgcComponent_H
#define TgcComponent_H

#include "MuonGeoModel/StandardComponent.h"
#include <string>
#include <iostream>

namespace MuonGM {

class TgcComponent: public StandardComponent {

public:
   TgcComponent();
   friend std::ostream & operator << (std::ostream& os, const TgcComponent & c);
};

} // namespace MuonGM

#endif
