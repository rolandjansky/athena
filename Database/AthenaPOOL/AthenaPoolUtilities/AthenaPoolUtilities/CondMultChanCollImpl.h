/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CondMultChanCollImpl.h
 *
 * @brief This file defines the non-template part of a CondMultChanCollection. 
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: CondMultChanCollImpl.h,v 1.3 2006-11-07 14:01:51 schaffer Exp $
 */

#ifndef DBDATAOBJECTS_CONDMULTCHANCOLLIMPL_H
# define DBDATAOBJECTS_CONDMULTCHANCOLLIMPL_H 

//<<<<<< INCLUDES                                                       >>>>>>

#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthContainers/DataVector.h"
#include "AthenaKernel/IOVRange.h"
#include <vector>

/**
 * @class CondMultChanCollImpl
 *
 * @brief A CondMultChanCollImpl defines the non-template part of a
 * CondMultChanCollection. It holds the vectors of:
 *   channel numbers
 *   IOVs
 *   tokens to the DataObject
 *   a pointer to the CondAttrListCollection, used to register in COOL
 *
 */

class CondMultChanCollImpl 
{
public:

    typedef unsigned int                  ChanNum;
    typedef std::vector<ChanNum>          ChanVec;
    typedef std::vector<IOVRange>         IOVVec;
    typedef std::vector<std::string>      TokenVec;
    typedef ChanVec::const_iterator       chan_const_iterator;
    typedef ChanVec::iterator             chan_iterator;
    typedef ChanVec::size_type            chan_size_type;
    typedef IOVVec::const_iterator        iov_const_iterator;
    typedef IOVVec::iterator              iov_iterator;
    typedef IOVVec::size_type             iov_size_type;
    typedef TokenVec::const_iterator      token_const_iterator;
    typedef TokenVec::iterator            token_iterator;
    typedef TokenVec::size_type           token_size_type;
    
    /// Default constructor
    CondMultChanCollImpl();

    /// Default destructor
    virtual ~CondMultChanCollImpl();

    /// Access to Channel numbers via iterators
    chan_const_iterator         chan_begin() const;
    chan_const_iterator         chan_end  () const;
    
    /// number of channels
    chan_size_type              chan_size() const;
    
    /// Access to IOVs via iterators
    iov_const_iterator          iov_begin() const;
    iov_const_iterator          iov_end  () const;
    
    /// number of IOVs
    iov_size_type               iov_size() const;

    /// Current minimal IOVRange
    IOVRange                    minRange() const;

    /// Check whether there is a unique IOV for all channels
    bool                        hasUniqueIOV() const;

    /// Access to tokens via iterators
    token_const_iterator        token_begin() const;
    token_const_iterator        token_end  () const;
    
    /// number of tokens
    token_size_type             token_size() const;

    /// Adding in channel numbers
    void                        add(ChanNum chanNum);

    /// Adding in iov ranges
    void                        add(const IOVRange& range);

    /// Add new stop time to minRange - make sure that stop is <= to
    /// new stop 
    void                        addNewStop(const IOVTime& stop);

    /// Adding in token
    void                        add(const std::string& token);

    /// Set pointer to the CondAttrListCollection
    void                        setAttrListColl(CondAttrListCollection* coll);

    /// Reset channel numbers - needed to allow sorting
    void                        resetChannelNumbers();

    /// Reset tokens - needed to multiple I/O of the same object
    void                        resetTokens();

private:


    CondAttrListCollection*   m_attrListColl;
    ChanVec                   m_channels;
    IOVVec                    m_iovs;
    TokenVec                  m_tokens;
    IOVRange                  m_minRange;
    bool                      m_hasUniqueIOV;
    bool                      m_checkRunEventTime;
};

//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>


inline
CondMultChanCollImpl::CondMultChanCollImpl()
	:
	m_attrListColl(0),
	// Assume run/event for minRange
	m_minRange(IOVRange(IOVTime(IOVTime::MINRUN, IOVTime::MINEVENT), 
			    IOVTime(IOVTime::MAXRUN, IOVTime::MAXEVENT))),
	m_hasUniqueIOV(true),
	m_checkRunEventTime(true)
{}

/// Default destructor
inline
CondMultChanCollImpl::~CondMultChanCollImpl()
{}


