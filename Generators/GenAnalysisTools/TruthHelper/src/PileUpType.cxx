/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TruthHelper/PileUpType.h"


PileUpType::PileUpType(const McEventCollection * collection) {
  m_particleList = collection;
}


HepMC::GenEvent::particle_const_iterator PileUpType::signal_particle_begin() const {
   HepMC::GenEvent::particle_const_iterator it;
   unsigned int colSize = m_particleList->size();
   if ( colSize > 0 )  it = (m_particleList->at(0))->particles_begin();
   return it;
}


HepMC::GenEvent::particle_const_iterator PileUpType::signal_particle_end() const {
   HepMC::GenEvent::particle_const_iterator it;
   unsigned int colSize = m_particleList->size();
   if ( colSize > 0 )  it = (m_particleList->at(0))->particles_end();
   return it;
}


const HepMC::GenEvent * PileUpType::signal_mc_event() const {
   if ( m_particleList->size() > 0 ) return m_particleList->at(0);
   else return NULL;
}


McEventCollection::const_iterator PileUpType::in_time_minimum_bias_event_begin() const {
   McEventCollection::const_iterator ibegin = m_particleList->begin();
   McEventCollection::const_iterator iend = m_particleList->end();
   if ( (ibegin!=iend) && (m_particleList->size()>1) ) {
     ibegin++;
     return ibegin;
   }
   else return iend;
}


McEventCollection::const_iterator PileUpType::in_time_minimum_bias_event_end() const {
   McEventCollection::const_iterator ibegin = m_particleList->begin();
   McEventCollection::const_iterator iend = m_particleList->end();
   for ( ; ibegin != iend; ++ibegin ) {
     if ( (*ibegin)->event_number() == -1 && (*ibegin)->signal_process_id() == 0 ) break;
   }
   return ibegin;
}


McEventCollection::const_iterator PileUpType::signal_and_in_time_minimum_bias_event_begin() const {
  return m_particleList->begin();
}


McEventCollection::const_iterator PileUpType::signal_and_in_time_minimum_bias_event_end() const {
   return this->in_time_minimum_bias_event_end();
}


McEventCollection::const_iterator PileUpType::in_time_event_begin() const {
   return m_particleList->begin();
}


McEventCollection::const_iterator PileUpType::in_time_event_end() const {
   return this->in_time_minimum_bias_event_end(); //FIXME - we want ALL the in-time McEvents
}
