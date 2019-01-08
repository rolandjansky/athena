/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloEvent/CaloCellContainerVector.h"

CaloCellContainerVector::CaloCellContainerVector(SG::OwnershipPolicy ownPolicy ) :
  DataVector<CaloCellContainer>(ownPolicy)
  {
  }

CaloCellContainerVector::CaloCellContainerVector(size_t n, SG::OwnershipPolicy ownPolicy ) :
  CaloCellContainerVector (ownPolicy)
  {
    resize(n);
  }