/// Access to Channel numbers via iterators
inline 
CondMultChanCollImpl::chan_const_iterator
CondMultChanCollImpl::chan_begin() const
{
    return (m_channels.begin());
}

inline 
CondMultChanCollImpl::chan_const_iterator 
CondMultChanCollImpl::chan_end  () const
{
    return (m_channels.end());
}

/// number of channels
inline 
CondMultChanCollImpl::chan_size_type
CondMultChanCollImpl::chan_size() const
{
    return (m_channels.size());
}


/// Access to IOVs via iterators
inline 
CondMultChanCollImpl::iov_const_iterator
CondMultChanCollImpl::iov_begin() const
{
    return (m_iovs.begin());
}

inline 
CondMultChanCollImpl::iov_const_iterator
CondMultChanCollImpl::iov_end  () const
{
    return (m_iovs.end());
}
    
/// number of iovs
inline 
CondMultChanCollImpl::iov_size_type 
CondMultChanCollImpl::iov_size() const
{
    return (m_iovs.size());
}

/// Current minimal IOVRange
inline 
IOVRange
CondMultChanCollImpl::minRange() const
{
    return (m_minRange);
}

/// Check whether there is a unique IOV for all channels
inline 
bool
CondMultChanCollImpl::hasUniqueIOV() const
{
    return (m_hasUniqueIOV);
}

/// Access to tokens via iterators
inline 
CondMultChanCollImpl::token_const_iterator
CondMultChanCollImpl::token_begin() const
{
    return m_tokens.begin();
}

inline 
CondMultChanCollImpl::token_const_iterator
CondMultChanCollImpl::token_end  () const
{
    return m_tokens.end();
}
    
/// number of tokens
inline 
CondMultChanCollImpl::token_size_type
CondMultChanCollImpl::token_size() const
{
    return m_tokens.size();
}

/// Adding in channel numbers
inline 
void                    
CondMultChanCollImpl::add(ChanNum chanNum)
{
    m_channels.push_back(chanNum);
}

/// Adding in iov ranges
inline 
void                    
CondMultChanCollImpl::add(const IOVRange& range)
{
    // Check if we're adding too many iovs - may be a 2nd pass
    if (m_iovs.size() >= m_channels.size()) {
	// Should throw an exception here because we are trying to
	// add more iovs than channels. Now we just return
	return;
    }
    
    // Save range
    m_iovs.push_back(range);

    // If CondAttrListCollection exists, also add to it
    if(m_attrListColl) {
	// Get channel number
	ChanNum chan = m_channels[m_iovs.size()-1];
	m_attrListColl->add(chan, range);
    }
    
    if(m_checkRunEventTime) {
	// On the first push_back we must check if we're using a time
	// stamp and if so, reset the minRange
	if(range.start().isTimestamp()) {
	    m_minRange = IOVRange(IOVTime(IOVTime::MINTIMESTAMP), 
				  IOVTime(IOVTime::MAXTIMESTAMP));
	}
	m_checkRunEventTime = false;
    }

    // Accumulate minRange
    IOVTime start = m_minRange.start();
    if (m_minRange.start() < range.start()) start = range.start();
    IOVTime stop = m_minRange.stop();
    if (range.stop() < m_minRange.stop()) stop = range.stop();
    m_minRange = IOVRange(start, stop);
    if (m_hasUniqueIOV && range != m_minRange)m_hasUniqueIOV = false;
}

/// Add new stop time to minRange - make sure that stop is <= to new
/// stop
inline 
void                    
CondMultChanCollImpl::addNewStop(const IOVTime& stop)
{
    if (stop < m_minRange.stop()) {
	m_minRange = IOVRange(m_minRange.start(), stop);
    }
}

/// Adding in token
inline 
void                  
CondMultChanCollImpl::add(const std::string& token)
{
    m_tokens.push_back(token);
}

/// Add pointer to the CondAttrListCollection
inline 
void                    
CondMultChanCollImpl::setAttrListColl(CondAttrListCollection* coll)
{
    m_attrListColl = coll;
}

/// Reset channel numbers - needed to allow sorting
inline 
void	
CondMultChanCollImpl::resetChannelNumbers()
{
    m_channels.clear();
}

/// Reset tokens - needed to multiple I/O of the same object
inline
void
CondMultChanCollImpl::resetTokens()
{
    m_tokens.clear();
}


#endif // DBDATAOBJECTS_CONDMULTCHANCOLLIMPL_H
