/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TRT_ConditionsData/FloatArrayStore.h"
#include "AthenaKernel/getMessageSvc.h"
#include <iostream>

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

void FloatArrayStore::dbg() const {
  std::cout << "dbg:" << m_bufmap.size() << "  " << m_buf.size() << std::endl;
}


std::ostream& operator<<(std::ostream& os, const FloatArrayStore& store){
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

std::istream& operator>>(std::istream& is, FloatArrayStore& store){
  store.clear() ;
  std::string tag ;
  is >> tag ;
  store = FloatArrayStore(tag) ;
  std::vector<int> identifiers(0) ;
  std::vector<float> data(0) ;
  constexpr int maxLines=2000000;//max 2 million lines
  constexpr int maxSize=2000000; //max size 2 million
  int nlines ;
  is >> nlines ;
  if ((nlines>=0) and (nlines<maxLines)) {
  	for(int iline = 0; iline <nlines; ++iline) {
    		int size ;
    		is >> size ;
                if ((size>=0) and (size<maxSize)){
    			identifiers.resize(size) ;
    			for(int i=0; i<size; ++i) is >> (identifiers[i]) ;
                }
                else {
                  MsgStream log(Athena::getMessageSvc(), "FloatArrayStore");
                  log << MSG::ERROR << "size of identifier is incorrect" << endmsg;
                  return is;
                }
    		is >> size ;
                if ((size>=0) and (size<maxSize)){
    			data.resize(size) ;
    			for(int i=0; i<size; ++i) is >> (data[i]) ;
                }
                else {
                  MsgStream log(Athena::getMessageSvc(), "FloatArrayStore");
                  log << MSG::ERROR << "size of data is incorrect" << endmsg;
                  return is;
                }
    		Identifier reference(identifiers.front()) ;
    		store.push_back( reference, data ) ;
    		std::vector<int>::const_iterator it = identifiers.begin() ;
    		for( ++it; it!= identifiers.end(); ++it) store.share(Identifier(*it),reference) ;
	}
  }
  return is ;
}
