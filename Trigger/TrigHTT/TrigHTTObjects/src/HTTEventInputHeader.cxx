/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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


std::ostream& operator<<(std::ostream& s, const HTTEventInputHeader& h)
{
    s << "Event: "    << h.event()<<"\t"
      << "Optional: " << h.optional() << "\t"
      << "Nhits="     << h.nHits() ;
    return s;
}

