/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArBadChanFebState_H
#define LArBadChanFebState_H

#include "LArRecConditions/LArBadChannel.h"
#include "LArRecConditions/LArBadFeb.h"

/**
   A class that stores the bad channels of a single FEB
 */

class LArBadChanFebState {
public:

  LArBadChanFebState() : m_status() {}

  void resetChannels() { 
    if (m_vec.capacity() != 0) std::vector<LArBadChannel>().swap(m_vec); // clear() and free the memory
  }

  // void setBad() {m_bad = true;} // replaced by setFebStatus
  void setFebStatus( LArBadFeb s) {m_status = s;}
  void resetBad() {m_status = LArBadFeb();}

  /// Returns true if the entire FEB is missing
  bool bad() const {return !m_status.good();}

  /// Returns the FEB state
  LArBadFeb febStatus() const {return m_status;}

  /// Returns true if there are no bad channels in the FEB 
  bool empty() const {return m_vec.empty();}

  void addChannel( unsigned int chan, LArBadChannel chanState) {
    // FIXME 128 (number of channels per FEB) is a hard-wired constant here
    if (m_vec.size() != 128) m_vec.resize(128);
    m_vec.at(chan) = chanState;
  }

  /// Returns an OR of the channel status and the missing FEB status
  LArBadChannel status( unsigned int chan) const {
    static LArBadChannel goodState;
    static LArBadChannel badState = setBadState(); // initialized only at first call

    if (empty()) {
      if (bad()) return badState;
      else       return goodState;
    }
    else {
      if (!bad()) return m_vec.at(chan);
      else return LArBadChannel( m_vec.at(chan).packedData() | badState.packedData());
    }

    // does not return channel info in case the FEB is missing
    // return ( bad() ? badState : ( m_vec.empty() ? goodState : m_vec.at(chan)));
  }

private:

  LArBadFeb                  m_status;
  std::vector<LArBadChannel> m_vec;

  LArBadChannel setBadState() const {
    LArBadChanBitPacking packing;
    LArBadChannel bs;
    packing.setBit( LArBadChanBitPacking::missingFEBBit, bs);
    return bs;
  }

};

#endif
