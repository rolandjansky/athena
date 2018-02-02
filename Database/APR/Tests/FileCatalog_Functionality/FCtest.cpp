/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//$Id: FCtest.cpp 740758 2016-04-16 07:11:28Z krasznaa $
/*
  @file a test driver for FileCatalog.
  @author: Zhen Xie
*/
#include<cppunit/extensions/HelperMacros.h>

#include <string>
#include <iostream>
#include <cstdlib>
#include "FileCatalog/IFileCatalog.h"
#include "FileCatalog/FCSystemTools.h"
//#ifndef POOL_URIPARSER_H
//#include "FileCatalog/URIParser.h"
//#endif
#ifndef POOL_FCEXCEPTION_H
#include "FileCatalog/FCException.h"
#endif
#ifndef POOL_IFCCONTAINER_H
#include "FileCatalog/IFCContainer.h"
#endif
#include "FileCatalog/IFCAction.h"


using namespace pool;

class FCtest: public CppUnit::TestFixture{
  CPPUNIT_TEST_SUITE( FCtest );
  CPPUNIT_TEST( testregisterFile );
  CPPUNIT_TEST( testimport );
  CPPUNIT_TEST_SUITE_END();
public:
  //URIParser p;
  IFileCatalog* mycatalog, *source, *dest;
  FileCatalog::FileID fid;
  std::string pfn;  std::string filetype;
  std::string sourcecatalogtype;
  std::string sourcecatalogurl;
  std::string destcatalogtype;
  std::string destcatalogurl;
  std::string mycatalogtype;
  std::string mycatalogurl;
  
  std::vector<std::string> names;
  enum importtype{
    xml2xml=0,
    xml2mysql,
    mysql2xml,
    mysql2mysql
  };
  enum singletype{
    xml=0,
    mysql
  };
  void setUp(){
    mycatalog=new IFileCatalog;
    source=new IFileCatalog;
    dest=new IFileCatalog;
    names.push_back("test1");
    names.push_back("test2");
    names.push_back("test3");

  }
  void singlesetUp(singletype t){
    if(t==xml){
      mycatalogurl="xmlcatalog_file:mycatalog.xml";
    }else if(t==mysql){
      mycatalogurl="mysqlcatalog_mysql://xiezhen@localhost/zhendb";
    }
  }
  void importsetUp(importtype t) {
    if(t==xml2xml){
      sourcecatalogurl="xmlcatalog_file:source.xml";
      destcatalogurl="xmlcatalog_file:dest.xml";
    }else if(t==xml2mysql){
      sourcecatalogurl="xmlcatalog_file:source.xml";
      destcatalogurl="mysqlcatalog_mysql://xiezhen@localhost/zhendb";
    }else if(t==mysql2mysql){
      sourcecatalogurl="mysqlcatalog_mysql://xiezhen@localhost/FCtest";
      destcatalogurl="mysqlcatalog_mysql://xiezhen@localhost/zhendb";
    }else if(t==mysql2xml){
      destcatalogurl="xmlcatalog_file:dest.xml";
      sourcecatalogurl="mysqlcatalog_mysql://xiezhen@localhost/zhendb";
    }
  }
  
  void tearDown() {
    if(mycatalog!=0) delete mycatalog;
    if(source!=0) delete source;
    if(dest!=0) delete dest;
    names.clear();
  }
  
