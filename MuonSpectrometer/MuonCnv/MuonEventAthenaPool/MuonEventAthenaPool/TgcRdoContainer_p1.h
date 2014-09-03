/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_TGCRDOCONTAINER_P1_H
#define MUON_TGCRDOCONTAINER_P1_H

/*

Persistent represenation of a container of TgcRdo,
Author: Marcin Nowak
        CERN, February, 2006

*/

#include <vector>
#include "MuonEventAthenaPool/TgcRdo_p1.h"


class TgcRdoContainer_p1
   : public std::vector<TgcRdo_p1>
{
public:
  // Default constructor 
  TgcRdoContainer_p1 () {}
};

#endif




