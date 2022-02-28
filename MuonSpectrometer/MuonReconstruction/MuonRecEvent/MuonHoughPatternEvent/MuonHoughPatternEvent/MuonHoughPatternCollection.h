/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHOUGHPATTERNEVENT_MUONHOUGHPATTERNCOLLECTION_H
#define MUONHOUGHPATTERNEVENT_MUONHOUGHPATTERNCOLLECTION_H

#include "MuonHoughPatternEvent/MuonHoughPattern.h"

/**
This typedef represents a collection and container of MuonHoughPattern objects.
*/
using MuonHoughPatternCollection = std::vector<std::unique_ptr<MuonHoughPattern>>;
using MuonHoughPatternContainer = std::vector<MuonHoughPatternCollection>;
using MuonHoughPatternContainerShip = std::vector<MuonHoughPatternContainer>;

#endif  // MUONHOUGHPATTERNEVENT_MUONHOUGHPATTERNCOLLECTION_H
