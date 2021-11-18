/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArBadChannelTool/LArBadChanContainerHashed.h"

void LArBadChanContainerHashed::set( const BadChanVec& cont)
{
  if (m_febVec.empty()) init();

  // reset all channel information, but keep FEB status unchanged
  for ( unsigned int i=0; i< m_hasher.maxFEBHash(); i++) m_febVec[i].resetChannels();

  for (BadChanVec::const_iterator i=cont.begin(); i!=cont.end(); ++i) {
    HashType h = m_hasher.febHashByChan( i->first);
    m_febVec.at(h).addChannel( m_hasher.channelInFeb(i->first), i->second);
  }

  m_bcv = cont;
}

void LArBadChanContainerHashed::init() 
{

  //if (!m_hasher.initialized()) FIXME should throw an exception!

  m_febVec.resize( m_hasher.maxFEBHash());
}

void LArBadChanContainerHashed::setBadFEBs( const std::vector<BadFebEntry>& badFebs)
{
  if (m_febVec.empty()) init();

  for ( unsigned int i=0; i< m_hasher.maxFEBHash(); i++) m_febVec[i].resetBad();

  for (std::vector<BadFebEntry>::const_iterator i=badFebs.begin(); 
       i!=badFebs.end(); ++i) {
    m_febVec.at(  m_hasher.hashFEB(i->first)).setFebStatus(i->second);
  }
}
