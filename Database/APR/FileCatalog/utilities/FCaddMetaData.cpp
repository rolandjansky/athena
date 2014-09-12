/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//$Id: FCaddMetaData.cpp 509054 2012-07-05 13:33:16Z mnowak $
/**FCaddMetaData.cpp -- FileCatalog command line tool to add metadata
  @author Zhen Xie
  @date: 02/03/2005 
  set default logging to Warning if no POOL_OUTMSG_LEVEL is set; 
  separate logging stream to std::cerr, output stream to std::cout.
  @date: 07/04/2005
  adopt to split catalog interface
*/
#include <cstdlib>
#include "FileCatalog/CommandLine.h"
#include "FileCatalog/IFileCatalog.h"
#include "FileCatalog/FCException.h"
#include "FileCatalog/IFCAction.h"
#include "FileCatalog/FCEntry.h"
#include "POOLCore/Exception.h"
#include "CoralBase/MessageStream.h"
#include "CoralBase/MessageStream.h"
#include <memory>
using namespace pool;
void printUsage(){
  std::cout<<"usage: FCaddMetadata [-m metadata -p pfname -l lfname -u contactstring -h]" <<std::endl;
  std::cout<<"format of metadata: "<<std::endl;
  std::cout<<"(jobid,value),(owner,value),(jobtype,value)"<<std::endl;
}

static const char* opts[] = {"m","p","l","u","h",0};

class Metadataparser{
public:
  Metadataparser(){}
  Metadataparser(const std::string& attr):m_attr(attr){}
  void getAttr(MetaDataEntry& meta) const{
    std::string delim("(,) ");
    std::string::size_type begIdx,endIdx;
    while( (begIdx=m_attr.find_first_not_of(delim))!= m_attr.npos ){
      endIdx=m_attr.find_first_of(')');
      std::string elem=m_attr.substr(begIdx,endIdx-begIdx);
      std::string::size_type colIdx=elem.find_last_of(',');
      if(colIdx != elem.npos){
        std::string colname=elem.substr(0,colIdx);
        //std::cout<<colname<<std::endl;
        std::string colval=elem.substr(colIdx+1);
        //std::cout<<colval<<std::endl;
        std::string coltype=meta.attrs()[colname].specification().typeName();
        if( coltype=="string" ){          
          meta.setAttributeValue(colname, colval);
        }else if( coltype=="int" ){
	  int v;
	  pool::fc::from_string<int>(v,colval,std::dec);
          meta.setAttributeValue<int>(colname,v);
        }else if( coltype=="unsigned int" ){
	  unsigned int v;
	  pool::fc::from_string<unsigned int>(v,colname,std::dec);
          meta.setAttributeValue<unsigned int>(colname,v);
        }else if( coltype=="short" ){
	  short v;
	  pool::fc::from_string<short>(v,colname,std::dec);
          meta.setAttributeValue<short>(colname,v);
        }else if( coltype=="unsigned short" ){
	  unsigned short v;
	  pool::fc::from_string<unsigned short>(v,colname,std::dec);
          meta.setAttributeValue<unsigned short>(colname,v);
        }else if( coltype=="long" ){
	  long v;
	  pool::fc::from_string<long>(v,colname,std::dec);
          meta.setAttributeValue<long>(colname,v);
        }else if( coltype=="unsigned long" ){
	  unsigned long v;
	  pool::fc::from_string<unsigned long>(v,colname,std::dec);
          meta.setAttributeValue<unsigned long>(colname,v);
        }else if( coltype=="long long" ){
	  long long v;
	  pool::fc::from_string<long long>(v,colname,std::dec);
          meta.setAttributeValue<long long>(colname,v);
        }else if( coltype=="unsigned long long" ){
	  unsigned long long v;
	  pool::fc::from_string<unsigned long long>(v,colname,std::dec);
          meta.setAttributeValue<unsigned long long>(colname,v);
        }else if( coltype=="float" ){
	  float v;
	  pool::fc::from_string<float>(v,colname,std::dec);
          meta.setAttributeValue<float>(colname,v);
        }else if( coltype=="double" ){
	  double v;
	  pool::fc::from_string<double>(v,colname,std::dec);
          meta.setAttributeValue<double>(colname,v);
        }else if( coltype=="long double" ){
	  long double v;
	  pool::fc::from_string<long double>(v,colname,std::dec);
          meta.setAttributeValue<long double>(colname,v);
        }else{
	  throw FCbackendException("FCaddMetaData",
				   std::string("unsupported type ")+coltype);
	}
      }
      m_attr=m_attr.substr(endIdx+1);
    }
  }
  
  ~Metadataparser(){
  }
private:
    mutable std::string m_attr;
};

int main(int argc, char** argv)
{
  std::string myuri;
  std::string mypfn;
  std::string mylfn;
  std::string metadata;
  try{
    CommandLine commands(argc,argv);
    commands.CheckOptions(opts);
    
    if( commands.Exists("u") ){
      myuri=commands.GetByName("u");
    } 
    if( commands.Exists("m") ){
      metadata=commands.GetByName("m");
    }     
    if( commands.Exists("p") ){
      mypfn=commands.GetByName("p");
    }
    if( commands.Exists("l") ){
      mylfn=commands.GetByName("l");
    }
    if( commands.Exists("h") ){
      printUsage();
      exit(0);
    }
  }catch(std::string& strError){
    std::cerr << "Error: command parsing error "<<strError<<std::endl;
    exit(0);
  }
  if( mypfn.empty()&&mylfn.empty() ){
    printUsage();
    std::cerr << "Error: must specify pfname using -p or lfname using -l" << std::endl;
    exit(0);
  }
  try{
    std::auto_ptr<IFileCatalog> mycatalog(new IFileCatalog);
    mycatalog->setWriteCatalog(myuri);
    FCregister r;
    mycatalog->setAction(r);
    mycatalog->connect();
    mycatalog->start();
    MetaDataEntry mymeta; 
    r.getMetaDataSpec(mymeta);
    Metadataparser metaparser(metadata);
    metaparser.getAttr(mymeta);
    FileCatalog::FileID fid;
    if( !mylfn.empty() ){
      r.lookupFileByLFN(mylfn,fid);
    }else if( !mypfn.empty() ){
      std::string ftype;
      r.lookupFileByPFN(mypfn,fid,ftype);
    }
    r.registerMetaData(fid,mymeta);
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

  
