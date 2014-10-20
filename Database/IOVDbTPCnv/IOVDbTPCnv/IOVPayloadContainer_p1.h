/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IOVDBDATAMODEL_IOVPAYLOADCONTAINER_P1_H
#define IOVDBDATAMODEL_IOVPAYLOADCONTAINER_P1_H

/**
 * @file IOVPayloadContainer_p1.h
 *
 * @brief This class is the persistent version of a container for the
 * payload of conditions data. It is intended to be used to store
 * conditions data from COOL as file meta data.
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 * @author Antoine Perus <perus@lal.in2p3.fr>
 *
 * $Id: IOVPayloadContainer_p1.h,v 1.5 2009-01-14 10:24:27 schaffer Exp $
 */

//<<<<<< INCLUDES                                                       >>>>>>
#include "IOVDbTPCnv/AttrListIndexes.h"
#include <stdint.h>
#include <vector>
#include <string>

class IOVPayloadContainer_p1 {
public:
    
    enum AttrListTypes {
    ATTR_BOOL,
    ATTR_CHAR,
    ATTR_UNSIGNED_CHAR,
    ATTR_SHORT,
    ATTR_UNSIGNED_SHORT,
    ATTR_INT,
    ATTR_UNSIGNED_INT,
    ATTR_LONG,
    ATTR_UNSIGNED_LONG,
    ATTR_LONG_LONG,
    ATTR_UNSIGNED_LONG_LONG,
    ATTR_FLOAT,
    ATTR_DOUBLE,
    ATTR_LONG_DOUBLE,
    ATTR_STRING,
    ATTR_BLOB,
    ATTR_DATE,
    ATTR_TIME_STAMP           
    };
    


    class IOVRange_p1
    {
    public:
        IOVRange_p1() : m_start(0), m_stop(0) {}
        unsigned long long m_start;
        unsigned long long m_stop;
    };
    
    
    class CondAttrListEntry_p1
    {
    public:

        unsigned int                      m_channelNumber;
        unsigned int                      m_firstIndex;
        unsigned int                      m_lastIndex;
        IOVRange_p1                       m_range;
        std::string                       m_name;
    };
    
    class CondAttrListCollection_p1
    {
    public:
        CondAttrListCollection_p1() :
          m_start(static_cast<unsigned long long>(-1)),
          m_stop (static_cast<unsigned long long>(-1)),
          m_hasRunLumiBlockTime(false)
      {}
        std::vector<CondAttrListEntry_p1> m_attrLists;
        // Must save stop - may be smaller than min stop of all
        // channels because there may be extra channels not yet in
        // this AttrListCollection
        // Similarly, we save start - introduced only for
        // IOVDbSvc-01-00-00
        unsigned long long                          m_start;
        unsigned long long                          m_stop;
        bool                              m_hasRunLumiBlockTime;
    };
    

    IOVPayloadContainer_p1();

//     // Copy from persistent attr list to transient attr list
//     void       copy(const CondAttrListCollection_p1 attrList_p, AthenaAttributeList& attrList);


//     // Copy in new transient attr list
//     void       add(const AthenaAttributeList& attrList);
    
    std::vector<CondAttrListCollection_p1>  m_payloadVec;

    // Keep attribute list indexes at top level to allow splitting
    std::vector<AttrListIndexes>    m_attrIndexes;

    std::vector<bool>               m_bool;
    std::vector<char>               m_char;
    std::vector<unsigned char>      m_unsignedChar;
    std::vector<short>              m_short;
    std::vector<unsigned short>     m_unsignedShort;
    std::vector<int>                m_int;
    std::vector<unsigned int>       m_unsignedInt;
    std::vector<long>               m_long;
    std::vector<unsigned long>      m_unsignedLong;
    std::vector<long long>          m_longLong;
    std::vector<unsigned long long> m_unsignedLongLong;
    std::vector<float>              m_float;
    std::vector<double>             m_double;
//    std::vector<long double>        m_longDouble;
    std::vector<std::string>        m_string;
    std::vector<unsigned long long> m_date;
    std::vector<unsigned long long> m_timeStamp;
    
    // AttributeList specification
    std::vector<std::string>        m_attrName;
    std::vector<unsigned int>       m_attrType;

};


#endif  // IOVDBDATAMODEL_IOVPAYLOADCONTAINER_P1_H
