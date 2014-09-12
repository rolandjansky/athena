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
#include "FileCatalog/FCException.h"
#include "FileCatalog/IFCAction.h"
#include "FileCatalog/FCLeaf.h"
#include "FileCatalog/FCImpl.h"
#include "FileCatalog/FCException.h"
#include "POOLCore/Exception.h"
#include "CoralBase/MessageStream.h"
#include "CoralBase/MessageStream.h"
#include <memory>
using namespace pool;
void printUsage(){
  std::cout<<"usage: FCaddReplica -r replica [-p pfname -g guid -u contactstring -h]" <<std::endl;
}

static const char* opts[] = {"r","p","g","u","h",0};


int main(int argc, char** argv) {

  if(!::getenv( "POOL_OUTMSG_LEVEL" )){ //if not set, default to warning
    //    // pool::POOLContext::setMessageVerbosityLevel(coral::Warning);
  }else{
    coral::MessageStream pms("get threshold");
    //// pool::POOLContext::setMessageVerbosityLevel( pms.threshold() );
  }

  /*
  if( mesgsvc ){
    //all logging go to cerr
    mesgsvc->setOutputStream( std::cerr, seal::Msg::Nil );
    mesgsvc->setOutputStream( std::cerr, coral::Verbose );
    mesgsvc->setOutputStream( std::cerr, coral::Debug );
    mesgsvc->setOutputStream( std::cerr, coral::Info );
    mesgsvc->setOutputStream( std::cerr, coral::Fatal );
    mesgsvc->setOutputStream( std::cerr, coral::Error );
    mesgsvc->setOutputStream( std::cerr, coral::Warning );
  } 
  */
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
    FCregister r;
    mycatalog->setAction(r);
    if( !mypfn.empty() ){
      mycatalog->connect();
      mycatalog->start();
      r.addReplicaPFN(mypfn,myrpf);
      mycatalog->commit();  
      mycatalog->disconnect();
    }else if( !myguid.empty() ){
      mycatalog->connect();
      mycatalog->start();
      FCLeaf* l=static_cast<FCLeaf*>(mycatalog->getWriteCatalog());
      try{
        l->getImpl()->addPFNtoGuid(myguid,myrpf,"");//dummy filetype!
      }catch (const pool::FCnonexistentFileException& /* e */){
        std::cerr<<"Error: cannot add replica to a non existent file "<<myguid<<std::endl;
        exit(1);
      }
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

