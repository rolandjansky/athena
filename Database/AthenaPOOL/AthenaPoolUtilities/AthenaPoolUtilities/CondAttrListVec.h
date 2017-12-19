/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CondAttrListVec.h
 *
 * @brief A CondAttrListVec is an Athena DataObject holding a vector of
 * CORAL AttributeLists, each associated to a channel number and
 * an interval of validity. There may be many AttributeLists per channel/IOV
 * This provides the Athena representation of data from the CoraCool
 * data model with payload data stored directly in tables referenced by COOL
 *
 * @author Richard Hawkings <richard.hawkings@cern.ch>
 *
 */

#ifndef DBDATAOBJECTS_CONDATTRLISTVEC_H 
#define DBDATAOBJECTS_CONDATTRLISTVEC_H 

//<<<<<< INCLUDES                                                       >>>>>>

#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeList.h"
#include "CoralBase/AttributeListSpecification.h"
#include "AthenaKernel/IOVRange.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "GaudiKernel/DataObject.h"

class CondAttrListVec : public DataObject
{
 public:
  typedef std::pair<unsigned int,coral::AttributeList> AttrListPair;
  typedef std::vector<AttrListPair> AttrListVec;
  typedef std::vector<coral::AttributeList>::const_iterator AttrListDataIter;
  typedef AttrListVec::const_iterator const_iterator;
  typedef AttrListVec::size_type size_type;
  typedef std::map<unsigned int,IOVRange> AttrListIOVMap;
  typedef AttrListIOVMap::const_iterator iov_const_iterator;
  typedef AttrListIOVMap::size_type iov_size_type;
  typedef std::map<unsigned int, std::vector<const coral::AttributeList*> >
    AttrListCVMap;

  // constructor with specification of type of time: run/event or timestamp
  CondAttrListVec(bool runevent);
  // constructor with specification of type and initial size
  CondAttrListVec(bool runevent, size_type nelm);
  // destructor to release specification
  ~CondAttrListVec();
  // copy constructor and assignment operator - have to be explicitly 
  // implemented to control use of the cachced AttributeListSpecification
  CondAttrListVec(const CondAttrListVec& rhs);
  // no copy operator with new Gaudi 
  CondAttrListVec& operator=(const CondAttrListVec& rhs) = delete;

  // access to iterate over channel/AttributeList pairs
  const_iterator begin() const;
  const_iterator end() const;
  // number of channel/AttributeList pairs
  size_type size() const;

  // access to channel/IOVRange map associated with AttributeList data
  iov_const_iterator iov_begin() const;
  iov_const_iterator iov_end() const;
  // find IOV associated with channel
  iov_const_iterator iovRange(const unsigned int channel) const;
  // number of IOV ranges stored
  size_type iov_size() const;

  // minimal IOVRange and uniqueness
  IOVRange minRange() const;
  bool hasUniqueIOV() const;

  // adding new data - specify IOV range, channel and start/end of 
  // AttributeLists
  void add(const IOVRange& range,const unsigned int chan,
	   AttrListDataIter data_begin,AttrListDataIter data_end);

  // adding new data sliced out of a vector of AttributeLists
  // specify IOV range, channel, vector and start/end offsets
  void addSlice(const IOVRange& range,const unsigned int chan,
		const std::vector<coral::AttributeList>& data,
		const unsigned int datastart,const unsigned int dataend);

  // add new stop time to minRange - make sure minRange is <=stop
  void addNewStop(const IOVTime& stop);

  // indexing function - return vector of unique channel IDs
  const std::vector<unsigned int>& channelIDs() const;

  // return true if a particular channel ID exists in the data
  bool hasChannel(const int chan) const;

  // return vector of pointers to attribute lists 
  // that are associated with given channel
  // requesting a channel which does not exist may give an exception
  const std::vector<const coral::AttributeList*>& 
    attrListVec(const int chan) const;

 private:
  // hide default constructor - force client to specify type
  CondAttrListVec();

  AttrListVec m_data;
  AttrListIOVMap m_iovmap;
  IOVRange m_minrange;
  bool m_uniqueiov;
  bool m_runevent;
  mutable bool m_indexed;
  coral::AttributeListSpecification* m_spec;
  // indexing variables
  mutable AttrListCVMap m_indexchanvec;
  mutable std::vector<unsigned int> m_indexchan;

  void makeIndex() const;
};

CLASS_DEF( CondAttrListVec , 55403898 , 1)

#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( CondAttrListVec, 74272308 );

