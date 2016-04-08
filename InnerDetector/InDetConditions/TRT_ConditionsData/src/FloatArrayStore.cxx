/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TRT_ConditionsData/FloatArrayStore.h"

void FloatArrayStore::cleanUp(){
  std::multimap<int,Identifier> indexmap;

  map_citr itr=m_bufmap.begin();
  for (;itr!=m_bufmap.end();++itr)
    indexmap.insert(std::pair<int,Identifier>(std::abs(itr->second),itr->first));
  std::vector< std::vector<float> > tmp(m_buf);
  dbg();
  this->clear();
  int i(-tmp.size());
  for (;i<(int)tmp.size();i++){
    //std::cout << indexmap.count(i) << " " << i << std::endl;
    bool first(true);
    Identifier shared_id;
    std::multimap<int,Identifier>::iterator pos;
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

std::ostream& operator<<(std::ostream& os, const FloatArrayStore& store)
{
  os << store.tag() << std::endl ;
  // it is a bit of a heck, but the FloatArrayStore doesn't allow for more.
  // first make an inverse map
  std::map<int, std::vector<Identifier> > indextoidentmap ;
  for( FloatArrayStore::map_citr it = store.cbegin(); it!=store.cend(); ++it)
    indextoidentmap[it->second].push_back(it->first) ;
  // store the number of lines
  os << indextoidentmap.size() << std::endl ;
  // now store all elements, as follows
  // each line is: #idents ident1 ident2 ...indentN vectorsize element1 element2 ... elementN
  for( std::map<int, std::vector<Identifier> >::const_iterator it = indextoidentmap.begin() ;
       it != indextoidentmap.end(); ++it) {
    os << it->second.size() << " " ;
    for( std::vector<Identifier>::const_iterator vit = it->second.begin() ;
	 vit != it->second.end(); ++vit)
      os << *vit << " " ;
    const std::vector<float>& data = store[it->second.front()] ;
    os << data.size() << " " ;
    for( std::vector<float>::const_iterator vit = data.begin() ;
	 vit != data.end(); ++vit)
      os << *vit << " " ;
    os << std::endl ;
  }
  return os ;
}

std::istream& operator>>(std::istream& is, FloatArrayStore& store)
{
  store.clear() ;
  std::string tag ;
  is >> tag ;
  store = FloatArrayStore(tag) ;
  std::vector<int> identifiers(0) ;
  std::vector<float> data(0) ;
  int nlines ;
  is >> nlines ;
  if (nlines>=0&&nlines<=INT_MAX)
  	for(int iline = 0; iline <nlines; ++iline) {
    		int size ;
    		is >> size ;
                if (size>=0&&size<=INT_MAX){
    			identifiers.resize(size) ;
    			for(int i=0; i<size; ++i) is >> (identifiers[i]) ;
			}
    		is >> size ;
                if (size>=0&&size<=INT_MAX){
    			data.resize(size) ;
    			for(int i=0; i<size; ++i) is >> (data[i]) ;
			}
    		Identifier reference(identifiers.front()) ;
    		store.push_back( reference, data ) ;
    		std::vector<int>::const_iterator it = identifiers.begin() ;
    		for( ++it; it!= identifiers.end(); ++it) store.share(Identifier(*it),reference) ;
  		}
  return is ;
}
