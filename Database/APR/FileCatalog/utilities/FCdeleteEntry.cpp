/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**FCdeleteEntry.cpp -- FileCatalog command line tool to delete all entries associated with given file.
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
  std::cout<<"usage: FCdeleteEntry [-q query -l lfname -p pfname -u contactstring -h]" <<std::endl;
}


int main(int argc, char** argv)
{
  SystemTools::initGaudi();
  
  std::string  myuri;
  std::string  mylfn;
  std::string  mypfn;
  std::string  myquery;
  try{
    CommandLine commands(argc,argv);
    const char* opts[] = {"q","l","p","u","h",0};
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
    if( commands.Exists("p") ){
      mypfn=commands.GetByName("p");
    }
    
    if( commands.Exists("h") ){
      printUsage();
      return 0;
    }
  }catch(std::string& strError){
    std::cerr << "Error: command parsing error "<<strError<<std::endl;
    return 0;
  }
  
  if( mylfn.empty() && mypfn.empty() && myquery.empty() ){
    printUsage();
    std::cerr<<"Error: must specify lfname using -l , pfname using -p, query using -q"<<std::endl;
    return 0;
  }

  if( !mylfn.empty() && !mypfn.empty() ){
    printUsage();
    std::cerr<<"Error: can only delete either PFN or LFN"<<std::endl;
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
    }else if( !mylfn.empty() ){
       mycatalog->deleteFID( mycatalog->lookupLFN( mylfn ) );
    }else if( !mypfn.empty() ) {
       mycatalog->deleteFID( mycatalog->lookupPFN( mypfn ) );
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


