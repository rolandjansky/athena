/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//$Id: FCdescribeMetaData.cpp 509054 2012-07-05 13:33:16Z mnowak $
/**FCdescribeMetaData.cpp -- FileCatalog command line tool to describe metadata definition of the catalog
   @author: Zhen Xie
   @date: 02/03/2005 Z.X.
   set default logging to Warning if no POOL_OUTMSG_LEVEL is set; 
   separate logging stream to std::cerr, output stream to std::cout.
*/
#include "FileCatalog/CommandLine.h"
#include "FileCatalog/IFileCatalog.h"
#include "FileCatalog/FCException.h"
#include "FileCatalog/IFCAction.h"
#include "FileCatalog/FCEntry.h"
#include "CoralBase/AttributeListSpecification.h"
#include "CoralBase/AttributeList.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/MessageStream.h"
#include <memory>
using namespace pool;

void printUsage(){
  std::cout<<"usage: FCdescribeMetadata [-u contactstring -h]" <<std::endl;
  std::cout<<"format of metadata description: "<<std::endl;
  std::cout<<"(jobid,string),(owner,string),(jobtype,string)"<<std::endl;
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
    std::cerr << "Error: command parsing error "<<strError<<std::endl;
    exit(-1);
  }
  try{
    std::auto_ptr<IFileCatalog> mycatalog(new IFileCatalog);
    mycatalog->addReadCatalog(myuri);
    FClookup l;
    mycatalog->setAction(l);
    mycatalog->connect();
    mycatalog->start();
    MetaDataEntry spec;
    l.getMetaDataSpec(spec);
    
    coral::AttributeList myspec=spec.attrs();
    unsigned int counter=0;
    for(coral::AttributeList::iterator ispec=myspec.begin();ispec!=myspec.end(); ++ispec){
      if( ispec->specification().name()!="guid" ){
	if( counter!=0 ){
	  std::cout<<",";
	}
	std::cout<<"("<<ispec->specification().name()<<","<<ispec->specification().typeName()<<")"; 
	++counter;
      }   
    }
    std::cout<<std::endl; 
    mycatalog->commit();  
    mycatalog->disconnect();
  }catch (const std::exception& er){
    std::cerr<<er.what()<<std::endl;
    exit(1);
  }
}


