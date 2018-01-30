/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//$Id: FCrenamePFN.cpp 509054 2012-07-05 13:33:16Z mnowak $
/**FCrenamePFN.cpp -- FileCatalog command line tool to rename PFN. Used in the case the file has been moved.
  @author: Zhen Xie
  @date: 02/03/2005 Z.X.
  set default logging to Warning if no POOL_OUTMSG_LEVEL is set; 
  separate logging stream to std::cerr, output stream to std::cout
*/
#include "FileCatalog/CommandLine.h"
#include "FileCatalog/IFileCatalog.h"
#include "FileCatalog/URIParser.h"
#include "POOLCore/Exception.h"
#include <memory>

using namespace pool;

void printUsage(){
  std::cout<<"usage: FCrenamePFN -p pfname -n newpfname [-u contactstring -h]" <<std::endl;
}

static const char* opts[] = {"p","n","u","h",0};


int main(int argc, char** argv)
{
  std::string  myuri;
  std::string  mypfn;
  std::string  mynewpfn;
  try{
    CommandLine commands(argc,argv);
    commands.CheckOptions(opts);

    if( commands.Exists("u") ){
      myuri=commands.GetByName("u");
    }    
    if( commands.Exists("p") ){
      mypfn=commands.GetByName("p");
    }
    if( commands.Exists("n") ){
      mynewpfn=commands.GetByName("n");
    }
    if( commands.Exists("h") ){
      printUsage();
      exit(0);
    }
  }catch(std::string& strError){
    std::cerr << "error "<<strError<<std::endl;
    exit(1);
  }
  
  if( mypfn.empty() || mynewpfn.empty() ){
    printUsage();
    std::cerr<<"must specify pfname using -p, newpfname using -n "<<std::endl;
    exit(0);
  }
  try{
    std::auto_ptr<IFileCatalog> mycatalog(new IFileCatalog);
    pool::URIParser p( myuri );
    p.parse();
    mycatalog->setWriteCatalog( p.contactstring() );
    mycatalog->connect();
    mycatalog->start();

    //MN: There is no renamePFN method in Gaudi::IFileCatalog - implement if needed
    mycatalog->renamePFN(mypfn,mynewpfn);
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


