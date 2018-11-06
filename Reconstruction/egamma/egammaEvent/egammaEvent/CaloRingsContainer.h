/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAEVENT_CALORINGSCONTAINER_H
#define EGAMMAEVENT_CALORINGSCONTAINER_H

// INCLUDE HEADER FILES:

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "egammaEvent/CaloRings.h"

class CaloRingsContainer : public DataVector<CaloRings> 
{
 public:
  virtual ~CaloRingsContainer() {};

  /** Main constructor */
  CaloRingsContainer(SG::OwnershipPolicy ownPolicy=SG::OWN_ELEMENTS ) ;
};

SG_BASE(CaloRingsContainer, DataVector<CaloRings>);
CLASS_DEF(CaloRingsContainer, 1253285755, 1)

#endif

