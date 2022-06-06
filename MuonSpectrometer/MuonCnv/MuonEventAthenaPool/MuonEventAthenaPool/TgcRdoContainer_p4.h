/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_TGCRDOCONTAINER_P4_H
#define MUON_TGCRDOCONTAINER_P4_H

#include <vector>
#include "MuonEventAthenaPool/TgcRdo_p4.h"


class TgcRdoContainer_p4
   : public std::vector<TgcRdo_p4>
{
public:
  // Default constructor
  TgcRdoContainer_p4 () {}
};

#endif

