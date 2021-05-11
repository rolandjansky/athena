/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**FCdeleteLFN.cpp -- FileCatalog command line tool to delete the LFN.
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
  std::cout<<"usage: FCdeleteLFN [-q query -l lfname -u contactstring -h]" <<std::endl;
}

static const char* opts[] = {"q","l","u","h",0};


int main(int argc, char** argv)
{
  SystemTools::initGaudi();

  std::string  myuri;
  std::string  mylfn;
  std::string  myquery;
  try{
    CommandLine commands(argc,argv);
    commands.CheckOptions(opts);

    if( commands.Exists("u") ){
      myuri=commands.GetByName("u");
    }else{
      myuri=SystemTools::GetEnvStr("POOL_CATALOG");
    }
    if( commands.Exists("q") ){
      myquery=commands.GetByName("q");
    }
    if( commands.Exists("l") ){
      mylfn=commands.GetByName("l");
    }
    if( commands.Exists("h") ){
      printUsage();
      exit(0);
    }
  }catch(std::string& strError){
    std::cerr << "Error: command parsing error "<<strError<<std::endl;
    exit(0);
  }

  if( mylfn.empty() && myquery.empty() ){
    printUsage();
    std::cerr<<"Error: must specify pfname using -l, query using -q"<<std::endl;
    exit(0);
  }
  try{
    auto mycatalog = std::make_unique<IFileCatalog>();
    pool::URIParser p( myuri );
    p.parse();
    mycatalog->setWriteCatalog(p.contactstring());
    mycatalog->connect();
    mycatalog->start();
    if( !myquery.empty() ){
       std::cerr << "Query option not supported" << std::endl;
       exit(2);
    }else if( !mylfn.empty() ) {
       mycatalog->deleteLFN(mylfn);
    }
    mycatalog->commit();
    mycatalog->disconnect();
  }catch (const pool::Exception& er){
    std::cerr<<er.what()<<std::endl;
    exit(1);
  }catch (const std::exception& er){
    std::cerr<<er.what()<<std::endl;
    exit(1);
  }
}


