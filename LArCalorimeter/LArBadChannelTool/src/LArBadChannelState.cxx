/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArBadChannelTool/LArBadChannelState.h"

#include <set>

LArBadChannelState::LArBadChannelState()
{
  for ( unsigned int i=0; i<NCoolChannels; i++) {
    m_DataVec.push_back( CoolChannelData( static_cast<CoolChannelEnum>(i)));
  }
}

void LArBadChannelState::add( const BadChanEntry& entry, CoolChannelEnum chan)
{
  m_DataVec[chan].add(entry);
}

void LArBadChannelState::setFromDB( const std::vector<BadChanEntry>& stateVec, 
				    CoolChannelEnum chan)
{
  m_DataVec[chan].setFromDB( stateVec);
}

void LArBadChannelState::addFromDB( const std::vector<BadChanEntry>& stateVec, 
				    CoolChannelEnum chan)
{
  m_DataVec[chan].addFromDB( stateVec);
}

void LArBadChannelState::reset()
{
  for ( unsigned int i=0; i<NCoolChannels; i++) {
    m_DataVec[i].reset();
  }
}

std::string LArBadChannelState::coolChannelName( CoolChannelEnum chan)
{
  switch( chan) {
  case EMBA:
    return "EMBA";
    break;
  case EMBC:
    return "EMBC";
    break;
  case EMECA:
    return "EMECA";
    break;
  case EMECC:
    return "EMECC";
    break;
  case HECA:
    return "HECA";
    break;
  case HECC:
    return "HECC";
    break;
  case FCALA:
    return "FCALA";
    break;
  case FCALC:
    return "FCALC";
    break;
  case MAXCOOLCHAN:
    break;
  }
  return "Unknown";
}

void LArBadChannelState::CoolChannelData::addFromDB( const std::vector<BadChanEntry>& stateVec)
{
  m_entries.insert( m_entries.end(), stateVec.begin(), stateVec.end());
  resolveDuplicateEntries();
  //sort( m_entries.begin(), m_entries.end())
  m_changed = false;  
}

void LArBadChannelState::CoolChannelData::resolveDuplicateEntries()
{
  //  using namespace std;
  typedef std::vector<BadChanEntry>::iterator   Iter;

  std::multiset<Iter> toDelete;
  for (Iter i = m_entries.begin(); i != m_entries.end(); i++) {
    for (Iter j = i+1; j != m_entries.end(); j++) {    
      if (i->first == j->first) {
	mergeFlags( *i, *j);  // j's flags are added to i's flags
	toDelete.insert(j);
      }
    }
  } 

  // We make a copy of the vector to avoid erasing elements
  std::vector<BadChanEntry> tmp;
  tmp.reserve( m_entries.size() - toDelete.size());
  for (Iter i = m_entries.begin(); i != m_entries.end(); i++) {
    if (toDelete.find(i) == toDelete.end()) {
      tmp.push_back(*i); // not one of the duplicates
    }
  }

  m_entries.swap( tmp);
}
