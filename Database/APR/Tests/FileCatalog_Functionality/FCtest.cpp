/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  @file a test driver for FileCatalog.
  @author: Zhen Xie
*/
#include<cppunit/extensions/HelperMacros.h>

#include <string>
#include <iostream>
#include <cstdlib>
#include "POOLCore/Exception.h"
#include "POOLCore/SystemTools.h"
#include "FileCatalog/IFileCatalog.h"

using namespace pool;

class FCtest: public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE( FCtest );
  CPPUNIT_TEST( testregisterFile );
  CPPUNIT_TEST( testimport );
  CPPUNIT_TEST_SUITE_END();

public:
  IFileCatalog* mycatalog, *source, *dest;
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
    SystemTools::initGaudi();
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
      std::set<std::string>     registered_pfns;
      const std::string         new_filetype = "root/tree";
      for(size_t i=0; i<names.size(); i++){
        //register PFN
         std::string new_pfn = std::string("pfn:") + names[i];
         std::string fid;
         mycatalog->registerPFN( new_pfn, new_filetype, fid );
         std::cout << "Registering PFN: " << new_pfn << " of type: " << new_filetype
                   << " FID assigned by the FC: " << fid <<std::endl;
         // read back PFN
         std::string pfn, filetype;
         mycatalog->getFirstPFN(fid, pfn, filetype);
         CPPUNIT_ASSERT_MESSAGE("wrong pfn", new_pfn==pfn );  
         CPPUNIT_ASSERT_MESSAGE("wrong filetype", new_filetype==filetype );
         registered_pfns.insert( new_pfn );
         //register LFN 
         // mycatalog->registerLFN(pfn,std::string("lfn:")+names[i]);
         mycatalog->registerLFN(fid, std::string("lfn:")+names[i]);

         //test addReplicaFilename
         // addReplicaPFN(pfn,std::string("replicapfn:")+names[i]);
      }
      //test reading back
      /*
      IFileCatalog::Files     mypfns;
      mycatalog->getPFN(fid, mypfns);
      for( const auto& attr_pair: mypfns ) {
         std::string pfn = attr_pair.first, ftype = attr_pair.second;
         std::cout  << "PFN: " << pfn <<  "  type: " << ftype << std::endl;
         CPPUNIT_ASSERT_MESSAGE("Error in the filename", registered_pfns.count(pfn) );
      }
      */
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
/* MN: not supporting FC import with Gaudi FC     
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
      }catch(const pool::FCduplicatemetadataspecException& ){
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
      }catch(const pool::FCduplicatemetadataspecException& ){
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
*/
  }

};

CPPUNIT_TEST_SUITE_REGISTRATION(FCtest);
#include<TestTools/CppUnit_testdriver.cxx>

