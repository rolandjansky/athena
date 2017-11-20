/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//$Id: XMLmetaTest_test.cxx 780038 2016-10-24 13:04:52Z mnowak $
/*
  CPPUnit test of metadata for XML Catalog.
  Author: Maria Girone from original test by Zhen Xie
*/
#include<cppunit/extensions/HelperMacros.h>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <iostream>
#include <cstdlib>
#ifndef POOL_XMLFILECATALOG_H
#include "XMLCatalog/XMLFileCatalog.h"
#endif
#ifndef POOL_FCSYSTEMTOOLS_H
#include "FileCatalog/FCSystemTools.h"
#endif
#include "FileCatalog/FCEntry.h"
#include "FileCatalog/IFCAction.h"
#include "FileCatalog/FCException.h"
#include "FileCatalog/IFCContainer.h"

class metadataTest: public CppUnit::TestFixture{
  CPPUNIT_TEST_SUITE( metadataTest );
  CPPUNIT_TEST( testCreateSchema );
  CPPUNIT_TEST( testUpdateSchema );
  CPPUNIT_TEST( testMetadata );
  //CPPUNIT_TEST( testDelete );
  CPPUNIT_TEST_SUITE_END();
public:
  pool::IFileCatalog* mycatalog;
  std::vector<std::string> names;
  pool::MetaDataEntry abx;
  pool::MetaDataEntry abc;
  void setUp(){
    mycatalog=new pool::IFileCatalog;
    mycatalog->setWriteCatalog("xmlcatalog_file:metadata.xml");
    for(size_t i=0; i<20; ++i){
      names.push_back("this-is-a-physical-file-name"+pool::FCSystemTools::itostr(i));
    }
    abx.addAttributeSpec("a","int");
    abx.addAttributeSpec("b","int");
    abx.addAttributeSpec("x","string");
    abc.addAttributeSpec("a","int");
    abc.addAttributeSpec("b","int");
    abc.addAttributeSpec("c", "string");
  }
  void tearDown() {
    if(mycatalog!=0) delete mycatalog;
    names.clear();
  }
  /*Test createMetaDataSpec, dropMetaDataSpec and 
    FCduplicatemetadataspecException.
    inner try block: create metadataspec
      -if it's nonexistant, create from scratch
      -if it exists, handle exception thrown from createMetaDataSpec:
          first drop the metadatspec then create a new one. 
  **/
  void testCreateSchema(){
    std::cout<<"[OVAL] TEST -->testCreateSchema"<<std::endl;
    try{
      pool::FCAdmin a;
      mycatalog->setAction(a);
      mycatalog->connect();
      mycatalog->start();
      try{
        a.createMetaDataSpec(abx);
      }
      catch(const pool::FCduplicatemetadataspecException& /* er */){
        std::cout<< "metadata schema already exists, drop it and create it again" <<std::endl;
        a.dropMetaDataSpec();
        a.createMetaDataSpec(abx);
      }
      mycatalog->commit();
      mycatalog->disconnect();
    }
    catch (const std::exception& er)
    {
      std::cerr << er.what() << std::endl;
      exit( 1 );
    }
  }
  /*Test updateMetaDataSpec
    case1: expand using AttributeList operation
         old spec a,b,x + a,b,c (AttributeList append_and_merge)
         new spec a,b,c,x
         result spec a,b,x,c
    case2: nonshrink(default metamode)
         old spec a,b,x,c
         new spec a,b,x 
         result spec a,b,x,c
    case3: shrink (metamode DELETE_REDUNDANT )
         old spec a,b,x,c
         new spec a,b,x
         result spec a,b,x
  **/
  void testUpdateSchema(){
    std::cout<<"[OVAL] TEST -->testUpdateSchema"<<std::endl; 
    try{
      pool::FCAdmin a;
      mycatalog->setAction(a);
      mycatalog->connect();
      pool::MetaDataEntry abcx,result1,result2,result3;
      abcx.attrs().merge(abc.attrs());
      mycatalog->start();
      a.updateMetaDataSpec(abcx);
      a.getMetaDataSpec(result1);
      std::cout <<"abcx "<<std::endl;
      std::cout <<abcx<<std::endl;
      std::cout <<"result1 "<<std::endl;
      std::cout <<result1<<std::endl;
      CPPUNIT_ASSERT_MESSAGE("case 1 failed",abcx.attrs()==result1.attrs());
      mycatalog->commit();
      
      mycatalog->start();
      a.updateMetaDataSpec(abx);
      a.getMetaDataSpec(result2);
      CPPUNIT_ASSERT_MESSAGE("case 2 failed",abcx.attrs()==result2.attrs());
      mycatalog->commit();
      
      mycatalog->start();
      a.updateMetaDataSpec(abx,pool::FileCatalog::DELETE_REDUNDANT);
      a.getMetaDataSpec(result3);
      CPPUNIT_ASSERT_MESSAGE("case 3 failed",abx.attrs()==result3.attrs());
      mycatalog->commit();

      mycatalog->disconnect();
    }
    catch (const std::exception& er)
    {
      std::cerr << er.what() << std::endl;
      exit( 2 );
    }
  }
  /*Test Metadata queries
  **/
  void testMetadata(){
    std::cout<<"[OVAL] TEST -->testMetadata"<<std::endl; 
    try{
      pool::FCAdmin a;
      pool::FCregister r;
      pool::FClookup l;
      mycatalog->setAction(a);
      mycatalog->setAction(r);
      mycatalog->connect();
      pool::MetaDataEntry mspec;
      mspec.addAttributeSpec("jobid", "int");
      mspec.addAttributeSpec("owner", "string");
      mycatalog->start();
      a.dropMetaDataSpec();
      a.createMetaDataSpec(mspec);
      std::vector<pool::FileCatalog::FileID> fids;
      for(size_t i=0; i<20; ++i){
        pool::FileCatalog::FileID fid;
        r.registerPFN(names[i],"metatest",fid);
        fids.push_back(fid);
      }
      mycatalog->commit();
      mycatalog->start();
      for( int n=12; n<20; ++n){
        pool::MetaDataEntry mentry(mspec);
        mentry.setAttributeValue<int>("jobid",n);
        mentry.setAttributeValue<std::string>("owner",std::string("cmsprod"));
        r.registerMetaData(fids[n], mentry);
      }
      mycatalog->commit();

      mycatalog->start();
      pool::MetaDataEntry myspec;
      a.getMetaDataSpec(myspec);
      pool::MetaDataContainer metas(mycatalog,10);
      l.lookupMetaDataByQuery("",metas);
      while( metas.hasNext() ){
        std::cout <<"hasNext"<<std::endl;
        pool::MetaDataEntry mm(metas.Next());
        std::cout <<mm.guid()<<std::endl;
        std::cout <<mm<<std::endl;
      }
      mycatalog->commit();
      mycatalog->start();
      pool::MetaDataEntry newspec;
      newspec.addAttributeSpec("jobid", "int");
      a.updateMetaDataSpec(newspec,pool::FileCatalog::DELETE_REDUNDANT);
      mycatalog->commit();
      
      mycatalog->disconnect();
    }
    catch (const std::exception& er)
    {
      std::cerr << er.what() << std::endl;
      exit( 3 );
    }
  }
  void testDelete(){
  }  
};

CPPUNIT_TEST_SUITE_REGISTRATION(metadataTest);

#include "CppUnit_testdriver.cxx"
