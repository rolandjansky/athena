/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAGEVENT_TAGFRAGMENTCOLLECTION_H
#define TAGEVENT_TAGFRAGMENTCOLLECTION_H
/********************************************************************

NAME:     TagFragmentCollection.h
Package : offline/PhysicsAnlaysis/EventTag/TagEvent

AUTHOR:   Ketevi A. Assamagan
CREATED:  January 2006

PURPOSE:  Tag fragment list
********************************************************************/

#include <string>
#include <vector>
#include <map>

class TagFragmentCollection {

 public:

  typedef std::pair<std::string, double> DataPair;

  /** constructor - destructor */
  TagFragmentCollection(){}
  ~TagFragmentCollection(){ m_attrData.clear(); }

  /** Copy constructor */
  TagFragmentCollection( const TagFragmentCollection& rhs );

  /** Assignement operator */
  TagFragmentCollection& operator=( const TagFragmentCollection& rhs );

  /** insert something */
  void insert(const std::string& name, const double val) {
     m_attrData.emplace_back( name, val );
  }

  /** return all attributes */
  const std::vector<DataPair> & attrData() const { return m_attrData; }

 protected:
  
  /** Tag data associated with this element */
  std::vector<DataPair>  m_attrData;

};

#endif


