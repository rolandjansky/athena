/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DETDESCRCOND_DETSTATUSMAP_H
#define DETDESCRCOND_DETSTATUSMAP_H
// DetStatusMap.h - class to hold details of all subdetector status
// can be written to POOL for ESD/AOD files
// Richard Hawkings, started 13/12/06

#include "AthenaKernel/CLASS_DEF.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/MsgStream.h"
#include "DetDescrConditions/DetStatus.h"
#include <string>
#include <vector>
#include <algorithm>

class DetStatusMap : public DataObject {
 public:
  typedef std::pair<std::string,DetStatus> DetStatusPair;
  typedef std::vector<DetStatusPair>       StatusMap;
  typedef StatusMap::const_iterator        const_iterator;

  // Add this to allow python iteration over the contents.
  typedef const_iterator iterator;

  DetStatusMap();
  virtual ~DetStatusMap();

  // access methods to map
  unsigned int   size() const;
  const_iterator begin() const;
  const_iterator end() const;
  const_iterator find(const std::string& name) const;

  // set methods
  void clear();
  void add(const std::string& name, const DetStatus& status);

  // print method
  MsgStream& toOutputStream(MsgStream& os) const;

 private:
  friend class DetStatusMapCnv_p1;

  class PairSort {
  public:
    bool operator () (const DetStatusPair& x, const DetStatusPair& y) 
    { return (x.first < y.first); }
    bool operator () (const DetStatusPair& x, const std::string& y) 
    { return (x.first < y); }
  };

  StatusMap m_statmap;
};

CLASS_DEF( DetStatusMap , 264405569 , 1 )

inline DetStatusMap::~DetStatusMap() {}

inline unsigned int DetStatusMap::size() const { return m_statmap.size(); }

inline DetStatusMap::const_iterator DetStatusMap::begin() const 
{ return m_statmap.begin(); }

inline DetStatusMap::const_iterator DetStatusMap::end() const 
{ return m_statmap.end(); }

inline DetStatusMap::const_iterator DetStatusMap::find(const std::string& name) const 
{
  const_iterator result = std::lower_bound(m_statmap.begin(), 
                                           m_statmap.end(),
                                           name, PairSort()) ;
  if ( result != m_statmap.end() && (*result).first == name )  return (result) ;
  return m_statmap.end() ;
}

inline void DetStatusMap::clear() { m_statmap.clear(); }

inline void DetStatusMap::add(const std::string& name, const DetStatus& status)
{ 
  m_statmap.emplace_back( name, status ) ;
  std::sort( m_statmap.begin() , m_statmap.end() , PairSort() );
}

inline MsgStream& DetStatusMap::toOutputStream(MsgStream& os) const {
  os << "DetStatusMap of size: " << m_statmap.size() << endmsg;
  for (StatusMap::const_iterator itr=m_statmap.begin();itr!=m_statmap.end();
       ++itr) os << itr->first << " : " << itr->second.colour() << " (" <<
		itr->second.deadFrac() << "," << itr->second.deadThrust() <<
		")" << endmsg;
  return os;
}

#endif // DETDESCRCOND_DETSTATUSMAP_H
