/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include "CaloEvent/CaloCellLink.h"

#include <algorithm>

CaloCellLink::CaloCellLink() : Navigable<CaloCellContainer,double>()
{ }

CaloCellLink::CaloCellLink(const CaloCellLink* pLink)
  : Navigable<CaloCellContainer,double>()
{
  this->copyStore(pLink);
}

CaloCellLink::CaloCellLink(const CaloCellLink& rLink)
  : INavigable (rLink),
    Navigable<CaloCellContainer,double>(rLink)
{
  this->copyStore(&rLink);
}

CaloCellLink& CaloCellLink::operator=(const CaloCellLink& rLink)
{
  if (this != &rLink) {
    this->removeAll();
    this->copyStore(&rLink);
  }
  return *this;
}

CaloCellLink::~CaloCellLink()
{ }

void CaloCellLink::copyStore(const CaloCellLink* pLink)
{
  cell_iterator fCell = pLink->begin();
  cell_iterator lCell = pLink->end();
  for ( ; fCell != lCell; fCell++ )
    {
      this->insertElement(pLink->getContainer(fCell),*fCell,
			  pLink->getParameter(fCell));
    }
}
