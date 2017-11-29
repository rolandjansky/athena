/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CondAttrListCollection.h
 *
 * @brief This file defines the class for a collection of
 * AttributeLists where each one is associated with a channel number.
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: CondAttrListCollection.h,v 1.18 2009-04-29 07:37:17 schaffer Exp $
 */

#ifndef DBDATAOBJECTS_CONDATTRLISTCOLLECTION_H
# define DBDATAOBJECTS_CONDATTRLISTCOLLECTION_H 

//<<<<<< INCLUDES                                                       >>>>>>

#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeList.h"
#include "CoralBase/AttributeListSpecification.h"

#include "AthenaKernel/IOVRange.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "GaudiKernel/DataObject.h"

#include <vector>
#include <map>
#include <algorithm>



/**
 * @class CondAttrListCollection
 *
 * @brief This class is a collection of AttributeLists where each one
 * is associated with a channel number. This allows R/W with COOL of
 * multichannel conditions objects, stored in COOL itself. The
 * collection looks like a vector of pairs of (channel num,
 * AttributeList) where one may iterate over all of them or access
 * each AttrList by channel number as in a map. One also maintains the
 * IOV of each channel allow update of only invalid channels. 
 * 
 * WARNING: In order to accumulation correctly the minimum IOVRange
 * for all channels one needs to use the constructor which specifies
 * the type of time being used, i.e. run/lumiBlock or timestamp
 *
 */

class CondAttrListCollection : public DataObject
{
public:

    typedef unsigned int                               ChanNum;
    typedef coral::AttributeList                       AttributeList;
    typedef std::map<ChanNum, coral::AttributeList>    ChanAttrListMap;
    typedef ChanAttrListMap::value_type                ChanAttrListPair;
    typedef std::map<ChanNum, IOVRange>                ChanIOVMap;
    typedef ChanIOVMap::value_type                     ChanIOVPair;
    typedef std::map<ChanNum, std::string>             ChanNameMap;
    typedef ChanNameMap::value_type                    ChanNamePair;
    typedef ChanAttrListMap::const_iterator            const_iterator;
    typedef ChanAttrListMap::iterator                  iterator;
    typedef ChanAttrListMap::size_type                 size_type;
    typedef ChanIOVMap::const_iterator                 iov_const_iterator;
    typedef ChanIOVMap::iterator                       iov_iterator;
    typedef ChanIOVMap::size_type                      iov_size_type;
    typedef ChanNameMap::const_iterator                name_const_iterator;
    typedef ChanNameMap::iterator                      name_iterator;
    typedef ChanNameMap::size_type                     name_size_type;
    
    /// Constructor with specification for type of time: run/lumiBlock or timestamp
    CondAttrListCollection(bool hasRunLumiBlockTime);

    ~CondAttrListCollection();

    // copy constructor and assignment operator - have to be explicitly 
    // implemented to control use of the cachced AttributeListSpecification
    CondAttrListCollection(const CondAttrListCollection& rhs);
    // no copy with new Gaudi
    CondAttrListCollection& operator=(const CondAttrListCollection& rhs) = delete;

    /// Access to Chan/AttributeList pairs via channel number: returns map iterator
    const_iterator      chanAttrListPair(ChanNum chanNum) const;

    /// Access to Chan/AttributeList pairs via iterators
    const_iterator       begin() const;
    const_iterator       end  () const;
    
    /// number of Chan/AttributeList pairs
    size_type            size() const;
    

    /// Access to Chan/IOV pairs via channel number: returns map iterator
    iov_const_iterator   chanIOVPair(ChanNum chanNum) const;

    /// Access to Chan/IOV pairs via iterators
    iov_const_iterator   iov_begin() const;
    iov_const_iterator   iov_end  () const;
    
    /// number of Chan/IOV pairs
    iov_size_type        iov_size() const;

    /// Access to Chan/Name pairs via channel number: returns map iterator
    name_const_iterator  chanNamePair(ChanNum chanNum) const;

    /// Access to Chan/Name pairs via iterators
    name_const_iterator  name_begin() const;
    name_const_iterator  name_end() const;

    /// number of Chan/Name pairs
    name_size_type       name_size() const;

    /// channel number for index: (index = 0 to size-1)
    ChanNum              chanNum(unsigned int index) const;
    
    /// attribute list for a given channel number
    const AttributeList& attributeList(ChanNum chanNum) const;
    
    /// IOVRange list for a given channel number
    const IOVRange&      iovRange(ChanNum chanNum) const;
    
