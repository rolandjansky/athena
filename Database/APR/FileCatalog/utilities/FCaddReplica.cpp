/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//$Id: FCaddReplica.cpp 509054 2012-07-05 13:33:16Z mnowak $
/**FCaddReplica.cpp -- FileCatalog command line tool to add replica pfn 
  @author Zhen Xie
  @author Maria Girone
  @date: 02/03/2005 Z.X.
  set default logging to Warning if no POOL_OUTMSG_LEVEL is set; 
  separate logging stream to std::cerr, output stream to std::cout.
  @date: 07/04/2005 Z.X.
  adopt to split catalog interface
*/
#include "FileCatalog/CommandLine.h"
#include "FileCatalog/IFileCatalog.h"
#include "POOLCore/Exception.h"
#include <memory>

using namespace pool;

void printUsage(){
  std::cout<<"usage: FCaddReplica -r replica [-p pfname -g guid -u contactstring -h]" <<std::endl;
}

static const char* opts[] = {"r","p","g","u","h",0};


int main(int argc, char** argv) {

  std::string  myuri;
  std::string  mypfn;
  std::string  myrpf;
  FileCatalog::FileID myguid;
  try{
    CommandLine commands(argc,argv);
    commands.CheckOptions(opts);
    
    if( commands.Exists("u") ){
      myuri=commands.GetByName("u");
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
      exit(0);
    }
  }catch(std::string& strError){
    std::cerr << "Error: command parsing error "<<strError<<std::endl;
    exit(-1);
  }
  
  if( myrpf.empty() ){
    printUsage();
    std::cerr<<"Error: must specify replica name using -r"<<std::endl;
    exit(0);
  }
  if( mypfn.empty()&& myguid.empty() ){
    printUsage();
    std::cerr<<"Error: must specify PFN using -p or guid using -g"<<std::endl;
    exit(0);
  }
  try{
    std::auto_ptr<IFileCatalog> mycatalog(new IFileCatalog);
    mycatalog->setWriteCatalog(myuri);
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
    exit(1);
  }catch (const std::exception& er){
    std::cerr<<er.what()<<std::endl;
    exit(1);
  }
}

