/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SupComponent_H
#define SupComponent_H

#include "MuonGeoModel/StandardComponent.h"

namespace MuonGM {

class SupComponent: public StandardComponent {
public:
    double xAMDB0() const;
    double yAMDB0() const;
    double zAMDB0()const ;
    double bottomsizewrtAMDB0() const ;
    double    topsizewrtAMDB0() const ;
};
} // namespace MuonGM

#endif
