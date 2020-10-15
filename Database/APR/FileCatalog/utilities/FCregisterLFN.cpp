/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**FCregisterLFN.cpp -- FileCatalog command line tool to register a LFN
  @author: Zhen Xie
  @author: Maria Girone
*/

#include "FileCatalog/CommandLine.h"
#include "FileCatalog/IFileCatalog.h"
#include "FileCatalog/URIParser.h"
#include "POOLCore/Exception.h"
#include "POOLCore/SystemTools.h"
#include <memory>

using namespace pool;

void printUsage(){
  std::cout<<"usage: FCregisterLFN -l lfname -p pfname [-u contactstring -h]" <<std::endl;
}


int main(int argc, char** argv)
{
  SystemTools::initGaudi();
  
  std::string  myuri;
  std::string  mypfn;
  std::string  mylfn;
  try{
    CommandLine commands(argc,argv);
    const char* opts[] = {"p","l","u","h",0};
    commands.CheckOptions(opts);

    if( commands.Exists("u") ){
      myuri=commands.GetByName("u");
    }else{
       myuri=SystemTools::GetEnvStr("POOL_CATALOG");
    }    
    if( commands.Exists("p") ){
      mypfn=commands.GetByName("p");
    }
    if( commands.Exists("l") ){
      mylfn=(commands.GetByName("l"));
    }
    if( commands.Exists("h") ){
      printUsage();
      return 0;
    }
  }catch(std::string& strError){
    std::cerr << "Error: command parsing error "<<strError<<std::endl;
    return 0;
  }
  
  if( mypfn.empty() || mylfn.empty() ){
    printUsage();
    std::cerr<<"Error: must specify pfname using -p, lfname using -l"<<std::endl;
    return 0;
  }
  try{
    std::unique_ptr<IFileCatalog> mycatalog(new IFileCatalog);
    pool::URIParser p( myuri );
    p.parse();
    mycatalog->setWriteCatalog( p.contactstring() );
    mycatalog->connect();
    mycatalog->start();
    mycatalog->registerLFN( mycatalog->lookupPFN(mypfn), mylfn );
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


