/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorObjects/McEventCollection.h"

namespace {
  class MatchEventNumber {
  public:
    MatchEventNumber(int eventNumber) : m_evtNumber(eventNumber) {}
    bool operator()(const HepMC::GenEvent* pGE) {
      return (m_evtNumber == pGE->event_number());
    }
  private:
    int m_evtNumber;
  };
  class FilledMatchEventNumber {
  public:
    FilledMatchEventNumber(int eventNumber) : m_evtNumber(eventNumber) {}
    bool operator()(const HepMC::GenEvent* pGE) {
#ifdef HEPMC3
      return ((m_evtNumber == pGE->event_number()) && !(pGE->vertices().empty()) );
#else
      return ((m_evtNumber == pGE->event_number()) && !(pGE->vertices_empty()) );
#endif
    }
  private:
    int m_evtNumber;
  };
}


const HepMC::GenEvent*
McEventCollection::find(int eventNumber) const {
  const_iterator i(std::find_if(begin(), end(), FilledMatchEventNumber(eventNumber)));
  if (end() == i)
    {
      //Didn't find a filled copy, so checking for an empty one.
      return (end() != (i=std::find_if(begin(), end(), MatchEventNumber(eventNumber)))) ? *i : 0 ;

    }
  return *i;


}


template class ClassName<McEventCollection>;
