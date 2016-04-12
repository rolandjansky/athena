/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloEvent/CaloClusterCellLinkContainer.h"


CaloClusterCellLinkContainer::CaloClusterCellLinkContainer() :
  DataVector<CaloClusterCellLink>()
{}

CaloClusterCellLinkContainer::~CaloClusterCellLinkContainer() {}

/**@brief Prepare for writing.
 */
void CaloClusterCellLinkContainer::toPersistent()
{
  for (auto p : *this)
    p->toPersistent();
}

CaloClusterCellLinkContainer::CaloClusterCellLinkContainer(CaloClusterCellLinkContainer::iterator it1,
							   CaloClusterCellLinkContainer::iterator it2,
							   SG::OwnershipPolicy policy) :
  DataVector<CaloClusterCellLink>(it1,it2,policy) 
{}

