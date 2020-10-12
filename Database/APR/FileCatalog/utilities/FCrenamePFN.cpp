/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**FCrenamePFN.cpp -- FileCatalog command line tool to rename PFN. Used in the case the file has been moved.
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
  std::cout<<"usage: FCrenamePFN -p pfname -n newpfname [-u contactstring -h]" <<std::endl;
}


int main(int argc, char** argv)
{
  SystemTools::initGaudi();
  
  std::string  myuri;
  std::string  mypfn;
  std::string  mynewpfn;
  try{
    CommandLine commands(argc,argv);
    const char* opts[] = {"p","n","u","h",0};
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
      return 0;
    }
  }catch(std::string& strError){
    std::cerr << "error "<<strError<<std::endl;
    return 1;
  }
  
  if( mypfn.empty() || mynewpfn.empty() ){
    printUsage();
    std::cerr<<"must specify pfname using -p, newpfname using -n "<<std::endl;
    return 0;
  }
  try{
    std::unique_ptr<IFileCatalog> mycatalog(new IFileCatalog);
    pool::URIParser p( myuri );
    p.parse();
    mycatalog->setWriteCatalog( p.contactstring() );
    mycatalog->connect();
    mycatalog->start();
    mycatalog->renamePFN(mypfn, mynewpfn);
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


