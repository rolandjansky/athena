/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "MuonSimData/MuonSimDataCollection.h"

MuonSimDataCollection::MuonSimDataCollection() 
{}

Identifier MuonSimDataCollection::id (size_t i) const
{
  auto it = begin();
  std::advance (it, i);
  return it->first;
}

const MuonSimData* MuonSimDataCollection::data (size_t i) const
{
  auto it = begin();
  std::advance (it, i);
  return &it->second;
}


