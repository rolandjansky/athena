/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHTTObjects/HTTLogicalEventInputHeader.h"


ClassImp(HTTLogicalEventInputHeader)


void HTTLogicalEventInputHeader::reset()
{
  m_event.reset();
  m_optional.reset();
  m_towers.clear();
}


std::ostream& operator<<(std::ostream& s, const HTTLogicalEventInputHeader& h)
{
  s << "Event: " << h.event() << "\t"
    << "Optional: " << h.optional() << "\t"
    << "NTowers: " << h.nTowers() << "\n";

  const std::vector<HTTTowerInputHeader>& towers = h.towers();
  for (int j = 0; j < h.nTowers(); j++)
  {
    s << " " << j << "  " << towers[j] << "\n";
  }
  s << std::endl;

  return s;
}

