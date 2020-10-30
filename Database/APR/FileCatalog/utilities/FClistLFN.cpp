/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**FClistLFN.cpp -- FileCatalog command line tool to list all the LFN entries from the catalog.
  @author Zhen Xie
  @author Maria Girone
*/

#include "FileCatalog/CommandLine.h"
#include "FileCatalog/IFileCatalog.h"
#include "FileCatalog/URIParser.h"
#include "POOLCore/Exception.h"
#include "POOLCore/SystemTools.h"
#include <memory>
#include <vector>

using namespace pool;

void printUsage(){
  std::cout<<"usage: FClistLFN [-p pfname] [-u contactstring] [-h]" <<std::endl; 
}


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
  std::string  mypfn;
  try{
    CommandLine commands(argc,argv);
    const char* opts[] = {"p","u","h",0};
    commands.CheckOptions(opts);
    
    if( commands.Exists("u") ){
      myuri=commands.GetByName("u");
    }else{
      myuri=SystemTools::GetEnvStr("POOL_CATALOG");
    }       
    if( commands.Exists("p") ){
      mypfn=commands.GetByName("p");
    }
    if( commands.Exists("h") ){
      printUsage();
      return 0;
    }
  }catch(std::string& strError){
    std::cerr<< "Error: command parsing error "<<strError<<std::endl;
    return 0;
  }

  if( !mypfn.empty() ) {
    std::cerr<< "Warning: list LFN by PFN..." <<std::endl;
  }
  try{
    std::unique_ptr<IFileCatalog> mycatalog(new IFileCatalog);
    if(myuri.empty()){
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
    if( !mypfn.empty() ) {
       fids.push_back( mycatalog->lookupPFN( mypfn ) );
    }else{
       mycatalog->getFIDs( fids );
    }
    for( const auto& fid: fids ) {
       pool::IFileCatalog::Files files;
       mycatalog->getLFNs( fid, files );
       for( const auto& file: files ) {
          std::cout << file.first << " ,   " << file.second << std::endl;
       }
    }

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


