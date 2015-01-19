/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_TGCRDOCONTAINER_P3_H
#define MUON_TGCRDOCONTAINER_P3_H

#include <vector>
#include "MuonEventAthenaPool/TgcRdo_p3.h"


class TgcRdoContainer_p3
   : public std::vector<TgcRdo_p3>
{
public:
  // Default constructor
  TgcRdoContainer_p3 () {}
};

#endif

