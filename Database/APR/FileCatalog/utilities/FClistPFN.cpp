/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**FClistPFN.cpp -- FileCatalog command line tool to list the PFN entries from the catalog
   @author: Zhen Xie
   @author: Maria Girone
*/

#include "FileCatalog/CommandLine.h"
#include "FileCatalog/IFileCatalog.h"
#include "FileCatalog/URIParser.h"
#include "POOLCore/Exception.h"
#include "POOLCore/SystemTools.h"
#include <memory>
#include <vector>
#include <string>
#include <cstdlib>

using namespace pool;

void printUsage(){
  std::cout<<"usage: FClistPFN [-l lfname] [-u contactstring] [-t -h]" <<std::endl; 
}

static const char* opts[] = {"t","l","u","h",0};


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
  SystemTools::initGaudi();
  
  std::string  myuri;
  std::string  mylfn;
  std::string query(""); 
  bool printall=false;
  try{
    CommandLine commands(argc,argv);
    commands.CheckOptions(opts);

    if( commands.Exists("u") ){
      myuri=commands.GetByName("u");
    }else{
      myuri=SystemTools::GetEnvStr("POOL_CATALOG");
    }    
    if( commands.Exists("l") ){
      mylfn=commands.GetByName("l");
    }
    if( commands.Exists("t") ){
      printall=true;
    }
    if( commands.Exists("h") ){
      printUsage();
      exit(0);
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
    if(myuri.empty()) {
       // get default
       pool::URIParser p;
       p.parse();
       mycatalog->addReadCatalog(p.contactstring());
    }else{
      contactParser parser(myuri);
      std::vector<std::string> uris;
      parser.getContacts(uris);
      for(std::vector<std::string>::iterator i=uris.begin(); i!=uris.end(); ++i){
        mycatalog->addReadCatalog(*i);
      }
    }
    mycatalog->connect();
    mycatalog->start();
    pool::IFileCatalog::Strings fids;
    if( !mylfn.empty() ) {
       fids.push_back( mycatalog->lookupLFN( mylfn ) );
    }else{
       mycatalog->getFIDs( fids );
    }
    for( const auto& fid: fids ) {
       pool::IFileCatalog::Files files;
       mycatalog->getPFNs( fid, files );
       for( const auto& file: files ) {
          std::string pf = file.first;
          std::string filetype = (file.second.empty()? std::string("NULL") : file.second);
          if( printall ) {
             std::cout<<pf<<"    "<<filetype<<std::endl;
          }else{
             std::cout<<pf<<std::endl;
          }
       }
    }
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


