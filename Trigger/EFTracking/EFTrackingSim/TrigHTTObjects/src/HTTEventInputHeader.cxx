/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHTTObjects/HTTEventInputHeader.h"
#include <iostream>

ClassImp(HTTEventInputHeader)

HTTEventInputHeader::~HTTEventInputHeader()
{
  reset();
}

void HTTEventInputHeader::reset()
{
  m_event.reset();
  m_optional.reset();
  m_Hits.clear();
}


std::ostream& operator<<(std::ostream& s, const HTTEventInputHeader& h) {
  s << "Event: " << h.event() << "\t"
    << "Optional: " << h.optional() << "\t"
    << "Nhits=" << h.nHits();

  s << "\n";

  const std::vector<HTTHit>& hits = h.hits();
  for (int j = 0; j < h.nHits(); j++) {
    s << " " << j << "  " << hits[j] << "\n";
    //    if ( (j+1)%5==0 ) s << "\n";
  }
  //s <<"\n";


  return s;
}

