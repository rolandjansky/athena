/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CORACOOL_CORACOOLOBJECT_H
#define CORACOOL_CORACOOLOBJECT_H
// CoraCoolObject.h - interface to an object read from a CoraCool folder
// The object has COOL properties (start/end IOV, channel)
// and a payload consisting of a vector of coral::AttributeList 
// Richard Hawkings, started 10/2006

#include <vector>
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeList.h"
#include "CoralBase/AttributeListSpecification.h"
#include "CoolKernel/types.h"
#include "CoolKernel/ValidityKey.h"
#include "CoolKernel/ChannelId.h"

class CoraCoolObject {
 public:
  typedef std::vector<coral::AttributeList> AttrListVec;
  typedef AttrListVec::const_iterator const_iterator;
  typedef AttrListVec::size_type size_type;

  // constructor - the object initially has no payload data
  // payload data is added after construction using the add method
  CoraCoolObject(const cool::ValidityKey& since, 
		 const cool::ValidityKey& until, const cool::ChannelId& chan);
  // constructor specifying external AttributeListSpecification to use as a
  // shared specification for all the attribute lists
  CoraCoolObject(const cool::ValidityKey& since, 
		 const cool::ValidityKey& until, const cool::ChannelId& chan,
		 const coral::AttributeListSpecification* spec);
  ~CoraCoolObject();

  // data accessors
  cool::ValidityKey since() const;
  cool::ValidityKey until() const;
  cool::ChannelId channelId() const;
  // const iterators over the data members
  const_iterator begin() const;
  const_iterator end() const;
  size_type size() const;

  // add an AttributeList of payload data
  void add(const coral::AttributeList& data);

 private:
  cool::ValidityKey m_since;
  cool::ValidityKey m_until;
  cool::ChannelId m_chan;
  AttrListVec m_payload;
  const coral::AttributeListSpecification* m_spec;
};

inline cool::ValidityKey CoraCoolObject::since() const { return m_since; }
inline cool::ValidityKey CoraCoolObject::until() const { return m_until; }
inline cool::ChannelId CoraCoolObject::channelId() const 
{return m_chan; }

inline CoraCoolObject::const_iterator CoraCoolObject::begin() const
{ return m_payload.begin(); }

inline CoraCoolObject::const_iterator CoraCoolObject::end() const
{ return m_payload.end(); }

inline CoraCoolObject::size_type CoraCoolObject::size() const 
{return m_payload.size(); } 

#endif //  CORACOOL_CORACOOLOBJECT_H
