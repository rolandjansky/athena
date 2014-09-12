/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef POOL_URIPARSER_H
#include "FileCatalog/URIParser.h"
#endif
#include <iostream>
#include <cstdlib>

namespace pool{
  
  URIParser::URIParser(){
  }
  
  URIParser::URIParser(const std::string& contactstr)
    :m_contactstr(contactstr){
  }
  
  void URIParser::dump() const{
    std::cout << "contact string "<<m_contactstr<<std::endl;
    std::cout << "prefix: "<<m_prefix<<std::endl;
    std::cout << "url: "<< m_url<<std::endl;
  }
  
  void URIParser::setURI(const std::string& contactstr){
    m_contactstr=contactstr;
    m_prefix="";
    m_url="";
  }

  void URIParser::parse()  {
    if(m_contactstr.empty()){
      std::string mystr;
      char* me= getenv("POOL_CATALOG");
      if (!me){
        mystr = "xmlcatalog_file:PoolFileCatalog.xml";
        std::cerr << "WARNING: $POOL_CATALOG is not defined\nusing default `" 
                  << mystr << "'" 
                  <<std::endl;
      }else{
        mystr=me;
      }
      m_contactstr=mystr;
    }
    //
    //look up separators
    size_t tmppos=m_contactstr.find_first_of(':');
    size_t prefixpos=m_contactstr.find_first_of('_');
    size_t startpos=0;
    
    if(prefixpos == m_contactstr.npos || prefixpos>tmppos){
      m_url=m_contactstr;
      if(tmppos != m_contactstr.npos && m_contactstr.substr(0,5)!="file:"){
        throw FCillegalContactStringException("URIParser::parse","only file: protocol is allowed for contactstring with no prefix.");
      }
    }else{
      //have prefix
      m_prefix=m_contactstr.substr(0,prefixpos);
      //if(m_prefix != "xmlcatalog" && m_prefix != "XMLFileCatalog"&& m_prefix != "mysqlcatalog" && m_prefix != "edgcatalog"){
      //	throw FCillegalContactStringException("URIParser::parse","illegal prefix");
      //}
      startpos=prefixpos+1;//new starting point
      m_url=m_contactstr.substr(startpos,m_contactstr.length()-startpos);
    }//end prefix lookup
  }
  
  const std::string URIParser::contactstring() const{
    return m_contactstr;
  }
  
  const std::string URIParser::prefix() const{
    return m_prefix;
  }
  
  const std::string URIParser::url() const{
    return m_url;
  }
  URIParser::~URIParser(){
  }
  
}//ns pool 