    /// find name for particular channel
    const std::string&   chanName(ChanNum chanNum) const;

    /// Current minimal IOVRange
    IOVRange             minRange() const;

    /// Check whether there is a unique IOV for all channels
    bool                 hasUniqueIOV() const;

    /// Adding in chan/attrList pairs
    void                 add(ChanNum chanNum, const AttributeList& attributeList);
    /// Adding in chan/attrList pairs with shared data
    void                 addShared(ChanNum chanNum, const AttributeList& attributeList);

    /// Adding in chan/iov range pairs
    void                 add(ChanNum chanNum, const IOVRange& range);

    // Adding in chan/name pairs
    void                 add(ChanNum chanNum,const std::string& name);

    /// Add new start time to minRange - make sure that start is >= to
    /// new start
    void                 addNewStart(const IOVTime& start);

    /// Add new stop time to minRange - make sure that stop is <= to
    /// new stop 
    void                 addNewStop(const IOVTime& stop);

    /// Reset minRange according to the IOVs of the contained
    /// channels.  This is needed to expand minRange, done with a
    /// subsequent addNewStart/Stop.
    void                 resetMinRange();

    /// Dump our contents to std::cout
    void                 dump() const;

    /// Equal operator. Compares channels, IOVs and attributes. For
    /// attributes only types and values are compared, not the
    /// attribute names 
    bool operator==( const CondAttrListCollection& rhs ) const;

    /// Comparison operator. Compares channels, IOVs and
    /// attributes. For attributes only types and values are compared,
    /// not the attribute names
    bool operator!=( const CondAttrListCollection& rhs ) const;

    /// Equal operator which ignores minRange. Same as equal operator
    /// but ignores the values for minRange. This allows merging of
    /// AttrListColls which have the same content and differ only by
    /// the minRange.
    bool                 isSameButMinRange ( const CondAttrListCollection& rhs,
                                             bool ignoreIOVs = false) const;



private:

    /// Default constructor - we hide it to prevent clients from using
    /// it - forcing them to specify the type of IOVTime with the
    /// other constructor
    CondAttrListCollection();

    ChanAttrListMap m_attrMap;
    ChanIOVMap      m_iovMap;
    ChanNameMap     m_nameMap;
    IOVRange        m_minRange;
    bool            m_hasUniqueIOV;
    bool            m_hasRunLumiBlockTime;
    coral::AttributeListSpecification* m_spec;
};

CLASS_DEF(CondAttrListCollection, 1238547719, 0)

#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( CondAttrListCollection, 1223307417 );

//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>


inline CondAttrListCollection::CondAttrListCollection()
:
m_minRange(IOVRange(IOVTime(IOVTime::MINRUN, IOVTime::MINEVENT), 
                    IOVTime(IOVTime::MAXRUN, IOVTime::MAXEVENT))),
    m_hasUniqueIOV(true),
    m_hasRunLumiBlockTime(true),
    m_spec(0)
{}

/// Constructor with specification for type of time: run/lumiBlock or timestamp
inline CondAttrListCollection::CondAttrListCollection(bool hasRunLumiBlockTime)
	:
	m_minRange(IOVRange(IOVTime(IOVTime::MINRUN, IOVTime::MINEVENT), 
			    IOVTime(IOVTime::MAXRUN, IOVTime::MAXEVENT))),
	m_hasUniqueIOV(true),
     m_hasRunLumiBlockTime(hasRunLumiBlockTime),
     m_spec(0)
{
    if (!m_hasRunLumiBlockTime) m_minRange = IOVRange(IOVTime(IOVTime::MINTIMESTAMP), 
                                                      IOVTime(IOVTime::MAXTIMESTAMP));
}

inline CondAttrListCollection::~CondAttrListCollection() {
  if (m_spec!=0) m_spec->release();
}

inline CondAttrListCollection::CondAttrListCollection(
			const CondAttrListCollection& rhs) :
  DataObject::DataObject(rhs),
  m_iovMap(rhs.m_iovMap),
  m_nameMap(rhs.m_nameMap),
  m_minRange(rhs.m_minRange),
  m_hasUniqueIOV(rhs.m_hasUniqueIOV),
  m_hasRunLumiBlockTime(rhs.m_hasRunLumiBlockTime),
  m_spec(0)
{
  // members with normal semantics setup in initialisation list
  // make a new cached AttributeListSpecificaiton and make the payload use it
  if (rhs.m_attrMap.size()>0) {
    m_spec=new coral::AttributeListSpecification();
    const coral::AttributeList& atr1=rhs.m_attrMap.begin()->second;
    for (coral::AttributeList::const_iterator itr=atr1.begin();itr!=atr1.end();++itr) {
      const coral::AttributeSpecification& aspec=itr->specification();
      m_spec->extend(aspec.name(),aspec.typeName());
    }
    for (const_iterator itr=rhs.m_attrMap.begin();itr!=rhs.m_attrMap.end();
       ++itr) {
      m_attrMap[itr->first]=coral::AttributeList(*m_spec,true);
      m_attrMap[itr->first].fastCopyData(itr->second);
    }
  }
}