  void testregisterFile() {
    try{	   
      singlesetUp(xml);
      std::cout<<"TEST --> testregisterFile"<<std::endl;
      mycatalog->setWriteCatalog(mycatalogurl);
      mycatalog->connect();
      mycatalog->start();
      FCregister r;
      mycatalog->setAction(r);
      FClookup l;
      mycatalog->setAction(l);
      PFNContainer mypfns(mycatalog);
      for(size_t i=0; i<names.size(); i++){
        r.registerPFN(std::string("pfn:")+names[i],"root/tree",fid);
        std::cout<<"fid"<<fid<<std::endl;
        r.lookupBestPFN(fid,FileCatalog::READ,FileCatalog::SEQUENTIAL,pfn,filetype);
        CPPUNIT_ASSERT_MESSAGE("wrong pfn",(std::string("pfn:")+names[i])==pfn);  
        CPPUNIT_ASSERT_MESSAGE("wrong filetype","root/tree"==filetype);      
        //register LFN 
        r.registerLFN(pfn,std::string("lfn:")+names[i]);
        //test addReplicaFilename
        r.addReplicaPFN(pfn,std::string("replicapfn:")+names[i]);
        //test iterator
        mypfns.reset();
        l.lookupPFN(fid,mypfns);
        unsigned int j=0;
        while(mypfns.hasNext()){
          PFNEntry pentry=mypfns.Next();
          //numberguid[i]=pentry.guid;
          std::cout<<pentry<<std::endl;
          CPPUNIT_ASSERT_MESSAGE("Error in the filename",(pentry.pfname()==std::string("pfn:")+names[i])||(pentry.pfname()==std::string("replicapfn:")+names[i]));
          j++;
        }
      }
      //CPPUNIT_ASSERT_MESSAGE("The guid is not the same",numberguid[0]==numberguid[1]);
      //commmit changes
      mycatalog->commit();
      std::cout<<"committed"<<std::endl;
      mycatalog->disconnect();
      std::cout<<"disconnect"<<std::endl;
    }catch(const pool::Exception& er){
      std::cerr << er.what() << std::endl;
      exit(1);
    }
  }
  
  void testimport() {
    
    try{
      importsetUp(xml2xml);
      std::cout<<"TEST --> testimport"<<std::endl;
      dest->setWriteCatalog(destcatalogurl);
      dest->connect();
      source->setWriteCatalog(sourcecatalogurl);
      source->connect();

      source->start();
      MetaDataEntry meta;
      meta.addAttributeSpec("jobid", "int");
      meta.addAttributeSpec("owner", "string");
      FCAdmin a;
      source->setAction(a);
      try{
        a.createMetaDataSpec(meta);
      }catch(const pool::FCduplicatemetadataspecException& /* er */){
        std::cout<<"metadata schema already exists, drop it and create it again"<<std::endl;
        a.dropMetaDataSpec();
        a.createMetaDataSpec(meta);
      }
      source->commit();
      
      dest->start();
      FCAdmin ad;
      dest->setAction(ad);
      try{
        ad.createMetaDataSpec(meta);
      }catch(const pool::FCduplicatemetadataspecException& /* er */ ){
        std::cout<<"metadata schema already exists, drop it and create it again"<<std::endl;
        ad.dropMetaDataSpec();
        ad.createMetaDataSpec(meta);
      }
      dest->commit();
      
      source->start();
      meta.setAttributeValue("jobid", 10);
      meta.setAttributeValue("owner",std::string("cmsprod"));
      FCregister r;
      source->setAction(r);
      for(size_t i=0; i<names.size(); i++){
        pfn=std::string("source::pfn:")+names[i];
        r.registerPFN(pfn,"root/tree",fid);
        r.registerLFN(pfn,std::string("source::lfn:")+names[i]);
        r.addReplicaPFN(pfn,std::string("rep::pfn:")+names[i] );
        r.registerMetaData(fid,meta);
      }
      source->commit(FileCatalog::ONHOLD);

      source->start();
      dest->start();
      dest->importCatalog(source,"",1000);
      dest->commit();
      source->commit();

      source->disconnect();
      dest->disconnect();
    }catch(const pool::Exception& er){
      std::cerr << er.what() << std::endl;
      exit(1);
    }
  }

};

CPPUNIT_TEST_SUITE_REGISTRATION(FCtest);
//#include<CppUnit_testdriver.cpp>
#include<TestPolicy/CppUnit_testdriver.cxx>

