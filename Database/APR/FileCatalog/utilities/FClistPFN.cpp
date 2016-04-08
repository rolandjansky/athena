/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//$Id: FClistPFN.cpp 509054 2012-07-05 13:33:16Z mnowak $
/**FClistPFN.cpp -- FileCatalog command line tool to list the PFN entries from the catalog
   @author: Zhen Xie
   @author: Maria Girone
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
  std::cout<<"usage: FClistPFN [-l lfname -q query -u contactstring -m maxcache(default 1000) -t -h]" <<std::endl; 
}

static const char* opts[] = {"t","l","q","u","m","h",0};


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
  std::string  mylfn;
  unsigned int maxcache=1000;
  std::string query(""); 
  bool printall=false;
  try{
    CommandLine commands(argc,argv);
    commands.CheckOptions(opts);

    if( commands.Exists("u") ){
      myuri=commands.GetByName("u");
    }else{
      myuri=std::string(::getenv("POOL_CATALOG"));
    }    
    if( commands.Exists("l") ){
      mylfn=commands.GetByName("l");
    }
    if( commands.Exists("q") ){
      query=commands.GetByName("q");
    }
    if( commands.Exists("t") ){
      printall=true;
    }
    if( commands.Exists("h") ){
      printUsage();
      exit(0);
    }
    if( commands.Exists("m") ){
      maxcache=atoi(commands.GetByName("m").c_str());
   }
  }catch(std::string& strError){
    std::cerr << "Error: command parsing error "<<strError<<std::endl;
    exit(0);
  }

  if(!query.empty()&&!mylfn.empty()){
    std::cerr<< "Warning: list PFN by LFN..." <<std::endl;
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
    PFNContainer mypfns( mycatalog.get(), maxcache );
    if( !mylfn.empty() ){
      l.lookupPFNByLFN(mylfn,mypfns);
    }else{
      l.lookupPFNByQuery(query,mypfns);
    }
    while(mypfns.hasNext()){
      PFNEntry pentry;
      pentry=mypfns.Next();
      std::string pf=pentry.pfname();
      std::string filetype=pentry.filetype();
      if(printall){
        if(filetype.empty()){
          std::cout<<pf<<"    NULL"<<std::endl;
        }else{
          std::cout<<pf<<"    "<<filetype<<std::endl;
        }
      }else{
        std::cout<<pf<<std::endl;
      }
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


