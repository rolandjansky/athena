/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IOVDBDATAMODEL_IOVPAYLOADCONTAINER_H
#define IOVDBDATAMODEL_IOVPAYLOADCONTAINER_H

/**
 * @file IOVPayloadContainer.h
 *
 * @brief This class is a container for the payload of conditions
 * data. It is intended to be used to store conditions data from COOL
 * as file meta data.
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 * @author Antoine Perus <perus@lal.in2p3.fr>
 *
 * $Id: IOVPayloadContainer.h,v 1.7 2009-04-30 14:15:30 schaffer Exp $
 */

//<<<<<< INCLUDES                                                       >>>>>>

#include "AthenaKernel/IOVRange.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include <vector>

//<<<<<< PUBLIC TYPES                                                   >>>>>>

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

/** @class IOVPayloadContainer
 *
 * @brief This class is a container for the payload of conditions
 * data. It is intended to be used to store conditions data from COOL
 * as file meta data.
 *
 */

class IOVPayloadContainer {
public:
    
    typedef  std::vector<CondAttrListCollection*>  payloadVec;
    typedef  payloadVec::const_iterator            const_iterator;
    typedef  payloadVec::size_type                 size_type;

    /// \name structors
    //@{
    IOVPayloadContainer();
    ~IOVPayloadContainer();
    //@}

    /// \name Payload accessors
    //@{

    /// Begin of payload vector
    const_iterator          begin() const;
 
    /// End of payload vector
    const_iterator          end() const;

    /// Element access
    CondAttrListCollection* at(unsigned int i) const;

    /// size of payload vector
    size_type               size() const;

    /// find the first payload that has a IOVRange which includes the
    //provided IOVTime
    const_iterator          find(const IOVTime& time) const;
 
    //@}
    
    /// \name Payload setting
    //@{
    /// Add in new payload, checking that it is not a
    /// duplicate. Returns true if merge is ok, false if it is a
    /// duplicate. Takes over ownership.
    /// Then the new AttrListColl will replace existing one when there
    /// is an overlap of the two IOVs, and returns true.
    bool                    merge(CondAttrListCollection* attrListColl);
    //@}

private:
    friend class IOVPayloadContainerCnv_p1;
    friend class IOVPayloadContainerPTCnv_p1;

    class AttrListCollSortStart {
    public:
	bool operator () (const CondAttrListCollection* x, const IOVTime& y) 
	    { return (x->minRange().start() < y); }
    };

    class AttrListCollSortStop {
    public:
	bool operator () (const CondAttrListCollection* x, const IOVTime& y) 
	    { return (x->minRange().stop() < y); }
    };

    payloadVec m_payloadVec;
};

// inlines
inline
IOVPayloadContainer::IOVPayloadContainer()
{}

inline
IOVPayloadContainer::const_iterator
IOVPayloadContainer::begin() const
{
    return (m_payloadVec.begin());
}

inline
IOVPayloadContainer::const_iterator
IOVPayloadContainer::end() const
{
    return (m_payloadVec.end());
}

inline
IOVPayloadContainer::size_type
IOVPayloadContainer::size() const
{
    return (m_payloadVec.size());
}

inline
CondAttrListCollection* 
IOVPayloadContainer::at(unsigned int i) const
{
    if (i < m_payloadVec.size()) return m_payloadVec[i];
    return (0);
}

inline
IOVPayloadContainer::const_iterator
IOVPayloadContainer::find(const IOVTime& time) const
{
    // Find first collection where time < stop time
    const_iterator result = std::lower_bound(m_payloadVec.begin(), 
					     m_payloadVec.end(),
					     time, AttrListCollSortStop()) ;
    // At end return:
    if ( result == m_payloadVec.end() )  return (result) ;

    // If time == stop, move forward one
    if ( (*result)->minRange().stop() == time)  ++result;

    // At end return:
    if ( result == m_payloadVec.end() )  return (result) ;

//     const CondAttrListCollection* coll  = (*(m_payloadVec.begin()));
//     const CondAttrListCollection* collr = (*result);
//     std::cout << "pos " << result - m_payloadVec.begin() << " size "
//               << m_payloadVec.size() << " range " 
//               << (*(coll->iov_begin())).second << " " << (*(collr->iov_begin())).second
//               << std::endl;
    
    // Check that time is in interval
    if ( (*result)->minRange().start() <= time)  return (result);

    // Not found
    return (m_payloadVec.end());
}


#endif  // IOVDBDATAMODEL_IOVPAYLOADCONTAINER_H
