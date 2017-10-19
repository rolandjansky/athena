// --*-- c++ --*--
/*

Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

Author: Sami Kama 2017
*/
// Author: Sami Kama

#ifndef ALP_UTILS_H
#define ALP_UTILS_H
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <chrono>
#include <ctime>
#include <iostream>

namespace ALPUtils{
  inline void printPtree(const boost::property_tree::ptree& args, std::string level) {
    boost::property_tree::ptree::const_iterator it,itend=args.end();
    level+=" ";
    for(it=args.begin();it!=itend;++it){
      std::string val(it->second.get_value<std::string>());
      boost::algorithm::trim(val);
      std::cout<<level<<it->first<<" : "<<val<<std::endl;
      printPtree(it->second,level);
    }
  }
  
  inline const std::string getTimeTag() {
    auto tnow=std::chrono::system_clock::now();
    std::time_t t=std::chrono::system_clock::to_time_t(tnow);
    char buff[100];
    auto countMS=std::chrono::duration_cast<std::chrono::milliseconds>(tnow.time_since_epoch()).count();
    auto countS=std::chrono::duration_cast<std::chrono::seconds>(tnow.time_since_epoch()).count();
    if (std::strftime(buff, sizeof(buff), "%Y-%b-%d %H:%M:%S", std::localtime(&t))){
      snprintf(buff+strlen(buff),100-strlen(buff),",%03ld ",countMS-countS*1000);
    }
    return std::string(buff);
  } 

  inline bool dump2File(const std::string &fname,const boost::property_tree::ptree& args) {
    if(!fname.empty()){
      try{
	boost::property_tree::write_xml(fname,args,std::locale());
      }catch(boost::property_tree::xml_parser::xml_parser_error &ex){
	std::cerr<<ALPUtils::getTimeTag()<<"Caught exception when writing xml to file \""
		 <<fname<<"\" exception is \""<<ex.what()<<"\""<<std::endl;
      }
      return false;
    }else{
      std::cerr<<ALPUtils::getTimeTag()<<" File name can not be empty"<<std::endl;
      return false;
    }
    return true;
  }
  
}

#endif
