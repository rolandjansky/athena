/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "XMLQueryHandle.h"
#include "CoralBase/MessageStream.h"

namespace pool {
  inline std::string
  trimmer(const std::string& buff){
    std::string tmp = buff;
    
    while (tmp.substr(0,1)==" ") {
      tmp=tmp.substr(1,tmp.npos);
    }
    while(tmp.length() && tmp.substr(tmp.length()-1,1)==" ") {
      tmp=tmp.substr(0,tmp.length()-1);
    }
    
    while (tmp.substr(0,1)=="'") {
      tmp=tmp.substr(1,tmp.npos);
    }
    while(tmp.length() && tmp.substr(tmp.length()-1,1)=="'") {
      tmp=tmp.substr(0,tmp.length()-1);
    }
    return tmp;
  }

  inline std::string
  wilder(const std::string& buff, XMLQueryHandle::WildStatus& ws){
    std::string tmp = buff;
    
    bool start=false; 
    bool end=false;
    while (tmp.substr(0,1)=="%") {
      tmp=tmp.substr(1,tmp.npos);
      end=true;
      ws = XMLQueryHandle::END;
    }
    if(tmp.empty()){return tmp;}
    while(tmp.length() && tmp.substr(tmp.length()-1,1)=="%") {
      tmp=tmp.substr(0,tmp.length()-1);
      start=true;
      ws = XMLQueryHandle::START;
    }
  
    if ( start && end )
      ws = XMLQueryHandle::MIDDLE;
    if ( !start && !end )
      ws = XMLQueryHandle::NONE;
    
    return tmp;
  }
}


pool::XMLQueryHandle::XMLQueryHandle()
  : m_nq(0)
{
}
  
  
void
pool::XMLQueryHandle::init(const std::string& query){

  coral::MessageStream xmllog( "XMLQueryHandle");

  std::vector<std::string> squ;
  std::string subquery;
  std::string tmp=query;
  std::string buff;
  size_t conc=0;
  do{ 
    buff=tmp;
    
    size_t conc1 = buff.find("and");
    size_t conc2 = buff.find("AND");
    conc = ( conc1 < conc2 ? conc1 : conc2 );
    size_t conc3 = buff.find("&&");
    conc = ( conc3 < conc ? conc3 : conc );
    
    subquery = trimmer(buff.substr(0,conc));
    squ.push_back(subquery);
    if (conc!=buff.npos)
      tmp = trimmer(buff.substr(conc+3,buff.npos));

    } while (conc != buff.npos);
    

  m_nq = squ.size();
  
  for (std::vector<std::string>::iterator isq = squ.begin(); 
       isq!=squ.end(); isq++){
    subquery = (*isq);
    
    size_t ftoken=subquery.find('=');
    if (ftoken != subquery.npos ){
      
      std::string tmp=subquery.substr(0,ftoken);
      m_attrib.push_back(trimmer(tmp));
      
      m_comman.push_back("=");
      m_wild.push_back(XMLQueryHandle::NONE);
      m_value.push_back(trimmer(subquery.substr(ftoken+1,subquery.npos)));
      
    }else{
      
      
      size_t ftoken = subquery.find(' ');
      if (ftoken == subquery.npos){
        xmllog<< coral::Warning <<"query =("<<subquery
              <<") is not supported"<<coral::MessageStream::endmsg;
        m_nq--;
        break;
      }        
      
      std::string buff1 = trimmer(subquery.substr(ftoken+1,subquery.npos));
      
      size_t stoken = buff1.find(' ');
      if (trimmer(buff1.substr(0,stoken)) != "like"){
        xmllog<< coral::Warning <<"query =("<<subquery
              <<") is not supported"<<coral::MessageStream::endmsg;
        m_nq--;
        break;
      }        
      m_attrib.push_back(trimmer(subquery.substr(0,ftoken)));
      m_comman.push_back(trimmer(buff1.substr(0,stoken)));
      std::string buff2 = trimmer(buff1.substr(stoken+1,buff1.npos));
      XMLQueryHandle::WildStatus ws;
      m_value.push_back(wilder(buff2,ws));
      m_wild.push_back(ws);
      
    }
    m_subquery.push_back(subquery);
    
  }
}

const std::string& pool::XMLQueryHandle::subquery(int i){return m_subquery[i];}

const std::string& pool::XMLQueryHandle::get_attrib(int i){return m_attrib[i];}

const std::string& pool::XMLQueryHandle::get_comman(int i){return m_comman[i];}

const std::string& pool::XMLQueryHandle::get_value(int i){ return m_value[i];}

pool::XMLQueryHandle::WildStatus pool::XMLQueryHandle::is_wild(int i){ return m_wild[i]; }

unsigned int pool::XMLQueryHandle::nqueries(){ return m_nq; }


