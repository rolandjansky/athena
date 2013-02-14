/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArBadChanFEBHasher_H
#define LArBadChanFEBHasher_H

#include "LArIdentifier/LArOnlineID.h"

class LArBadChanFEBHasher {
public:

  LArBadChanFEBHasher() : m_firstFEBid(0), m_shift(0), m_FEBShift(0),
			  m_maxHash(0), m_maxFEBHash(0), m_onlineID(0) {}

  LArBadChanFEBHasher( unsigned int firstFEB, unsigned int lastFEB,
		       unsigned int febShift, const LArOnlineID* onlineID) :
    m_firstFEBid(firstFEB), 
    m_shift( febShift - 7), // 7 bits shift to the left for mult. by 128
    m_FEBShift( febShift),
    m_onlineID( onlineID)
  {
    m_maxFEBHash = (((lastFEB - firstFEB) >> febShift) + 1);
    m_maxHash = m_maxFEBHash // range of FEBs
      * maxChannel();        // number of channels per FEB
  }

  unsigned int hash( const HWIdentifier& FEBid, int chan) const {
    return ((FEBid.get_identifier32().get_compact() - m_firstFEBid) >> m_shift) + chan;
  }

  unsigned int hash( const HWIdentifier& onlineID) const {
    return hash( m_onlineID->feb_Id( onlineID), m_onlineID->channel(onlineID));
  }

  // channel
  unsigned int hash( unsigned int febHash, unsigned int chan) const {
    return febHash * maxChannel() + chan;
  }

  unsigned int hashFEB( const HWIdentifier& FEBid) const {
    return ((FEBid.get_identifier32().get_compact() - m_firstFEBid) >> m_FEBShift);
  }

  unsigned int febHashByChan(  const HWIdentifier& chanID) const {
    return hashFEB( m_onlineID->feb_Id( chanID));
  }

  unsigned int channelInFeb( const HWIdentifier& chanID) const {
    return m_onlineID->channel(chanID);
  }

  unsigned int maxHash() const { return m_maxHash;}
  unsigned int maxFEBHash() const { return m_maxFEBHash;}
  unsigned int maxChannel() const {return 128;} // max # of channels per FEB

  bool initialized() const {return ((m_onlineID != 0) && (m_maxHash != 0));}

private:

  unsigned int m_firstFEBid;
  unsigned int m_shift;
  unsigned int m_FEBShift;
  unsigned int m_maxHash;
  unsigned int m_maxFEBHash;
  const LArOnlineID* m_onlineID;
};

#endif
