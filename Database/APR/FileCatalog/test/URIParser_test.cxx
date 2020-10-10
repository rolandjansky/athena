/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//$Id: URIParser_test.cxx 457829 2011-09-08 13:13:25Z mnowak $
/*
  a test driver for class URIParser.
  Author: Zhen Xie
*/
#include <string>
#include <iostream>
#include "FileCatalog/URIParser.h"
#include "POOLCore/Exception.h"

using namespace pool;

int main(){
  std::string uri="mysqlcatalog_mysql://user@localhost:3306/mydb";
  URIParser p(uri);
  try{
    p.parse();p.dump();
    std::cout << "[OVAL]----------"<<std::endl;
    p.setURI("xmlcatalog_ftp://pc01.cern.ch/mycatalog.xml");
    p.parse();p.dump();  
    std::cout << "[OVAL]----------"<<std::endl;
    p.setURI("xmlcatalog_http://pc02.cern.ch/mycatalog.xml");
    p.parse();p.dump();  
    std::cout << "[OVAL]----------"<<std::endl;
    p.setURI("file:/tmp/mycatalog.xml");
    p.parse();p.dump(); 
    std::cout << "[OVAL]----------"<<std::endl;
    p.setURI("xmlcatalog_file:/tmp/mycatalog.xml");
    p.parse();p.dump(); 
    std::cout << "[OVAL]----------"<<std::endl;
    p.setURI("edgcatalog_http://ciccio.cern.ch/mycatalog");
    p.parse();p.dump(); 
  }catch(const pool::Exception& er){
    std::cerr <<er.what()<< std::endl;
    return 1;
  }
}




