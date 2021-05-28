/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_TGCL1RDOCONTAINER_P1_H
#define MUON_TGCL1RDOCONTAINER_P1_H

#include <vector>
#include "MuonEventAthenaPool/TgcL1Rdo_p1.h"


class TgcL1RdoContainer_p1
   : public std::vector<TgcL1Rdo_p1>
{
public:
  // Default constructor
  TgcL1RdoContainer_p1 () {}
};

#endif

