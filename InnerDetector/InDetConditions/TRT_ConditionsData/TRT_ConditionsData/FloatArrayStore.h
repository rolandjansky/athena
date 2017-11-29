/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRTCONDITIONSDATA_FLOATARRAYSTORE_H
#define TRTCONDITIONSDATA_FLOATARRAYSTORE_H
/********************************************************************

NAME:     FloatArrayStore
PACKAGE:  TRT_ConditionsData

AUTHORS:  Jorgen Beck Hansen, Peter Hansen
CREATED:  1/8/05

PURPOSE:  Access and manipulate an indexed storage
          
********************************************************************/

// INCLUDE HEADER FILES:

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include "Identifier/Identifier.h"
#include "AthenaKernel/CLASS_DEF.h"

/** class FloatArrayStore
Access and manipulate an indexed storage of float vectors
@author Jorgen Beck Hansen
*/
class FloatArrayStore{
public:

  typedef std::less<Identifier> lessp;
  typedef std::map<Identifier,int,lessp> BufferMap;
  typedef BufferMap::const_iterator map_citr;
  typedef BufferMap::iterator map_itr;
  typedef std::vector< std::vector<float> > Buffer;

/** constructors, optionally with a tag */
  FloatArrayStore();
  FloatArrayStore(const std::string tag);
/** destructor  */
  virtual ~FloatArrayStore();
  
/** Tag accessor */
  const std::string& tag() const;
/** Size accessor */
  int numberOfIDs() const;
/** Overload [] const access */
  const std::vector<float>& operator[](const Identifier& ident) const;
/** returns flag for index sharing data */
  bool sharedID(const Identifier& ident) const;
/** returns flag for index existing in the std::map */
  bool existID(const Identifier& ident) const;
/** print method */
  void dbg() const;
/** begin/end const iterators */
  map_citr cbegin() const;
  map_citr cend() const;
  
/** Overload [] modifier */
  std::vector<float>& operator[](const Identifier& ident);
/** makes index ident share data with referenceIdent */
  void share(const Identifier& ident, const Identifier& referenceIdent);
/** makes new indices */
  void push_back(const Identifier& ident, const std::vector<float>& value);
  void push_back(const std::vector<Identifier>& identvec, const std::vector<float>& value);
/** remove new index */
  void removeID(const Identifier& ident);
/** remove all */
  void clear();
/** remove data with no index pointing to it */
  void cleanUp();
/** begin/end iterators */
  map_itr begin();
  map_itr end();
/** return stored vector */
  const Buffer& buffer() const { return m_buf ; }
  

private:
  
  std::string m_tag;                //!< The tag std::string
  BufferMap m_bufmap;               //!< The std::map
  Buffer m_buf;                     //!< The std::vector<float> stored in each second element
  
};

CLASS_DEF(FloatArrayStore,170308771,1)

// inline methods
inline FloatArrayStore::FloatArrayStore() : m_tag("Undefined") { }

inline FloatArrayStore::FloatArrayStore(const std::string tag) : m_tag(tag) { }

inline FloatArrayStore::~FloatArrayStore() {}

inline void FloatArrayStore::clear() {m_bufmap.clear(); m_buf.clear(); }

inline void FloatArrayStore::push_back
     (const Identifier& ident, const std::vector<float>& value){
  m_buf.push_back(value);
  m_bufmap[ident]= m_buf.size()-1;
}

inline void FloatArrayStore::push_back(const std::vector<Identifier>& identvec, const std::vector<float>& value){
  m_buf.push_back(value);
  size_t index = m_buf.size()-1;
  for(std::vector<Identifier>::const_iterator it = identvec.begin() ; it != identvec.end(); ++it)
    m_bufmap[*it] = index ;
}

inline int FloatArrayStore::numberOfIDs() const
{ return m_bufmap.size(); }

inline void FloatArrayStore::removeID(const Identifier& ident)
{ m_bufmap.erase(ident); }

inline bool FloatArrayStore::existID(const Identifier& ident) const
{ return m_bufmap.find(ident)!=m_bufmap.end(); }

inline bool FloatArrayStore::sharedID(const Identifier& ident) const { return existID(ident)? m_bufmap.find(ident)->second <0 : false; }

inline void FloatArrayStore::dbg() const {std::cout << "dbg:" << m_bufmap.size() << "  " << m_buf.size() << std::endl;}

inline const std::vector<float>& FloatArrayStore::operator[]
(const Identifier& ident) const
{return m_buf[std::abs(m_bufmap.find(ident)->second)]; }

inline std::vector<float>& FloatArrayStore::operator[](const Identifier& ident)
  {return m_buf[std::abs(m_bufmap[ident])]; }

inline void FloatArrayStore::share
(const Identifier& ident, const Identifier& referenceIdent) {
  if (existID(referenceIdent)){
    m_bufmap[ident] = -std::abs(m_bufmap[referenceIdent]);
    m_bufmap[referenceIdent] = -std::abs(m_bufmap[referenceIdent]);
  }
}

inline const std::string& FloatArrayStore::tag() const {return m_tag;}
 
inline  FloatArrayStore::map_itr
 FloatArrayStore::begin() { return m_bufmap.begin(); }
 
inline  FloatArrayStore::map_itr
 FloatArrayStore::end() { return m_bufmap.end(); }
 
inline  FloatArrayStore::map_citr
 FloatArrayStore::cbegin() const { return m_bufmap.begin(); }
 
inline  FloatArrayStore::map_citr FloatArrayStore::cend() const { return m_bufmap.end(); }

std::ostream& operator<<(std::ostream& os, const FloatArrayStore& store) ;
std::istream& operator>>(std::istream& is, FloatArrayStore& store) ;

#endif 
