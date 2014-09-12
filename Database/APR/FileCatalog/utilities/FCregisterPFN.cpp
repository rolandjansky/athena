/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//$Id: FCregisterPFN.cpp 509054 2012-07-05 13:33:16Z mnowak $
/**FCregisterPFN.cpp -- FileCatalog command line tool registerPFN
   Register a PFN in the File Catalog: a unique FileID is generated and inserted in the catalog, 
   @author: Zhen Xie
   @author: Maria Girone
   @date: 02/03/2005 Z.X.
   set default logging to Warning if no POOL_OUTMSG_LEVEL is set; 
   separate logging stream to std::cerr, output stream to std::cout.
   @date: 07/04/2005 Z.X.
   adopt to split interface
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
  std::cout<< "usage: registerPFN -p pfname [ -u contactstring -t filetype -g guid -h ]" <<std::endl;
}
static const char* opts[] = {"p","t","g","u","h",0};


int main(int argc, char** argv)
{
  std::string myuri;
  std::string mypfn;
  std::string myfiletype;
  std::string myguid;
  try{
    CommandLine commands(argc,argv);
    commands.CheckOptions(opts);

    if( commands.Exists("u") ){
      myuri=commands.GetByName("u");
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
      exit(0);
    }
  }catch(std::string& strError){
    std::cerr<<"Error: command parsing error "<<strError<<std::endl;
    exit(0);
  }
  
  if( mypfn.empty() ){
    printUsage();
    std::cerr<<"Error: must specify pfname using -p"<<std::endl;
    exit(0);
  }
  try{  
    std::auto_ptr<IFileCatalog> mycatalog(new IFileCatalog);
    mycatalog->setWriteCatalog(myuri);
    IFCAction a;
    mycatalog->setAction(a);
    mycatalog->connect();
    mycatalog->start();
    FileCatalog::FileID fid = myguid;
    a.registerPFN(mypfn,myfiletype,fid);
    mycatalog->commit();  
    mycatalog->disconnect();
    std::cout<<fid<<std::endl;
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





