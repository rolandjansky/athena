/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRTCONDITIONSDATA_ARRAYSTORE_H
#define TRTCONDITIONSDATA_ARRAYSTORE_H
/********************************************************************

NAME:     
PACKAGE:  

AUTHORS:  Jorgen Beck Hansen
CREATED:  1/8/05

PURPOSE:  Access and manipulate an indexed storage
          

********************************************************************/

// INCLUDE HEADER FILES:

#include <vector>
#include <map>
#include <string>
#include <iostream>

/** @class ArrayStore
Access and manipulate an indexed storage
@author Jorgen Beck Hansen
*/
template <typename IdType = int, typename StorageType = std::vector<float> >
class ArrayStore{
public:

  typedef std::less<IdType> lessp;
  typedef std::map<IdType,int,lessp> BufferMap;
  typedef typename BufferMap::const_iterator map_citr;
  typedef typename BufferMap::iterator map_itr;
  
/** constructors, optionally with a tag */
  ArrayStore();
  ArrayStore(const std::string tag);
/** destructor  */
  virtual ~ArrayStore();
  
/** Tag accessor */
  const std::string& tag() const;
/** Size accessor */
  int numberOfIDs();
/** Overload [] const access */
  const StorageType& operator[](const IdType& ident) const;
/** returns flag for index sharing data */
  bool sharedID(const IdType& ident);
/** returns flag for index existing in the std::map */
  bool existID(const IdType& ident);
/** print method */
  void dbg() const;
/** begin/end const iterators */
  map_citr cbegin() const;
  map_citr cend() const;
  
/** Overload [] modifier */
  StorageType& operator[](const IdType& ident);
/** makes index ident share data with referenceIdent */
  void share(const IdType& ident, const IdType& referenceIdent);
/** makes new index */
  void push_back(const IdType& ident, const StorageType& value);
/** remove index */
  void removeID(const IdType& ident);
/** remove all indices */
  void clear();
/** remove data with no index pointing to it */
  void cleanUp();
/** begin/end iterators */
  map_itr begin();
  map_itr end();
  
private:
  
std::string m_tag;                  //!< The tag std::string
BufferMap m_bufmap;                 //!< The std::map
std::vector<StorageType> m_buf;     //!< The object stored in each second element
  
};

// inline methods
template <typename IdType, typename StorageType>
inline ArrayStore<IdType, StorageType>::ArrayStore() : m_tag("Undefined") { }

template <typename IdType, typename StorageType> 
inline ArrayStore<IdType, StorageType>::ArrayStore(const std::string tag) : m_tag(tag) { }

template <typename IdType, typename StorageType>
inline ArrayStore<IdType, StorageType>::~ArrayStore() {}

template <typename IdType, typename StorageType>
inline void ArrayStore<IdType, StorageType>::clear() {m_bufmap.clear(); m_buf.clear(); }

template <typename IdType, typename StorageType>
inline void ArrayStore<IdType, StorageType>::push_back(const IdType& ident, const StorageType& value){
  m_buf.push_back(value);
  m_bufmap[ident]= m_buf.size()-1;
}

template <typename IdType, typename StorageType>
inline int ArrayStore<IdType, StorageType>::numberOfIDs() { return m_bufmap.size(); }

template <typename IdType, typename StorageType>
inline void ArrayStore<IdType, StorageType>::removeID(const IdType& ident) { m_bufmap.erase(ident); }

template <typename IdType, typename StorageType>
inline bool ArrayStore<IdType, StorageType>::existID(const IdType& ident) { return m_bufmap.find(ident)!=m_bufmap.end(); }

template <typename IdType, typename StorageType>
inline bool ArrayStore<IdType, StorageType>::sharedID(const IdType& ident) { return existID(ident)? m_bufmap.find(ident)->second <0 : false; }

template <typename IdType, typename StorageType>
inline void ArrayStore<IdType, StorageType>::dbg() const {std::cout << "dbg:" << m_bufmap.size() << "  " << m_buf.size() << std::endl;}

template <typename IdType, typename StorageType>
inline const StorageType& ArrayStore<IdType, StorageType>::operator[](const IdType& ident) const {return m_buf[std::abs(m_bufmap.find(ident)->second)]; }

template <typename IdType, typename StorageType>
inline StorageType& ArrayStore<IdType, StorageType>::operator[](const IdType& ident) {return m_buf[std::abs(m_bufmap[ident])]; }

template <typename IdType, typename StorageType>
inline void ArrayStore<IdType, StorageType>::share(const IdType& ident, const IdType& referenceIdent) {
  if (existID(referenceIdent)){
    m_bufmap[ident] = -std::abs(m_bufmap[referenceIdent]);
    m_bufmap[referenceIdent] = -std::abs(m_bufmap[referenceIdent]);
  }
}

template <typename IdType, typename StorageType>
inline const std::string& ArrayStore<IdType, StorageType>::tag() const {return m_tag;}
 
template <typename IdType, typename StorageType>
inline typename ArrayStore<IdType, StorageType>::map_itr ArrayStore<IdType, StorageType>::begin() { return m_bufmap.begin(); }
 
template <typename IdType, typename StorageType>
inline typename ArrayStore<IdType, StorageType>::map_itr ArrayStore<IdType, StorageType>::end() { return m_bufmap.end(); }
 
template <typename IdType, typename StorageType>
inline typename ArrayStore<IdType, StorageType>::map_citr ArrayStore<IdType, StorageType>::cbegin() const { return m_bufmap.begin(); }
 
template <typename IdType, typename StorageType>
inline typename ArrayStore<IdType, StorageType>::map_citr ArrayStore<IdType, StorageType>::cend() const { return m_bufmap.end(); }

#ifndef TRTCONDITIONSDATA_ARRAYSTORE_ICC
#define TRTCONDITIONSDATA_ARRAYSTORE_ICC

template <typename IdType, typename StorageType>
void ArrayStore<IdType, StorageType>::cleanUp(){
  std::multimap<int,IdType> indexmap;

  map_citr itr=m_bufmap.begin();
  for (;itr!=m_bufmap.end();++itr)
    indexmap.insert(std::pair<int,IdType>(std::abs(itr->second),itr->first));
  std::vector<StorageType> tmp(m_buf);
  dbg();
  this->clear();
  int i(-tmp.size());
  for (;i<(int)tmp.size();i++){
    //std::cout << indexmap.count(i) << " " << i << std::endl;
    bool first(true);
    IdType shared_id;
    typename std::multimap<int,IdType>::iterator pos;
    for ( pos=indexmap.lower_bound(i); pos!=indexmap.upper_bound(i); ++pos){
      if (first){
	first=false;
	shared_id=pos->second;
	this->push_back(shared_id,tmp[std::abs(i)]);
      } else
	this->share(pos->second,shared_id);
    }
  }
}
#endif

#endif 
