//Dear emacs, this is -*-c++-*-
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRECCONDITIONS_LARBADCHANNELCONT_H
#define LARRECCONDITIONS_LARBADCHANNELCONT_H

#include <vector>
#include <algorithm>
#include "LArRecConditions/LArBadChannel.h"
#include "LArRecConditions/LArBadFeb.h"
#include "Identifier/HWIdentifier.h"

/**
 * @brief Conditions-Data class holding LAr Bad Channel or Bad Feb information
 *
 * Uses internally a vector<pair<channelID,LArBadChannel> >
 * ordered by channel-id to speed up searching 
 * Possible optimization: use a hash-indexed bitset to tell if the channel 
 * is on the list. Avoid searching for 'good' channels which are the majority 
 * Downside: Need acess the LArOnline_ID helper class for channel to hash conversion
 * and a template specialization for the different hashes of FEBs and channels
 * Or even keep a hash-indexed vector LArBadChannels for every channel, even good ones
 */

template<class LArBC_t>
class LArBadXCont {
 public:
  typedef uint32_t ChanId_t;
  typedef std::pair<ChanId_t,LArBC_t>          BadChanEntry;
  typedef std::vector<BadChanEntry>            BadChanVec;
  typedef typename BadChanVec::const_iterator  const_iterator;
  typedef typename BadChanVec::size_type       size_type;

public:

  /// Default Constructor
  
  LArBadXCont() {}
  
  /// Constructor with payload
  LArBadXCont( const BadChanVec& vec);

  /**@brief Add a channel/FEB to the list of bad channels 
   * @param channel HWIdenifier of the channel in question
   * @param stat Bad-Channel object describing the disease this channels suffers
   */
  void add(const HWIdentifier channel, const LArBC_t stat);

  /**@brief Sort and purge the list of bad channels 
   * The list of bad channels get sorted by channel-ID
   * For channels appearing more than once, the bad-channel word 
   * gets merged by or'ing the problem-bits
   */
  void sort(); 
  
  /**@brief Query the status of a particular channel or FEB
   * This is the main client access method
   * @param HWIdentifer of the channel in question
   * @return LArBadChannel or LArBadFeb object describing the problem
   */
  LArBC_t status(const HWIdentifier channel) const;


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

#include "LArRecConditions/LArBadChannelCont.icc"



//Template instantiation for LArBadChannel
typedef LArBadXCont<LArBadChannel> LArBadChannelCont;
#include "CLIDSvc/CLASS_DEF.h" 
CLASS_DEF(LArBadChannelCont,64272230,1)
#include "AthenaKernel/CondCont.h"
CLASS_DEF( CondCont<LArBadChannelCont>,144954956 , 1 )

//Template instantiation for LArBadFeb
typedef LArBadXCont<LArBadFeb> LArBadFebCont;
CLASS_DEF(LArBadFebCont,60500160,1)
CLASS_DEF(CondCont<LArBadFebCont>,18499682, 1)


#endif
