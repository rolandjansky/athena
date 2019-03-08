/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArBadChannelState_H
#define LArBadChannelState_H

#include "Identifier/HWIdentifier.h"
#include "LArRecConditions/LArBadChannel.h"

#include <vector>
#include <string>


class LArBadChannelState {
 public:

  typedef std::pair<HWIdentifier, LArBadChannel>                BadChanEntry;

  enum {NCoolChannels=8};
  enum CaloPartEnum {EMB, EMEC, HEC, FCAL};
  enum PosNegEnum { NEG, POS};
  enum CoolChannelEnum { EMBA, EMBC, EMECA, EMECC, HECA, HECC, FCALA, FCALC, MAXCOOLCHAN};

  class CoolChannelData {
  public:

    typedef std::vector<BadChanEntry>       BadChanVec;

    /// Construct with bad channel data 
    CoolChannelData( CoolChannelEnum cc, const BadChanVec& entries) :
      m_number(cc), m_entries( entries), m_changed(false) {}

    /// Construct an empty CoolChannelData
    explicit CoolChannelData( CoolChannelEnum cc) :
    m_number(cc), m_entries(), m_changed(false) {}

    void reset() {
      m_entries.clear();
      m_changed = false;
    }

    /// the entry is assumed to belong to the cool channel and is not checked.
    /// Duplicate entries for the same channel may exist after this call
    void add( const BadChanEntry& e) {
      m_entries.push_back(e);
      m_changed = true;
    }

    void setFromDB( const BadChanVec& stateVec) {
      m_entries = stateVec;
      m_changed = false;  
    }

    void addFromDB( const BadChanVec& stateVec);

    void addUpdate( const BadChanVec& stateVec) {
      m_entries.insert( m_entries.end(), stateVec.begin(), stateVec.end());
      m_changed = true;  
    }

    void resolveDuplicateEntries();

    CoolChannelEnum number() const { return m_number;}

    bool changed() const { return m_changed;}

    BadChanVec::const_iterator begin() const {
      return m_entries.begin();
    }
    BadChanVec::iterator begin() {
      return m_entries.begin();
    }

    BadChanVec::const_iterator end() const {
      return m_entries.end();
    }
    BadChanVec::iterator end() {
      return m_entries.end();
    }

    BadChanVec::size_type size() const {
      return m_entries.size();
    }

    bool empty() const {
      return m_entries.empty();
    }

    const BadChanVec& stateVector() const { return m_entries;}

  private:
    CoolChannelEnum             m_number;
    BadChanVec                  m_entries;
    bool                        m_changed;
  };


  typedef CoolChannelData::BadChanVec         BadChanVec;

  LArBadChannelState();

  int nCoolChannels() const { return m_DataVec.size();}

  /// Returns true if all cool channels are empty
  bool empty() const {
    for (std::vector< CoolChannelData>::const_iterator i=m_DataVec.begin();
	 i != m_DataVec.end(); i++) if (!i->empty()) return false;
    return true;
  }

  /// Reset to an empty state, erasing all data in all cool channels
  void reset();

  /// Add a complete cool channel from DB, with duplicate removal
  void addFromDB( const BadChanVec& stateVec, 
		  CoolChannelEnum chan);

  /// Add a bad channel entry to a specified cool channel data
  void add( const BadChanEntry& entry, CoolChannelEnum chan);

  /// Set cool channel from vector<BadChanEntry>, for example when reading from DB.
  void setFromDB( const BadChanVec& stateVec, 
		  CoolChannelEnum chan);

  /// Add updates, e.g. from an ASCII file or interactively.
  /// The new entries are appended to the existing container.
  void addUpdate( const BadChanVec& stateVec, 
		  CoolChannelEnum chan) {
    m_DataVec[chan].addUpdate( stateVec);
  }

  const CoolChannelData& coolChannel( CoolChannelEnum chan) const {
    return m_DataVec.at(chan);
  }

  static std::string coolChannelName( CoolChannelEnum chan);

  static CoolChannelEnum coolChannelEnum( CaloPartEnum part, PosNegEnum side) {
    return static_cast<CoolChannelEnum>( 2 * static_cast<unsigned int>(part) +
					 1 - static_cast<unsigned int>(side));
  }

  static int barrelEndcap(  CoolChannelEnum chan) {
    if (chan == EMBA || chan == EMBC) return 0;
    else return 1;
  }

  static CaloPartEnum caloPart( CoolChannelEnum chan) {
    return static_cast<CaloPartEnum>(static_cast<int>(chan)/2);
  }

  static int posNeg( CoolChannelEnum chan) {
    return (static_cast<int>(chan)+1) % 2;
  }

  static void mergeFlags( BadChanEntry& a, const BadChanEntry& b) {
    a.second = LArBadChannel( a.second.packedData() | b.second.packedData());
  }

  //void CurrentState
 private:

  std::vector< CoolChannelData> m_DataVec;

  // Non-const access to channel data
  CoolChannelData& coolChannel( int i) {
    return m_DataVec.at( static_cast<CoolChannelEnum>(i));
  }

};

#endif
