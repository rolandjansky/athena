/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/** FCdeletePFN.cpp -- FileCatalog command line tool to delete the selected PFN 
    @author: Zhen Xie
*/

#include "FileCatalog/CommandLine.h"
#include "FileCatalog/IFileCatalog.h"
#include "FileCatalog/URIParser.h"
#include "POOLCore/Exception.h"
#include "POOLCore/SystemTools.h"
#include <memory>

using namespace pool;

void printUsage(){
  std::cout<<"usage: FCdeletePFN [-q query -p pfname -u contactstring -h]" <<std::endl;
}


int main(int argc, char** argv)
{
  SystemTools::initGaudi();
  
  std::string  myuri;
  std::string  mypfn;
  std::string  myquery;
  try{
    CommandLine commands(argc,argv);
    const char* opts[] = {"p","q","u","h",0};
    commands.CheckOptions(opts);

    if( commands.Exists("u") ){
      myuri=commands.GetByName("u");
    }else{
      myuri=SystemTools::GetEnvStr("POOL_CATALOG");
    }    
    if( commands.Exists("q") ){
      myquery=commands.GetByName("q");
    }
    if( commands.Exists("p") ){
      mypfn=commands.GetByName("p");
    }
    if( commands.Exists("h") ){
      printUsage();
      return 0;
    }
  }catch(std::string& strError){
    std::cerr<< "Error: command parsing error "<<strError<<std::endl;
    return 0;
  }
  
  if( mypfn.empty() && myquery.empty() ){
    printUsage();
    std::cerr<<"Error: must specify pfname using -p, query using -q"<<std::endl;
    return 0;
  }
  try{
    std::unique_ptr<IFileCatalog> mycatalog(new IFileCatalog);
    pool::URIParser p( myuri );
    p.parse();
    mycatalog->setWriteCatalog(p.contactstring());
    mycatalog->connect();
    mycatalog->start();
    if( !myquery.empty() ){
      std::cerr << "Query option not supported" << std::endl;
      return 2;
    }else if( !mypfn.empty() ) {
      mycatalog->deletePFN(mypfn);
    }
    mycatalog->commit();  
    mycatalog->disconnect();
  }catch (const pool::Exception& er){
    std::cerr<<er.what()<<std::endl;
    return 1;
  }catch (const std::exception& er){
    std::cerr<<er.what()<<std::endl;
    return 1;
  }
}


