/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**FCaddReplica.cpp -- FileCatalog command line tool to add replica pfn 
  @author Zhen Xie
  @author Maria Girone
*/

#include "FileCatalog/CommandLine.h"
#include "FileCatalog/IFileCatalog.h"
#include "FileCatalog/URIParser.h"
#include "POOLCore/Exception.h"
#include "POOLCore/SystemTools.h"
#include <memory>

using namespace pool;

void printUsage(){
  std::cout<<"usage: FCaddReplica -r replica [-p pfname -g guid -u contactstring -h]" <<std::endl;
}


int main(int argc, char** argv)
{
  SystemTools::initGaudi();

  std::string  myuri;
  std::string  mypfn;
  std::string  myrpf;
  FileCatalog::FileID myguid;
  try{
    CommandLine commands(argc,argv);
    const char* opts[] = {"r","p","g","u","h",0};
    commands.CheckOptions(opts);
    
    if( commands.Exists("u") ){
      myuri=commands.GetByName("u");
    } else {
      myuri=SystemTools::GetEnvStr("POOL_CATALOG");
    } 
    if( commands.Exists("p") ){
      mypfn=commands.GetByName("p");
    }
    if( commands.Exists("r") ){
      myrpf=(commands.GetByName("r"));
    }
    if( commands.Exists("g") ){
      myguid=(commands.GetByName("g"));
    }
    if( commands.Exists("h") ){
      printUsage();
      return 0;
    }
  }catch(std::string& strError){
    std::cerr << "Error: command parsing error "<<strError<<std::endl;
    return -1;
  }
  
  if( myrpf.empty() ){
    printUsage();
    std::cerr<<"Error: must specify replica name using -r"<<std::endl;
    return 0;
  }
  if( mypfn.empty()&& myguid.empty() ){
    printUsage();
    std::cerr<<"Error: must specify PFN using -p or guid using -g"<<std::endl;
    return 0;
  }
  try{
    std::unique_ptr<IFileCatalog> mycatalog(new IFileCatalog);
    pool::URIParser p( myuri );
    p.parse();
    mycatalog->setWriteCatalog(p.contactstring());
    if( !mypfn.empty() ){
      mycatalog->connect();
      mycatalog->start();
      mycatalog->addReplicaPFN(mypfn,myrpf);
      mycatalog->commit();  
      mycatalog->disconnect();
    }else if( !myguid.empty() ){
      mycatalog->connect();
      mycatalog->start();
      mycatalog->addReplicaFID(myguid, myrpf);
      mycatalog->commit();  
      mycatalog->disconnect();
    }
  }catch (const pool::Exception& er){
    std::cerr<<er.what()<<std::endl;
    return 1;
  }catch (const std::exception& er){
    std::cerr<<er.what()<<std::endl;
    return 1;
  }
}

