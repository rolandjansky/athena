
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
