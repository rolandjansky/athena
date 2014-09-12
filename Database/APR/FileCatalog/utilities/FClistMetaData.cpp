/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//$Id: FClistMetaData.cpp 509054 2012-07-05 13:33:16Z mnowak $
/**FClistMetaData.cpp -- FileCatalog command line tool to list the selected MetaData entries
  @author: Zhen Xie
  @date: 02/03/2005 Z.X.
  set default logging to Warning if no POOL_OUTMSG_LEVEL is set; 
  separate logging stream to std::cerr, output stream to std::cout
  @date: 07/04/2005 Z.X.
  adopt to split catalog
*/
#include "FileCatalog/CommandLine.h"
#include "FileCatalog/IFileCatalog.h"
#include "FileCatalog/FCException.h"
#include "FileCatalog/IFCAction.h"
#include "FileCatalog/IFCContainer.h"
//#include "POOLCore/Exception.h"
#include "CoralBase/AttributeList.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeSpecification.h"
#include "CoralBase/MessageStream.h"
#include <memory>
#include <vector>
#include <string>
#include <cstdlib>
using namespace pool;

void printUsage(){
  std::cout<<"usage: FClistMetaData [-l lfname -p pfname -q query -u contactstring -m maxcache(default 1000) -h]" <<std::endl; 
}

static const char* opts[] = {"p","l","q","u","m","h",0};

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
  std::string myuri;
  std::string mylfn;
  std::string mypfn;
  unsigned int maxcache=1000;
  std::string query; 
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
    if( commands.Exists("p") ){
      mypfn=commands.GetByName("p");
    }
    if( commands.Exists("q") ){
      query=commands.GetByName("q");
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
  if(!query.empty()&&(!mypfn.empty()||!mylfn.empty())){
    std::cerr << "list metadata as query result"<<std::endl;
    mypfn=""; mylfn="";
  }
  if(!mypfn.empty()&&!mylfn.empty()){
    std::cerr << "WARNING: list metadata by PFN "<< mypfn<<std::endl;
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
    MetaDataContainer metas( mycatalog.get(), maxcache );
    if( !mypfn.empty() ){
      FileCatalog::FileID fid;
      std::string ftype;
      l.lookupFileByPFN(mypfn, fid, ftype);
      l.lookupMetaDataByQuery(std::string("guid='"+fid+"'"),metas);
    }else if( !mylfn.empty() ){
      FileCatalog::FileID fid;
      l.lookupFileByLFN(mylfn, fid);
      l.lookupMetaDataByQuery(std::string("guid='"+fid+"'"),metas);
    }else{
      l.lookupMetaDataByQuery(query, metas);
    }
    while(metas.hasNext()){
      MetaDataEntry attrs=metas.Next();
      coral::AttributeList spec=attrs.attrs();
      unsigned int counter=0;
      for(coral::AttributeList::const_iterator i=spec.begin(); i!=spec.end(); ++i){
        std::string colname=i->specification().name();
        std::string coltype=i->specification().typeName();
        if( colname != "guid" ){
	  std::string colvalue;
	  if(coltype=="string"){
	    colvalue=attrs.getAttributeValue<std::string>(colname);
	    //std::cout<<"###colvalue###"<<colvalue;
	  }else if(coltype=="short"){
	    short v=attrs.getAttributeValue<short>(colname);
	    colvalue=pool::fc::to_string<short>(v);
	  }else if(coltype=="unsigned short"){
	    unsigned short v=attrs.getAttributeValue<unsigned short>(colname);
	    colvalue=pool::fc::to_string<unsigned short>(v);
	  }else if(coltype=="int"){
	    int v=attrs.getAttributeValue<int>(colname);
	    colvalue=pool::fc::to_string<int>(v);
	  }else if(coltype=="unsigned int"){
	    unsigned int v=attrs.getAttributeValue<unsigned int>(colname);
	    colvalue=pool::fc::to_string<unsigned int>(v);
	  }else if(coltype=="long"){
	    long v=attrs.getAttributeValue<long>(colname);
	    colvalue=pool::fc::to_string<long>(v);
	  }else if(coltype=="unsigned long"){
	    unsigned long v=attrs.getAttributeValue<unsigned long>(colname);
	    colvalue=pool::fc::to_string<unsigned long>(v);
	  }else if(coltype=="long long"){
	    long long v=attrs.getAttributeValue<long long>(colname);
	    colvalue=pool::fc::to_string<long long>(v);
	  }else if(coltype=="unsigned long long"){
	    unsigned long long v=attrs.getAttributeValue<unsigned long long>(colname);
	    colvalue=pool::fc::to_string<unsigned long long>(v);
	  }else if(coltype=="float"){
	    float v=attrs.getAttributeValue<float>(colname);
	    colvalue=pool::fc::to_string<float>(v);
	  }else if(coltype=="double"){
	    double v=attrs.getAttributeValue<double>(colname);
	    colvalue=pool::fc::to_string<double>(v);
	  }else if(coltype=="long double"){
	    long double v=attrs.getAttributeValue<long double>(colname);
	    colvalue=pool::fc::to_string<long double>(v);
	  }else{
	    throw pool::FCbackendException("FClistMetaData ", std::string("unsuported type ")+coltype);
	  }
	  if( colvalue.empty()){
	    colvalue="NULL";
	  }
	  if ( counter!=0 ){
            std::cout<<",";
          }
	  std::cout<<"("<<colname<<","<<colvalue<<")";
	  ++counter;
        }
      }
      std::cout<<'\n';
    }
    std::cout<<std::endl;
    mycatalog->commit();  
    mycatalog->disconnect();
  }catch (const std::exception& er){
    std::cerr<<er.what()<<std::endl;
    exit(1);
  }
}


