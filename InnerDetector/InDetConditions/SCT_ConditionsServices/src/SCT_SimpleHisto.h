/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/**
 *  SCT_SimpleHisto.h
 *  C++ projects
 *
 *  Created by sroe on 13/11/2009.
 *
 **/
#ifndef SCT_SimpleHisto_h
#define SCT_SimpleHisto_h

#include <map>
#include <string>
#include <sstream>

namespace SCT_ConditionsServices{
  typedef std::map<float, unsigned int> S_t;
  
  ///Initialize a map to be used as a histogram
  bool 
  init(S_t & s, const float xlo, const float xhi, const unsigned int nbins){
    if ((xhi <= xlo) or (nbins<2)) return false;
    float increment=(xhi-xlo)/nbins;
    float acc(xlo);
    unsigned int init(0);
    for (unsigned int i(0);i!=nbins;++i){
      s[acc]=init;
      acc+=increment;
    } 
    return (s.size()==nbins); //should be true now
  }//initHisto function
  
  //increment a bin
  int
  fill(S_t & s, const float value){
    S_t::const_iterator start=s.begin();
    S_t::const_iterator end=s.end();
    unsigned int nbins=s.size();
    if (value * nbins > ((--end)->first - start->first) * (nbins+1)) return 1;
    S_t::iterator i=s.upper_bound(value);
    if (i==start) return -1;
    ++((--i)->second);
    return 0;
  }//fill function

  //return xml string representation
  std::string
  asXmlString(const S_t & s){
    std::ostringstream os;
    os<<"<histogram>\n";
    for (S_t::const_iterator i=s.begin();i!=s.end();++i){
      os<<"<b x=\""<<i->first<<"\">"<<i->second<<"</b>\n";
    }
    os<<"</histogram>\n";
    return os.str();
  }//asXmlString
  
  std::string 
  xmlHeader(const std::string & version="1.0", const std::string & encoding="UTF-8"){
    return "<?xml version=\""+version+"\" encoding=\""+encoding+"\" standalone=\"yes\"?>";
  }
  std::string
  stylesheet(const std::string & path){
    return "<?xml-stylesheet type=\"text/xsl\" href=\""+path+"\"?>";
  }
  
}//namespace

#endif