inline CondAttrListVec::CondAttrListVec(bool runevent) :
  m_minrange(IOVRange(IOVTime(IOVTime::MINRUN, IOVTime::MINEVENT), 
		      IOVTime(IOVTime::MAXRUN, IOVTime::MAXEVENT))),
  m_uniqueiov(true),m_runevent(runevent),m_indexed(false),m_spec(0) {
  if (!runevent) m_minrange=IOVRange(IOVTime(IOVTime::MINTIMESTAMP), 
				     IOVTime(IOVTime::MAXTIMESTAMP));
}

inline CondAttrListVec::CondAttrListVec(bool runevent, size_type nelm) :
  m_minrange(IOVRange(IOVTime(IOVTime::MINRUN, IOVTime::MINEVENT), 
		      IOVTime(IOVTime::MAXRUN, IOVTime::MAXEVENT))),
	 m_uniqueiov(true),m_runevent(runevent),m_indexed(false),m_spec(0) {
  if (!runevent) m_minrange=IOVRange(IOVTime(IOVTime::MINTIMESTAMP), 
				     IOVTime(IOVTime::MAXTIMESTAMP));
  m_data.reserve(nelm);
}


inline CondAttrListVec::~CondAttrListVec() {
  if (m_spec!=0) m_spec->release();
}

inline CondAttrListVec::CondAttrListVec(const CondAttrListVec& rhs) :
  DataObject::DataObject(rhs),
  m_iovmap(rhs.m_iovmap),
  m_minrange(rhs.m_minrange),
  m_uniqueiov(rhs.m_uniqueiov),
  m_runevent(rhs.m_runevent),m_indexed(false),
  m_spec(0)
{
  // members with normal semantics setup in initialisation list
  // make a new cached AttributeListSpecification and make the payload use it
  if (rhs.m_data.size()>0) {
    m_spec=new coral::AttributeListSpecification();
    const coral::AttributeList& atr1=rhs.m_data.begin()->second;
    for (coral::AttributeList::const_iterator itr=atr1.begin();itr!=atr1.end();
	 ++itr) {
      const coral::AttributeSpecification& aspec=itr->specification();
      m_spec->extend(aspec.name(),aspec.typeName());
    }
    for (const_iterator itr=rhs.m_data.begin();itr!=rhs.m_data.end();++itr) {
      m_data.push_back(AttrListPair(itr->first,
				    coral::AttributeList(*m_spec,true)));
      (m_data.back().second).fastCopyData(itr->second);
    }
  }
}

// no copy operator with new Gaudi
// inline CondAttrListVec& CondAttrListVec::operator=(const CondAttrListVec& rhs) {
//   // catch assignment to self - do nothing
//   if (this==&rhs) return *this;
//   // assign base class
//   DataObject::operator=(rhs);
//   // assign members with normal semantics
//   m_iovmap=rhs.m_iovmap;
//   m_minrange=rhs.m_minrange;
//   m_uniqueiov=rhs.m_uniqueiov;
//   m_runevent=rhs.m_runevent;
//   m_indexed=false;
//   // make a new cache AttributeListSpecification and make the payload map
//   // use it
//   if (m_spec!=0) m_spec->release();
//   if (rhs.m_data.size()>0) {
//     m_spec=new coral::AttributeListSpecification();
//     const coral::AttributeList& atr1=rhs.m_data.begin()->second;
//     for (coral::AttributeList::const_iterator itr=atr1.begin();itr!=atr1.end();
// 	 ++itr) {
//       const coral::AttributeSpecification& aspec=itr->specification();
//       m_spec->extend(aspec.name(),aspec.typeName());
//     }
//     m_data.clear();
//     for (const_iterator itr=rhs.m_data.begin();itr!=rhs.m_data.end();++itr) {
//       m_data.push_back(AttrListPair(itr->first,
// 				    coral::AttributeList(*m_spec,true)));
//       (m_data.back().second).fastCopyData(itr->second);
//     }
//   } else {
//     m_spec=0;
//   }
//   return *this;
// }

inline CondAttrListVec::const_iterator CondAttrListVec::begin() const
{ return m_data.begin(); }

inline CondAttrListVec::const_iterator CondAttrListVec::end() const
{ return m_data.end(); }

inline CondAttrListVec::size_type CondAttrListVec::size() const
{ return m_data.size(); }

inline CondAttrListVec::iov_const_iterator CondAttrListVec::iov_begin() const
{ return m_iovmap.begin(); }

inline CondAttrListVec::iov_const_iterator CondAttrListVec::iov_end() const
{ return m_iovmap.end(); }

inline CondAttrListVec::iov_const_iterator 
CondAttrListVec::iovRange(const unsigned int channel) const 
{ return m_iovmap.find(channel); }

