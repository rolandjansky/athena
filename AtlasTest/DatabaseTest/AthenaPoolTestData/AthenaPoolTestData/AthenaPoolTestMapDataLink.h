/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file AthenaPoolTestMapDataLink.h 
 * 
 * @brief
 * 
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 * 
 * $Id: AthenaPoolTestMapDataLink.h,v 1.4 2009-02-15 12:56:15 schaffer Exp $
 */

#ifndef ATHENAPOOLTESTDATA_ATHENAPOOLTESTMAPDATALINK_H
# define ATHENAPOOLTESTDATA_ATHENAPOOLTESTMAPDATALINK_H

//<<<<<< INCLUDES                                                       >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>

/**
 * @class AthenaPoolTestMapDataLink 
 * 
 * @brief
 * 
 */

#include <cassert>
#include <iostream>
#include <limits>  /* numeric_limits */
#include <string>

class MyDummyClass;

class AthenaPoolTestMapDataLink {
public:  
    typedef unsigned int index_type;

    /// \name structors
    //@{
    AthenaPoolTestMapDataLink();
    AthenaPoolTestMapDataLink(index_type barCode, index_type eventIndex = 0);
    AthenaPoolTestMapDataLink(const MyDummyClass* p, index_type eventIndex = 0);
    //@}
  
    /// \name indexing accessors (e.g. for writing)
    //@{
    int        barcode() const;
    index_type eventIndex() const;
    //@}
  
    const MyDummyClass* cptr() const;

    class ExtendedBarCode {
    public:
	ExtendedBarCode();
	ExtendedBarCode(index_type barcode, index_type eventIndex);

	unsigned long  barcode() const;
	unsigned short eventIndex() const;

    private:
	unsigned long m_BC;
	unsigned short m_evtIndex;
    };
private:
    mutable MyDummyClass* m_particle; /* transient */
    ExtendedBarCode m_extBarcode;
};


inline AthenaPoolTestMapDataLink::ExtendedBarCode::ExtendedBarCode() : m_BC(0), m_evtIndex(0) {}

inline AthenaPoolTestMapDataLink::ExtendedBarCode::ExtendedBarCode(index_type barcode, index_type eventIndex) :
    m_BC(barcode), m_evtIndex(eventIndex)
{
    assert(eventIndex < std::numeric_limits<unsigned short>::max()); 
}

inline unsigned long \
AthenaPoolTestMapDataLink::ExtendedBarCode::barcode() const
{
    return (m_BC);
}

inline unsigned short 
AthenaPoolTestMapDataLink::ExtendedBarCode::eventIndex() const
{
    return (m_evtIndex);
}

inline
AthenaPoolTestMapDataLink::AthenaPoolTestMapDataLink() : m_particle(0), m_extBarcode() {}

inline
AthenaPoolTestMapDataLink::AthenaPoolTestMapDataLink(index_type barCode, index_type eventIndex) :
    m_particle(0), m_extBarcode(barCode, eventIndex) {}

inline
AthenaPoolTestMapDataLink::AthenaPoolTestMapDataLink(const MyDummyClass* /*p*/, index_type eventIndex) :
    m_particle(0), m_extBarcode(0, eventIndex) {}

inline int 
AthenaPoolTestMapDataLink::barcode() const 
{ return int(m_extBarcode.barcode()); } //FIXME ret type

inline const MyDummyClass* 
AthenaPoolTestMapDataLink::cptr() const
{
    return (m_particle);
}


inline AthenaPoolTestMapDataLink::index_type 
AthenaPoolTestMapDataLink::eventIndex() const 
{ return index_type(m_extBarcode.eventIndex()); }


//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // ATHENAPOOLTESTDATA_ATHENAPOOLTESTMAPDATALINK_H
