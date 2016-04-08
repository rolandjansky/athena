/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//$Id: FCpublish.cpp 676790 2015-06-19 17:55:38Z ssnyder $
/**FCpublish.cpp -- FileCatalog command line tool to extract a fragment from the source catalog and attach it to the destination catalog 
  @author: Zhen Xie
  @date: 02/03/2005 Z.X.
  set default logging to Warning if no POOL_OUTMSG_LEVEL is set; 
  separate logging stream to std::cerr, output stream to std::cout
*/
#include "FileCatalog/CommandLine.h"
#include "FileCatalog/IFileCatalog.h"
#include "FileCatalog/FCException.h"
#include "FileCatalog/IFCAction.h"
#include "FileCatalog/IFCContainer.h"
#include "FileCatalog/FCEntry.h"
#include "FileCatalog/FCSystemTools.h"
#include "POOLCore/Exception.h"
#include "CoralBase/MessageStream.h"
#include "CoralBase/MessageStream.h"
#include <memory>
#include <vector>
#include <string>
#include <cstdlib>

void printUsage(){
  std::cout<<"usage: FCpublish -d destinationcatalog [-q query -u sourcecatalog -s destmetadataspec -m buffersize(default 1000) -h]" <<std::endl;  
}

static const char* opts[] = {"d","s","q","u","m","h",0};

using namespace pool;
class contactParser{
public:
  contactParser(const std::string& contact):m_contact(contact){}
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

class MetadataSpecparser{
public:
  MetadataSpecparser(){}
  MetadataSpecparser(const std::string& spec):m_spec(spec){}
  std::unique_ptr<MetaDataEntry> getSpec() const{
    std::unique_ptr<MetaDataEntry> result(new MetaDataEntry);
    std::string delim("(,) ");
    std::string::size_type begIdx,endIdx;
    while( (begIdx=m_spec.find_first_not_of(delim))!= m_spec.npos ){
      endIdx=m_spec.find_first_of(')');
      std::string elem=m_spec.substr(begIdx,endIdx-begIdx);
      std::string::size_type colIdx=elem.find_last_of(',');
      if(colIdx != elem.npos){
        std::string colname=elem.substr(0,colIdx);
        std::string coltype=elem.substr(colIdx+1);
        result->addAttributeSpec(colname,coltype);
      }
      m_spec=m_spec.substr(endIdx+1);
    }
    return result;
  }
  ~MetadataSpecparser(){
  }
private:
  mutable std::string m_spec;
};

int main(int argc, char** argv)
{
  std::string  suri("file:PoolFileCatalog.xml");//default source uri
  if( const char* me=FCSystemTools::GetEnv("POOL_CATALOG") ){
    suri=me;
  }
  std::string  duri;
  std::string  querystr;
  std::string  destmetaspec;
  bool importSourceMetaData=true;
  bool existMetaData=true;
  unsigned int maxcache=1000; 
  try{
    CommandLine commands(argc,argv);
    commands.CheckOptions(opts);

    if( commands.Exists("u") ){
      suri=commands.GetByName("u");
    }  
    if( commands.Exists("d") ){
      duri=commands.GetByName("d");
    }
    if( commands.Exists("q") ){
      querystr=commands.GetByName("q");
    }
    if( commands.Exists("s") ){
      destmetaspec=commands.GetByName("s");
      if(destmetaspec.empty()){
        importSourceMetaData=false;
      }
    }
    if( commands.Exists("m") ){
      maxcache=atoi(commands.GetByName("m").c_str());
    }
    if( commands.Exists("h") ){
      printUsage();  
      exit(0);
    }
  }catch(std::string& strError){
    std::cerr<< "Error: command parsing error "<<strError<<std::endl;
    exit(0);
  }
  
  if( duri.empty() ){
    printUsage();
    std::cerr<<"Error: must specify destination catalog using -d"<<std::endl;
    exit(0);
  }
  try{
    std::unique_ptr<IFileCatalog> scatalog(new IFileCatalog);//can be n catalogs
    std::unique_ptr<IFileCatalog> dcatalog(new IFileCatalog);
    contactParser parser(suri);
    std::vector<std::string> uris;
    parser.getContacts(uris);
    for(std::vector<std::string>::iterator i=uris.begin(); i!=uris.end(); ++i){
      scatalog->addReadCatalog(*i);
    }
    dcatalog->setWriteCatalog(duri);
    FCAdmin as,ad;
    scatalog->setAction(as);
    dcatalog->setAction(ad);
    scatalog->connect();
    dcatalog->connect();
    dcatalog->start();
    scatalog->start();
    MetaDataEntry dspec, sspec;
    try{
      ad.getMetaDataSpec(dspec);
    }catch( const FCnonexistentMetaDataException& /* er */){
      existMetaData=false;
    }
    if(importSourceMetaData){
      try{
        as.getMetaDataSpec(sspec);
      }catch( const FCnonexistentMetaDataException& /* er */){
        existMetaData=false;
      }
      if( existMetaData&&sspec.size() >1 ){ //make sense only if source has metadata 
        if(dspec.size()<=1){
          ad.createMetaDataSpec(sspec);
        }else{
          ad.updateMetaDataSpec(sspec);
        }
      }
    }else{
      if( existMetaData ){
        MetadataSpecparser specparser(destmetaspec);
        std::unique_ptr<MetaDataEntry>newmeta=specparser.getSpec();
        if(newmeta->size()>1){
          ad.updateMetaDataSpec(dspec);
        }
      }
    }
    dcatalog->importCatalog(scatalog.get(),querystr,maxcache);
    scatalog->commit();  
    scatalog->disconnect();
    dcatalog->commit();
    dcatalog->disconnect();
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


