/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_TGCRDOCONTAINER_P2_H
#define MUON_TGCRDOCONTAINER_P2_H

/*

Persistent represenation of a container of TgcRdo, version 2
Author: Zvi Tarem
        CERN, August, 2006

*/

#include <vector>
#include "MuonEventAthenaPool/TgcRdo_p2.h"


class TgcRdoContainer_p2
   : public std::vector<TgcRdo_p2>
{
public:
  // Default constructor
  TgcRdoContainer_p2 () {}
};

#endif