// no copy with new Gaudi
// inline CondAttrListCollection& CondAttrListCollection::operator=(
// 				    const CondAttrListCollection& rhs) {
//   // catch assignment to self - do nothing
//   if (this==&rhs) return *this;
//   // assign base class
//   DataObject::operator=(rhs);
//   // assign members with normal semantics
//   m_attrMap             = rhs.m_attrMap;
//   m_iovMap              = rhs.m_iovMap;
//   m_nameMap             = rhs.m_nameMap;
//   m_minRange            = rhs.m_minRange;
//   m_hasUniqueIOV        = rhs.m_hasUniqueIOV;
//   m_hasRunLumiBlockTime = rhs.m_hasRunLumiBlockTime;
//   // make a new cache AttributeListSpecification and make the payload map
//   // use it
//   if (m_spec!=0) m_spec->release();
//   if (rhs.m_attrMap.size()>0) {
//     m_spec = new coral::AttributeListSpecification();
//     const coral::AttributeList& atr1 = rhs.m_attrMap.begin()->second;
//     for (coral::AttributeList::const_iterator itr = atr1.begin();itr!=atr1.end();++itr) {
//       const coral::AttributeSpecification& aspec = itr->specification();
//       m_spec->extend(aspec.name(),aspec.typeName());
//     }
//     m_attrMap.clear();
//     const_iterator itr=rhs.m_attrMap.begin();
//     for (; itr != rhs.m_attrMap.end(); ++itr) {
//       m_attrMap[itr->first] = coral::AttributeList(*m_spec,true);
//       m_attrMap[itr->first].fastCopyData(itr->second);
//     }
//   } else {
//     m_spec = 0;
//   }
//   return *this;
// }

/// Access to Chan/AttributeList pairs via channel number
inline CondAttrListCollection::const_iterator
CondAttrListCollection::chanAttrListPair(ChanNum chanNum) const
{
    return (m_attrMap.find(chanNum));
}


/// Access to Chan/AttributeList pairs via iterators
inline CondAttrListCollection::const_iterator
CondAttrListCollection::begin() const
{
    return (m_attrMap.begin());
}

inline CondAttrListCollection::const_iterator 
CondAttrListCollection::end  () const
{
    return (m_attrMap.end());
}

/// number of pairs
inline CondAttrListCollection::size_type  
CondAttrListCollection::size() const
{
    return (m_attrMap.size());
}


/// Access to IOV pairs via channel number
inline CondAttrListCollection::iov_const_iterator
CondAttrListCollection::chanIOVPair(ChanNum chanNum) const
{
    return (m_iovMap.find(chanNum));
}

/// Access to AttributeList pairs via iterators
inline CondAttrListCollection::iov_const_iterator
CondAttrListCollection::iov_begin() const
{
    return (m_iovMap.begin());
}

inline CondAttrListCollection::iov_const_iterator
CondAttrListCollection::iov_end  () const
{
    return (m_iovMap.end());
}
    
/// number of pairs
inline CondAttrListCollection::iov_size_type 
CondAttrListCollection::iov_size() const
{
    return (m_iovMap.size());
}

/// Access to name via channel number
inline CondAttrListCollection::name_const_iterator
CondAttrListCollection::chanNamePair(ChanNum chanNum) const
{
    return (m_nameMap.find(chanNum));
}

/// Access to chan/name pairs via iterators
inline CondAttrListCollection::name_const_iterator
CondAttrListCollection::name_begin() const
{
    return (m_nameMap.begin());
}

inline CondAttrListCollection::name_const_iterator
CondAttrListCollection::name_end  () const
{
    return (m_nameMap.end());
}
    
/// number of pairs
inline CondAttrListCollection::name_size_type 
CondAttrListCollection::name_size() const
{
    return (m_nameMap.size());
}

