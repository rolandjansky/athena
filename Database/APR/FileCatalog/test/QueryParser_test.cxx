/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FileCatalog/FCQueryParser.h"
#include <algorithm>
#include <iterator>
#include <iostream>
using namespace pool;
int main(){
  FCQueryParser parser("pfname LIKE %pfname123 AND guid == dfdf ");
  parser.parse();
  const std::vector<std::string>& result=parser.parseResult();
  std::copy(result.begin(),result.end(),std::ostream_iterator<std::string>(std::cout," "));
  std::cout<<std::endl;
  
  FCQueryParser parser2("pfname LIKE '%pfname123' AND guid == 'dfdf' ");
  parser2.parse();
  const std::vector<std::string>& result2=parser2.parseResult();
  std::copy(result2.begin(),result2.end(),std::ostream_iterator<std::string>(std::cout," "));
  std::cout<<std::endl;
  if( !parser2.isMetaQuery() ){
    std::cout<<"is NOT meta query: correct"<<std::endl;
  }
  FCQueryParser parser3("pfname LIKE '%pfname123' AND guid == 'dfdf' AND id==123");
  parser3.parse();
  const std::vector<std::string>& result3=parser3.parseResult();
  std::copy(result3.begin(),result3.end(),std::ostream_iterator<std::string>(std::cout," "));
  std::cout<<std::endl;
  if( parser3.isMetaQuery() ){
    std::cout<<"is meta query: correct"<<std::endl;
  }
}
