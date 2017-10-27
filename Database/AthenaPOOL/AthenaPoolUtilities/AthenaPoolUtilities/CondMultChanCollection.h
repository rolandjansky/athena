/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CondMultChanCollection.h
 *
 * @brief This file defines the template class used to register the
 * tokens of T* in a COOL multchannel folder. 
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: CondMultChanCollection.h,v 1.5 2005-12-21 18:07:42 schaffer Exp $
 */

#ifndef DBDATAOBJECTS_CONDMULTCHANCOLLECTION_H
# define DBDATAOBJECTS_CONDMULTCHANCOLLECTION_H 

//<<<<<< INCLUDES                                                       >>>>>>

#include "AthenaPoolUtilities/CondMultChanCollImpl.h"
#include "AthContainers/DataVector.h"
#include "AthenaKernel/IOVRange.h"
#include <vector>

/**
 * @class CondMultChanCollection
 *
 * @brief A CondMultChanCollection is a template class which can hold
 * a collection of T* objects which are intended to be written out and
 * then registered in a COOL multichannel folder, or just read
 * back. The T* objects are normally written/read via a conversion
 * service, e.g. via AthenaPOOL, and their references (e.g. tokens)
 * saved as the data payload in COOL.
 * 
 * This class allows a subset of the channels to be written and
 * registered. This is done by adding the channel number for each T*:
 * for each push_back of a T*, one should also add a channel
 * number. On readback, each T* will have its channel number in the
 * parallel ChanVec.
 *
 * Finally when writing/registering, one may optionally specify the
 * IOV of each channel if they are different. This is done by an
 * IOVRange object for each channel. If the IOV is the same per
 * channel, it may be specified separately via the IIOVRegistrationSvc
 * interface. On readback, each channel will have its correct IOV. 
 *
 */

template <typename T>
class CondMultChanCollection : public DataVector<T>
{
public:

    typedef unsigned int                           ChanNum;
    typedef std::vector<ChanNum>                   ChanVec;
    typedef std::vector<IOVRange>                  IOVVec;
    typedef typename ChanVec::const_iterator       chan_const_iterator;
    typedef typename ChanVec::iterator             chan_iterator;
    typedef typename ChanVec::size_type            chan_size_type;
    typedef typename IOVVec::const_iterator        iov_const_iterator;
    typedef typename IOVVec::iterator              iov_iterator;
    typedef typename IOVVec::size_type             iov_size_type;
    
    /// Default constructor
    CondMultChanCollection();

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

    /// Adding in channel numbers
    void                        add(ChanNum chanNum);

    /// Adding in iov ranges
    void                        add(const IOVRange& range);

    /// Add new stop time to minRange - make sure that stop is <= to
    /// new stop 
    void                        addNewStop(const IOVTime& stop);

    /// Initialization done after creation or read back - derived
    /// classes may augment the functionality
    virtual StatusCode          initialize();

    /// Reset channel numbers - needed to allow sorting
    void                        resetChannelNumbers();

    /// Get implementation
    CondMultChanCollImpl*       implementation();

private:

    CondMultChanCollImpl  m_impl;
};

//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>


template <typename T>
inline
CondMultChanCollection<T>::CondMultChanCollection()
	:
	DataVector<T>::DataVector()
{}

/// Access to Channel numbers via iterators
template <typename T>
inline 
typename CondMultChanCollection<T>::chan_const_iterator
CondMultChanCollection<T>::chan_begin() const
{
    return (m_impl.chan_begin());
}

template <typename T>
inline 
typename CondMultChanCollection<T>::chan_const_iterator 
CondMultChanCollection<T>::chan_end  () const
{
    return (m_impl.chan_end());
}

/// number of channels
template <typename T>
inline 
typename CondMultChanCollection<T>::chan_size_type
CondMultChanCollection<T>::chan_size() const
{
    return (m_impl.chan_size());
}


/// Access to IOVs via iterators
template <typename T>
inline 
typename CondMultChanCollection<T>::iov_const_iterator
CondMultChanCollection<T>::iov_begin() const
{
    return (m_impl.iov_begin());
}

template <typename T>
inline 
typename CondMultChanCollection<T>::iov_const_iterator
CondMultChanCollection<T>::iov_end  () const
{
    return (m_impl.iov_end());
}
    
/// number of iovs
template <typename T>
inline 
typename CondMultChanCollection<T>::iov_size_type 
CondMultChanCollection<T>::iov_size() const
{
    return (m_impl.iov_size());
}

/// Current minimal IOVRange
template <typename T>
inline 
IOVRange
CondMultChanCollection<T>::minRange() const
{
    return (m_impl.minRange());
}

/// Check whether there is a unique IOV for all channels
template <typename T>
inline 
bool
CondMultChanCollection<T>::hasUniqueIOV() const
{
    return (m_impl.hasUniqueIOV());
}

/// Adding in channel numbers
template <typename T>
inline 
void                    
CondMultChanCollection<T>::add(ChanNum chanNum)
{
    m_impl.add(chanNum);
}

/// Adding in iov ranges
template <typename T>
inline 
void                    
CondMultChanCollection<T>::add(const IOVRange& range)
{
    m_impl.add(range);
}

/// Add new stop time to minRange - make sure that stop is <= to new
/// stop
template <typename T>
inline 
void                    
CondMultChanCollection<T>::addNewStop(const IOVTime& stop)
{
    m_impl.addNewStop(stop);
}

/// Initialization done after creation or read back - derived
/// classes may augment the functionality
template <typename T>
inline 
StatusCode 
CondMultChanCollection<T>::initialize() 
{
    return (StatusCode::SUCCESS);
}

/// Reset channel numbers - needed to allow sorting
template <typename T>
inline 
void 
CondMultChanCollection<T>::resetChannelNumbers()
{
    m_impl.resetChannelNumbers();
}

/// Get implementation
template <typename T>
inline 
CondMultChanCollImpl*
CondMultChanCollection<T>::implementation()
{
    return &m_impl;
}



#endif // DBDATAOBJECTS_CONDMULTCHANCOLLECTION_H