/// channel number for index: (index = 0 to size-1)
inline CondAttrListCollection::ChanNum
CondAttrListCollection::chanNum(unsigned int index) const
{
    if (index < size()) {
        const_iterator it = begin();
        unsigned int count = index;
        while (count > 0) {
            ++it;
            --count;
        }
        return (it->first);
    }
    // otherwise return max
    else return (0xFFFF);
}

/// attribute list for a given channel number
inline const CondAttrListCollection::AttributeList&
CondAttrListCollection::attributeList(ChanNum chanNum) const
{
    const_iterator itr=m_attrMap.find(chanNum);
    if (itr!=m_attrMap.end()) {
        return itr->second;
    } else {
        static AttributeList attr;
        return attr;
    }
}
    
/// IOVRange list for a given channel number
inline const IOVRange&         
CondAttrListCollection::iovRange(ChanNum chanNum) const
{
    iov_const_iterator itr=m_iovMap.find(chanNum);
    if (itr!=m_iovMap.end()) {
        return itr->second;
    } else {
        // Default is the minRange
        return (m_minRange);
    }
}

// find name for channel - empty string for unknonwn/unnamed channels
inline const std::string& CondAttrListCollection::chanName(ChanNum chanNum) const {
    name_const_iterator itr=m_nameMap.find(chanNum);
    if (itr!=m_nameMap.end()) {
        return itr->second;
    } else {
        static std::string name;
        return name;
    }
}

/// Current minimal IOVRange
inline IOVRange
CondAttrListCollection::minRange() const
{
    return (m_minRange);
}

/// Check whether there is a unique IOV for all channels
inline bool
CondAttrListCollection::hasUniqueIOV() const
{
    return (m_hasUniqueIOV);
}

/// Adding in chan/attrList pairs: ASSUMED TO BE IN ORDER
inline void                    
CondAttrListCollection::add(ChanNum chanNum, const AttributeList& attributeList)
{
  if (m_attrMap.size()==0) {   
    m_spec=new coral::AttributeListSpecification();
    for (coral::AttributeList::const_iterator itr=attributeList.begin();itr!=attributeList.end();++itr) {
      const coral::AttributeSpecification& aspec=itr->specification();
      m_spec->extend(aspec.name(),aspec.typeName());
    }
  }
  m_attrMap[chanNum]=coral::AttributeList(*m_spec,true);
  m_attrMap[chanNum].fastCopyData(attributeList);
}

/// Adding in chan/attrList pairs with shared AttrList: ASSUMED TO BE IN ORDER
inline void                    
CondAttrListCollection::addShared(ChanNum chanNum, const AttributeList& attributeList)
{
  if (m_attrMap.size()==0) {   
    m_spec=new coral::AttributeListSpecification();
    for (coral::AttributeList::const_iterator itr=attributeList.begin();itr!=attributeList.end();++itr) {
      const coral::AttributeSpecification& aspec=itr->specification();
      m_spec->extend(aspec.name(),aspec.typeName());
    }
  }
  m_attrMap[chanNum]=coral::AttributeList(*m_spec,true);
  m_attrMap[chanNum].merge(attributeList);
}

/// Adding in chan/iov pairs:  ASSUMED TO BE IN ORDER
inline void                    
CondAttrListCollection::add(ChanNum chanNum, const IOVRange& range)
{
    m_iovMap[chanNum] = range;

    // Accumulate minRange
    IOVTime start = m_minRange.start();
    if (m_minRange.start() < range.start()) start = range.start();
    IOVTime stop = m_minRange.stop();
    if (range.stop() < m_minRange.stop()) stop = range.stop();
    m_minRange = IOVRange(start, stop);
    if (m_hasUniqueIOV && range != m_minRange)m_hasUniqueIOV = false;
}

/// Adding in chan/name pairs: ASSUMED TO BE IN ORDER
inline void                    
CondAttrListCollection::add(ChanNum chanNum, const std::string& name)
{
    m_nameMap[chanNum] = name;
}


/// Add new start time to minRange - make sure that start is >= to new
/// start
inline void                    
CondAttrListCollection::addNewStart(const IOVTime& start)
{
    if (start > m_minRange.start()) {
      m_minRange = IOVRange(start,m_minRange.stop());
    }
}

/// Add new stop time to minRange - make sure that stop is <= to new
/// stop
inline void                    
CondAttrListCollection::addNewStop(const IOVTime& stop)
{
    if (stop < m_minRange.stop()) {
	m_minRange = IOVRange(m_minRange.start(), stop);
    }
}

