/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//$Id: FCcreateMetaDataSpec.cpp 676790 2015-06-19 17:55:38Z ssnyder $
/**FCcreateMetaDataSpec.cpp -- FileCatalog command line tool to create metadata definition
  @author: Zhen Xie
  @date: 02/03/2005 Z.X.
  set default logging to Warning if no POOL_OUTMSG_LEVEL is set; 
  separate logging stream to std::cerr, output stream to std::cout.
*/
#include <string>
#include "FileCatalog/CommandLine.h"
#include "FileCatalog/IFileCatalog.h"
#include "FileCatalog/FCException.h"
#include "FileCatalog/FCEntry.h"
#include "FileCatalog/IFCAction.h"
#include "POOLCore/Exception.h"
#include "CoralBase/MessageStream.h"
#include "CoralBase/MessageStream.h"
#include <memory>
using namespace pool;
void printUsage(){
  std::cout<<"usage: FCcreateMetadataSpec [-F -m metadatadef -u contactstring -h]" <<std::endl;
  std::cout<<"format of metadatadef: "<<std::endl;
  std::cout<<"(jobid,string),(owner,string),(jobtype,string)"<<std::endl;
}

static const char* opts[] = {"F","m","u","h",0};


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
        //std::cout<<colname<<std::endl;
        std::string coltype=elem.substr(colIdx+1);
        //std::cout<<coltype<<std::endl;
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
  std::string myuri;
  std::string metadef;
  bool dropandcreate=false;
  try{
    CommandLine commands(argc,argv);
    commands.CheckOptions(opts);
    
    if( commands.Exists("u") ){
      myuri=commands.GetByName("u");
    } 
    if( commands.Exists("m") ){
      metadef=commands.GetByName("m");
    }    
    if( commands.Exists("F") ){
      dropandcreate=true;
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
    std::unique_ptr<IFileCatalog> mycatalog(new IFileCatalog);
    mycatalog->setWriteCatalog(myuri);
    FCAdmin a;
    mycatalog->setAction(a);
    mycatalog->connect();
    mycatalog->start();
    MetadataSpecparser myspec(metadef);
    if(dropandcreate){
      a.dropMetaDataSpec();
      a.createMetaDataSpec(*(myspec.getSpec()));
    }else{
      a.createMetaDataSpec(*(myspec.getSpec()));
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


