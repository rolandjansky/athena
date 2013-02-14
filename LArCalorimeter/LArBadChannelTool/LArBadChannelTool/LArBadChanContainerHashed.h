/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArBadChanContainerHashed_H
#define LArBadChanContainerHashed_H

#include "LArRecConditions/LArBadChannel.h"
#include "LArRecConditions/LArBadFeb.h"
#include "LArBadChannelTool/LArBadChanFebState.h"
#include "LArBadChannelTool/LArBadChanFEBHasher.h"

class LArBadChanContainerHashed {
public:

  typedef std::pair<HWIdentifier,LArBadChannel>        BadChanEntry;
  typedef std::pair<HWIdentifier,LArBadFeb>            BadFebEntry;
  typedef std::vector<BadChanEntry>                    BadChanVec;
  typedef BadChanVec::const_iterator                   const_iterator;
  typedef BadChanVec::size_type                        size_type;

public:

  LArBadChanContainerHashed() {}

  explicit LArBadChanContainerHashed( const LArBadChanFEBHasher& h) : m_hasher(h) {}

  LArBadChanContainerHashed( const BadChanVec& vec) { set(vec);}

  LArBadChannel status( const HWIdentifier& channelID) const {
    return m_febVec.at( m_hasher.febHashByChan( channelID)).status( m_hasher.channelInFeb( channelID));
  }

  LArBadChannel status( const HWIdentifier& feb, int ch) const {
    return m_febVec.at( m_hasher.hashFEB( feb)).status( ch);
  }

  bool febBad( HWIdentifier febId) const {
    return m_febVec.at( m_hasher.hashFEB( febId)).bad();
  }

  bool febAllGood( HWIdentifier febId) const {
    return m_febVec.at( m_hasher.hashFEB( febId)).empty();
  }

  LArBadFeb febStatus( HWIdentifier febId) const {
    return m_febVec.at( m_hasher.hashFEB( febId)).febStatus();
  }

  const_iterator begin() const {return m_bcv.begin();}
  const_iterator end() const {return m_bcv.end();}
  size_type      size() const {return m_bcv.size();}

  void set( const BadChanVec& cont);

  void setBadFEBs( const std::vector<BadFebEntry>& badFebs);

  void setHasher( const LArBadChanFEBHasher& hasher) { m_hasher = hasher;}

private:

  typedef unsigned int         HashType;

  std::vector<LArBadChanFebState>   m_febVec;
  LArBadChanFEBHasher               m_hasher;
  BadChanVec                        m_bcv;

  void init();

};

#endif