/// Reset minRange according to the IOVs of the contained
/// channels.  This is needed to expand minRange, done with a
/// subsequent addNewStart/Stop.
inline void
CondAttrListCollection::resetMinRange()
{
    // Reset minRange to max
    if (m_minRange.start().isTimestamp()) {
        m_minRange = IOVRange(IOVTime(IOVTime::MINTIMESTAMP), IOVTime(IOVTime::MAXTIMESTAMP));
    }
    else {
        m_minRange = IOVRange(IOVTime(IOVTime::MINRUN, IOVTime::MINEVENT), 
                              IOVTime(IOVTime::MAXRUN, IOVTime::MAXEVENT)); 
    }
    iov_const_iterator it1   = iov_begin();
    iov_const_iterator last1 = iov_end();
    for (; it1 != last1; ++it1) {
        const IOVTime& start = (*it1).second.start();
        const IOVTime& stop  = (*it1).second.stop();
        if (start > m_minRange.start() && stop < m_minRange.stop()) {
            m_minRange = IOVRange(start, stop);
        }
        else if (start > m_minRange.start()) {
            m_minRange = IOVRange(start,m_minRange.stop());
        }
        else if (stop < m_minRange.stop()) {
            m_minRange = IOVRange(m_minRange.start(), stop);
        }
    }
}

inline void                    
CondAttrListCollection::dump() const
{
    // min range
    std::cout << "min range: " << m_minRange << std::endl;

    // attribute list
    const_iterator it   = begin();
    const_iterator last = end();
    for (; it != last; ++it) {
	std::cout << "chan, attr: " << (*it).first << std::endl;
	(*it).second.toOutputStream(std::cout) << std::endl;
    }
    // IOVs
    iov_const_iterator it1   = iov_begin();
    iov_const_iterator last1 = iov_end();
    for (; it1 != last1; ++it1) {
	std::cout << "chan, iov: " << (*it1).first << std::endl;
	std::cout << (*it1).second  << std::endl;
    }
    // channel names
    name_const_iterator it2   = name_begin();
    name_const_iterator last2 = name_end();
    for (; it2 != last2; ++it2) {
	std::cout << "chan, name: " << (*it2).first << std::endl;
	std::cout << (*it2).second  << std::endl;
    }
    
}

inline
bool 
CondAttrListCollection::operator==( const CondAttrListCollection& rhs ) const
{

    // Compare minRange 
    if (m_minRange != rhs.m_minRange)                      return false;
    // Rest is in isSameButMinRange
    return (isSameButMinRange (rhs));
}

    

inline
bool
CondAttrListCollection::operator!=( const CondAttrListCollection& rhs ) const
{
    return (!((*this) == rhs));
}

inline
bool
CondAttrListCollection::isSameButMinRange ( const CondAttrListCollection& rhs,
                                            bool ignoreIOVs ) const
{
    
    // Compare with right hand side

    if(m_hasUniqueIOV != rhs.m_hasUniqueIOV)               return false;
    if(m_hasRunLumiBlockTime != rhs.m_hasRunLumiBlockTime) return false;
    if (m_attrMap.size() != rhs.m_attrMap.size())          return false;
    if (m_iovMap.size() != rhs.m_iovMap.size())            return false;
    if (m_nameMap.size() != rhs.m_nameMap.size())          return false;

    if (!ignoreIOVs) {
        // Check IOVs
        iov_const_iterator it1  = m_iovMap.begin();
        iov_const_iterator it2  = rhs.m_iovMap.begin();
        iov_const_iterator end1 = m_iovMap.end();
        for (; it1 != end1; ++it1, ++it2) {
            if ((*it1).first != (*it2).first)   return false;
            if ((*it1).second != (*it2).second) return false;
        }
    }
    
    // Check attribute lists
    const_iterator it3  = m_attrMap.begin();
    const_iterator it4  = rhs.m_attrMap.begin();
    const_iterator end3 = m_attrMap.end();
    for (; it3 != end3; ++it3, ++it4) {
        if ((*it3).first != (*it4).first)   return false;
        if ((*it3).second != (*it4).second) return false;
    }
    
    // Check names
    name_const_iterator it5  = m_nameMap.begin();
    name_const_iterator it6  = rhs.m_nameMap.begin();
    name_const_iterator end5 = m_nameMap.end();
    for (; it5 != end5; ++it5, ++it6) {
        if ((*it5).first != (*it6).first)   return false;
        if ((*it5).second != (*it6).second) return false;
    }

    return true;
}

    
    

#endif // DBDATAOBJECTS_CONDATTRLISTCOLLECTION_H
