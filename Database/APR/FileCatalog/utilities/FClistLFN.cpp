/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//$Id: FClistLFN.cpp 509054 2012-07-05 13:33:16Z mnowak $
/**FClistLFN.cpp -- FileCatalog command line tool to list all the LFN entries from the catalog.
  @author Zhen Xie
  @author Maria Girone
  @date: 02/03/2005 Z.X.
   set default logging to Warning if no POOL_OUTMSG_LEVEL is set; 
   separate logging stream to std::cerr, output stream to std::cout.
*/

#include "FileCatalog/CommandLine.h"
#include "FileCatalog/IFileCatalog.h"
#include "FileCatalog/FCException.h"
#include "FileCatalog/IFCAction.h"
#include "FileCatalog/IFCContainer.h"
#include "POOLCore/Exception.h"
#include "CoralBase/MessageStream.h"
#include "CoralBase/MessageStream.h"
#include <memory>
#include <vector>
#include <string>
#include <cstdlib>
using namespace pool;

void printUsage(){
  std::cout<<"usage: FClistLFN [-p pfname -q query -u contactstring -m maxcache(default 1000) -h]" <<std::endl; 
}

static const char* opts[] = {"p","q","u","m","h",0};


class contactParser{
public:
  explicit contactParser(const std::string& contact):m_contact(contact){}
  void getContacts(std::vector<std::string>& uris){
    std::string delim(" ");
    std::string::size_type begIdx,endIdx;
    std::string elem;
    while( (begIdx=m_contact.find_first_not_of(delim))!= m_contact.npos ){
      endIdx=m_contact.find_first_of(delim);
      if(endIdx != m_contact.npos ){
        elem=m_contact.substr(begIdx,endIdx-begIdx);
        m_contact=m_contact.substr(endIdx+1);
        uris.push_back(elem);
      }else{
        elem=m_contact;
        uris.push_back(elem);
        break;
      }
    }
  }
  ~contactParser(){}
private:
  std::string m_contact;
};


int main(int argc, char** argv)
{
  std::string  myuri;
  std::string  mypfn;
  std::string  query;
  unsigned int maxcache=1000; 
  try{
    CommandLine commands(argc,argv);
    commands.CheckOptions(opts);
    
    if( commands.Exists("u") ){
      myuri=commands.GetByName("u");
    }else{
      myuri=std::string(::getenv("POOL_CATALOG"));
    }       
    if( commands.Exists("p") ){
      mypfn=commands.GetByName("p");
    }
    if( commands.Exists("m") ){
      maxcache=atoi(commands.GetByName("m").c_str());
    }
    if( commands.Exists("q") ){
      query=commands.GetByName("q");
    }
    if( commands.Exists("h") ){
      printUsage();
      exit(0);
    }
  }catch(std::string& strError){
    std::cerr<< "Error: command parsing error "<<strError<<std::endl;
    exit(0);
  }

  if(!query.empty()&&!mypfn.empty()){
    std::cerr<< "Warning: list LFN by PFN..." <<std::endl;
  }
  try{
    std::auto_ptr<IFileCatalog> mycatalog(new IFileCatalog);
    if(myuri.empty()){
      mycatalog->addReadCatalog(myuri);
    }else{
      contactParser parser(myuri);
      std::vector<std::string> uris;
      parser.getContacts(uris);
      for(std::vector<std::string>::iterator i=uris.begin(); i!=uris.end(); ++i){
      mycatalog->addReadCatalog(*i);
      }
    }
    FClookup l;
    mycatalog->setAction(l);
    mycatalog->connect();
    mycatalog->start();
    LFNContainer mylfns( mycatalog.get(), maxcache );
    if( !mypfn.empty() ){
      l.lookupLFNByPFN(mypfn,mylfns);
    }else{
      l.lookupLFNByQuery(query,mylfns);
    }
    while(mylfns.hasNext()){
      LFNEntry lentry;
      lentry=mylfns.Next();
      std::cout<<lentry.lfname()<<std::endl;
    }
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


