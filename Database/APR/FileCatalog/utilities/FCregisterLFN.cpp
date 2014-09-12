/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//$Id: FCregisterLFN.cpp 509054 2012-07-05 13:33:16Z mnowak $
/**FCregisterLFN.cpp -- FileCatalog command line tool to register a LFN
  @author: Zhen Xie
  @author: Maria Girone
  @date: 02/03/2005 Z.X.
  set default logging to Warning if no POOL_OUTMSG_LEVEL is set; 
  separate logging stream to std::cerr, output stream to std::cout
*/
#include "FileCatalog/CommandLine.h"
#include "FileCatalog/IFileCatalog.h"
#include "FileCatalog/FCException.h"
#include "FileCatalog/IFCAction.h"
#include "POOLCore/Exception.h"
#include "CoralBase/MessageStream.h"
#include "CoralBase/MessageStream.h"
#include <memory>
using namespace pool;
void printUsage(){
  std::cout<<"usage: FCregisterLFN -l lfname -p pfname [-u contactstring -h]" <<std::endl;
}

static const char* opts[] = {"p","l","u","h",0};


int main(int argc, char** argv)
{
  std::string  myuri;
  std::string  mypfn;
  std::string  mylfn;
  try{
    CommandLine commands(argc,argv);
    commands.CheckOptions(opts);

    if( commands.Exists("u") ){
      myuri=commands.GetByName("u");
    }    
    if( commands.Exists("p") ){
      mypfn=commands.GetByName("p");
    }
    if( commands.Exists("l") ){
      mylfn=(commands.GetByName("l"));
    }
    if( commands.Exists("h") ){
      printUsage();
      exit(0);
    }
  }catch(std::string& strError){
    std::cerr << "Error: command parsing error "<<strError<<std::endl;
    exit(0);
  }
  
  if( mypfn.empty() || mylfn.empty() ){
    printUsage();
    std::cerr<<"Error: must specify pfname using -p, lfname using -l"<<std::endl;
    exit(0);
  }
  try{
    std::auto_ptr<IFileCatalog> mycatalog(new IFileCatalog);
    mycatalog->setWriteCatalog(myuri);
    FCregister r;
    mycatalog->setAction(r);
    mycatalog->connect();
    mycatalog->start();
    r.registerLFN(mypfn,mylfn);
    mycatalog->commit();  
    mycatalog->disconnect();
  }catch (const pool::Exception& er){
    //er.printOut(std::cerr);
    //std::cerr << std::endl;
    std::cerr<<er.what()<<std::endl;
    exit(1);
  }catch (const std::exception& er){
    std::cerr<<er.what()<<std::endl;
    exit(1);
  }
}


