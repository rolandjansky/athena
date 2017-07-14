//Dear emacs, this is -*-c++-*-
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRECCONDITIONS_LARBADCHANNELCONT_H
#define LARRECCONDITIONS_LARBADCHANNELCONT_H

#include <vector>
#include <algorithm>
#include "LArRecConditions/LArBadChannel.h"
#include "Identifier/HWIdentifier.h"

/**
 * @brief Conditions-Data class holding LAr Bad Channel information
 *
 * Uses internally a vector<pair<channelID,LArBadChannel> >
 * ordered by channel-id to speed up searching 
 * Possible optimization: use a hash-indexed bitset to tell if the channel 
 * is on the list. Avoid searching for 'good' channels which are the majority 
 * Downside: Need acess the LArOnline_ID helper class + channel -> hash conversion
 */

class LArBadChannelCont {
 public:
  typedef uint32_t ChanId_t;
  typedef std::pair<ChanId_t,LArBadChannel>    BadChanEntry;
  typedef std::vector<BadChanEntry>            BadChanVec;
  typedef typename BadChanVec::const_iterator  const_iterator;
  typedef typename BadChanVec::size_type       size_type;

public:

  
  /// Default Constructor
  LArBadChannelCont() {}
  
  /// Constructor with payload
  LArBadChannelCont( const BadChanVec& vec);

  /**@brief Add a channel to the list of bad channels 
   * @param channel HWIdenifier of the channel in question
   * @param stat Bad-Channel object describing the disease this channels suffers
   */
  void add(const HWIdentifier channel, const LArBadChannel stat);

  /**@brief Sort and purge the list of bad channels 
   * The list of bad channels get sorted by channel-ID
   * For channels appearing more than once, the bad-channel word 
   * gets merged by or'ing the problem-bits
   */
  void sort(); 
  
  /**@brief Query the status of a particular channel
   * This is the main client access method
   * @param HWIdentifer of the channel in question
   * @return LArBadChannel object describing the problem of the channel
   */
  LArBadChannel status(const HWIdentifier channel) const;


  ///Access to the begin iterator of the underlying vector
  const_iterator begin() const {return m_cont.begin();}

  ///Access to the end iterator of the underlying vector
  const_iterator end() const {return m_cont.end();}

  ///Number of known bad channels 
  size_type      size() const {return m_cont.size();}

  /// Deletes the contents and sets size to 0 (same as stl::vector<T>::clear())
  void clear() {m_cont.clear();}

private:
  ///Comparison functor for sorting and searching
  struct ChannelLess {
    bool operator()(const BadChanEntry& a, const BadChanEntry& b) const {
      return a.first < b.first;
    }
  };

  //The vector storing bad-channels
  BadChanVec m_cont;
};



inline LArBadChannel LArBadChannelCont::status(const HWIdentifier hwid) const {

  const ChanId_t channel=hwid.get_identifier32().get_compact();

  const_iterator i = 
    std::lower_bound( m_cont.begin(), m_cont.end(), BadChanEntry( channel,(LArBadChannel) 0),
		      ChannelLess());
  if (i != m_cont.end() && i->first == channel) {
    return i->second;
  }
  else {
    return LArBadChannel(0);
  }
}


#include "CLIDSvc/CLASS_DEF.h" 
CLASS_DEF(LArBadChannelCont,64272230,1)
#include "AthenaKernel/CondCont.h"
CLASS_DEF( CondCont<LArBadChannelCont>,144954956 , 1 )
#include "SGTools/BaseInfo.h"
SG_BASE( CondCont<LArBadChannelCont>, CondContBase );


#endif
