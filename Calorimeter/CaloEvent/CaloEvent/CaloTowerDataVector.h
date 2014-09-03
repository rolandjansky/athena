/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOEVENT_CALOTOWERDATAVECTOR_H
#define CALOEVENT_CALOTOWERDATAVECTOR_H

#include "DataModel/DataVector.h"
#include "CaloEvent/CaloTower.h"
#include "NavFourMom/INavigable4MomentumCollection.h"

DATAVECTOR_VIRTBASES1 (CaloTower, INavigable4Momentum);

#endif
