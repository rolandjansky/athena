/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArConditionsChannelSet.h
 *
 * @brief This file defines the template class used for I/O of
 * conditions data
 *
 * @author RD Schaffer  <R.D.Schaffer@cern.ch>
 *
 * $Id: LArConditionsChannelSet.h,v 1.3 2006-03-31 14:39:13 schaffer Exp $
 */

#ifndef LARRAWCONDITIONS_LARCONDITIONSCHANNELSET_H
#define LARRAWCONDITIONS_LARCONDITIONSCHANNELSET_H
/**
 * @class LArConditionsChannelSet
 *
 * @brief template class for use for I/O of conditions data correction
 * sets 
 *
 * This class contains a vector of pairs of hardware channels ids and
 * a conditions object T. This class allows to store the conditions
 * for a subset of channels. It's original purpose is to allow
 * correction sets to be saved with corrections on a channel by
 * channel basis. The LArConditionsSubset is the corresponding primary
 * conditions container which is more optimal on space.
 *
**/

#include <vector>

template<class T> 
class LArConditionsChannelSet
{
public: 

    ///  Public typedefs
    typedef unsigned int  ChannelId; 
    typedef typename std::pair<ChannelId, T>             ChanIdTPair;
    typedef typename std::vector<ChanIdTPair>            ChannelVector;
    typedef typename ChannelVector::const_iterator       ConstChannelIt;
    typedef typename ChannelVector::iterator             ChannelIt;

    /// Default constructor
    LArConditionsChannelSet( );

    /// destructor
    virtual ~LArConditionsChannelSet();

    /// Access to a conditions object pair for a given channel id
    ConstChannelIt          find(ChannelId  id) const;

    /// Iterators over channel set
    ConstChannelIt          begin() const;
    ConstChannelIt          end()   const;

    /// Size of channel set
    size_t                  size() const;
    

    /// Insert a new channel id / T pair. If new channel id is the
    /// same as an existing one, the new T replaces the old T
    void                    insert (ChannelId id, const T& cond);

private:

    class PairSort 
    {
    public:
	bool operator () (const ChanIdTPair& x, const ChanIdTPair& y) 
	    {
		return (x.first < y.first);
	    }
	bool operator () (const ChanIdTPair& x, const unsigned int& id) 
	    {
		return (x.first < id);
	    }
    };

    ChannelVector           m_channelVec;
};


// INLINE FUNCTIONS


template<class T>
inline
LArConditionsChannelSet<T>::LArConditionsChannelSet( )
{}


template<class T>
inline
LArConditionsChannelSet<T>::~LArConditionsChannelSet()
{}


template<class T>
inline
typename LArConditionsChannelSet<T>::ConstChannelIt    
LArConditionsChannelSet<T>::find(ChannelId  id) const
{
  ConstChannelIt result = std::lower_bound(m_channelVec.begin(), 
					   m_channelVec.end(), 
					   id,
					   PairSort());

  if (result == m_channelVec.end()) return (result);
  if ((*result).first == id) {
      return (result);
  } else { 
    return m_channelVec.end() ;
  }
}

    

template<class T>
inline
typename LArConditionsChannelSet<T>::ConstChannelIt
LArConditionsChannelSet<T>::begin() const
{
    return (m_channelVec.begin());
}

template<class T>
inline
typename LArConditionsChannelSet<T>::ConstChannelIt       
LArConditionsChannelSet<T>::end()   const
{
    return (m_channelVec.end());
}



template<class T>
inline
size_t 
LArConditionsChannelSet<T>::size() const
{
    return (m_channelVec.size());
}
    
template<class T>
inline
void 
LArConditionsChannelSet<T>::insert (ChannelId id, const T& cond)
{
    m_channelVec.push_back(ChanIdTPair(id,cond));
    std::sort(m_channelVec.begin(), m_channelVec.end(), PairSort());
}

#endif // LARRAWCONDITIONS_LARCONDITIONSCHANNELSET_H
