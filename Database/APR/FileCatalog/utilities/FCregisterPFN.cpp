/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
   FCregisterPFN.cpp -- FileCatalog command line tool registerPFN
   Register a PFN in the File Catalog: a unique FileID is generated and inserted in the catalog, 
*/

#include "FileCatalog/CommandLine.h"
#include "FileCatalog/IFileCatalog.h"
#include "FileCatalog/URIParser.h"
#include "POOLCore/Exception.h"
#include "POOLCore/SystemTools.h"
#include <memory>

using namespace pool;

void printUsage(){
  std::cout<< "usage: registerPFN -p pfname [ -u contactstring -t filetype -g guid -h ]" <<std::endl;
}


int main(int argc, char** argv)
{
  SystemTools::initGaudi();
  
  std::string myuri;
  std::string mypfn;
  std::string myfiletype;
  std::string myguid;
  try{
    CommandLine commands(argc,argv);
    const char* opts[] = {"p","t","g","u","h",0};
    commands.CheckOptions(opts);

    if( commands.Exists("u") ){
      myuri=commands.GetByName("u");
    }else{
      myuri=SystemTools::GetEnvStr("POOL_CATALOG");
    } 
    if( commands.Exists("p") ){
      mypfn=commands.GetByName("p");
    }
    if( commands.Exists("t") ){
      myfiletype=commands.GetByName("t");
    }
    if( commands.Exists("g") ){
      myguid=(commands.GetByName("g"));
    }
    if( commands.Exists("h") ){
      printUsage();
      return 0;
    }
  }catch(std::string& strError){
    std::cerr<<"Error: command parsing error "<<strError<<std::endl;
    return 0;
  }
  
  if( mypfn.empty() ){
    printUsage();
    std::cerr<<"Error: must specify pfname using -p"<<std::endl;
    return 0;
  }
  try{  
    std::unique_ptr<IFileCatalog> mycatalog(new IFileCatalog);
    pool::URIParser p( myuri );
    p.parse();
    mycatalog->setWriteCatalog(p.contactstring());
    mycatalog->connect();
    mycatalog->start();
    mycatalog->registerPFN(mypfn, myfiletype, myguid);
    mycatalog->commit();  
    mycatalog->disconnect();
    std::cout<<myguid<<std::endl;
  }catch (const pool::Exception& er){
    std::cerr<<er.what()<<std::endl;
    return 1;
  }catch (const std::exception& er){
    std::cerr<<er.what()<<std::endl;
    return 1;
  }
}





