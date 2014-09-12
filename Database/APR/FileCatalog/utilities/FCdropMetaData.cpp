/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//$Id: FCdropMetaData.cpp 509054 2012-07-05 13:33:16Z mnowak $
/**FCdropMetaData.cpp -- FileCatalog command line tool to drop metadata definition and value
   @author: Zhen Xie
   @date: 02/03/2005 Z.X.
   set default logging to Warning if no POOL_OUTMSG_LEVEL is set; 
   separate logging stream to std::cerr, output stream to std::cout.
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
  std::cout<<"usage: FCdropMetadata [ -u contactstring -h]" <<std::endl;
  std::cout<<"drop all the metadata and its specification"<<std::endl;
}

static const char* opts[] = {"u","h",0};


int main(int argc, char** argv)
{
  std::string myuri;
  std::string metadef;
  try{
    CommandLine commands(argc,argv);
    commands.CheckOptions(opts);

    if( commands.Exists("u") ){
      myuri=commands.GetByName("u");
    } 
    if( commands.Exists("h") ){
      printUsage();
      exit(0);
    }
  }catch(std::string& strError){
    std::cerr << "Error:  command parsing error "<<strError<<std::endl;
    exit(-1);
  }
  try{
    std::auto_ptr<IFileCatalog> mycatalog(new IFileCatalog);
    mycatalog->setWriteCatalog(myuri);
    FCAdmin a;
    mycatalog->setAction(a);
    mycatalog->connect();
    mycatalog->start();
    a.dropMetaDataSpec();
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


