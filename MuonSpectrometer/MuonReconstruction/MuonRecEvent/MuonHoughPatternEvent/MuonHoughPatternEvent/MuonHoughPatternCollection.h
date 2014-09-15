/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHOUGHPATTERNEVENT_MUONHOUGHPATTERNCOLLECTION_H
#define MUONHOUGHPATTERNEVENT_MUONHOUGHPATTERNCOLLECTION_H

#include "MuonHoughPatternEvent/MuonHoughPattern.h"

/**
This typedef represents a collection and container of MuonHoughPattern objects. 
*/
typedef std::vector<MuonHoughPattern*> MuonHoughPatternCollection;

typedef std::vector <MuonHoughPatternCollection> MuonHoughPatternContainer;

typedef std::vector <MuonHoughPatternContainer> MuonHoughPatternContainerShip;

#endif // MUONHOUGHPATTERNEVENT_MUONHOUGHPATTERNCOLLECTION_H