inline CondAttrListVec::size_type CondAttrListVec::iov_size() const
{ return m_iovmap.size(); }

inline IOVRange CondAttrListVec::minRange() const
{ return m_minrange; }

inline bool CondAttrListVec::hasUniqueIOV() const 
{return m_uniqueiov; }


inline void CondAttrListVec::add(const IOVRange& range,
	   const unsigned int chan, 
	   AttrListDataIter data_begin, AttrListDataIter data_end) {
  //invalidate index
  m_indexed=false;
  // set minimum range correctly
  IOVTime start=m_minrange.start();
  if (m_minrange.start()<range.start()) start=range.start();
  IOVTime stop = m_minrange.stop();
  if (range.stop()<m_minrange.stop()) stop=range.stop();
  m_minrange=IOVRange(start,stop);
  if (m_uniqueiov && range!=m_minrange) m_uniqueiov=false;
  // store attributeList if needed
  if (m_spec==0 && data_begin!=data_end) {
    m_spec=new coral::AttributeListSpecification();
    for (coral::AttributeList::const_iterator itr=data_begin->begin();itr!=data_begin->end();++itr) {
      const coral::AttributeSpecification& aspec=itr->specification();
      m_spec->extend(aspec.name(),aspec.typeName());
    }
  }
  // copy in data
  for (AttrListDataIter itr=data_begin;itr!=data_end;++itr) {
    m_data.push_back(AttrListPair(chan,coral::AttributeList(*m_spec,true)));
    (m_data.back().second).fastCopyData(*itr);
  }
  m_iovmap[chan]=range;
}

inline void CondAttrListVec::addSlice(const IOVRange& range,
	   const unsigned int chan, 
	   const std::vector<coral::AttributeList>& data,
	   const unsigned int datastart,const unsigned int dataend) {
  // invalidate index
  m_indexed=false;
  // set minimum range correctly
  IOVTime start=m_minrange.start();
  if (m_minrange.start()<range.start()) start=range.start();
  IOVTime stop = m_minrange.stop();
  if (range.stop()<m_minrange.stop()) stop=range.stop();
  m_minrange=IOVRange(start,stop);
  if (m_uniqueiov && range!=m_minrange) m_uniqueiov=false;
  // store attributeList if needed
  if (m_spec==0 && datastart!=dataend) {
    m_spec=new coral::AttributeListSpecification();
    const coral::AttributeList& atr0=data[datastart];
    for (coral::AttributeList::const_iterator itr=atr0.begin();
	 itr!=atr0.end();++itr) {
      const coral::AttributeSpecification& aspec=itr->specification();
      m_spec->extend(aspec.name(),aspec.typeName());
    }
  }
  // copy in data
  for (unsigned int i=datastart;i<dataend;++i) {
    m_data.push_back(AttrListPair(chan,coral::AttributeList(*m_spec,true)));
    (m_data.back().second).merge(data[i]);
  }
  m_iovmap[chan]=range;
}

inline void CondAttrListVec::addNewStop(const IOVTime& stop) {
  if (stop<m_minrange.stop()) m_minrange=IOVRange(m_minrange.start(),stop);
}

inline const std::vector<unsigned int>& CondAttrListVec::channelIDs() const {
  makeIndex();
  return m_indexchan;
}

inline bool CondAttrListVec::hasChannel(const int chan) const {
  return (m_indexchanvec.find(chan)!=m_indexchanvec.end());
}

inline const std::vector<const coral::AttributeList*>& 
  CondAttrListVec::attrListVec(const int chan) const {
  makeIndex();
  return m_indexchanvec[chan];
}

inline void CondAttrListVec::makeIndex() const {
  // check if already indexed
  if (m_indexed) return;
  m_indexchanvec.clear();
  m_indexchan.clear();
  // loop over all the elements, build vector of AttributeList for each channel
  // and a vector of all the channels
  for (const_iterator citr=m_data.begin();citr!=m_data.end();++citr) {
    unsigned int chan=citr->first;
    AttrListCVMap::iterator clist=m_indexchanvec.find(chan);
    if (clist==m_indexchanvec.end()) {
      // channel not seen before, create a new entry in map and vector
      std::pair<AttrListCVMap::iterator,bool> res=
	m_indexchanvec.insert(AttrListCVMap::value_type(chan,
	std::vector<const coral::AttributeList*>()));
      clist=res.first;
      m_indexchan.push_back(chan);
    }
    clist->second.push_back(&(citr->second));
  }
  m_indexed=true;
}

#endif //  DBDATAOBJECTS_CONDATTRLISTVEC_H 
