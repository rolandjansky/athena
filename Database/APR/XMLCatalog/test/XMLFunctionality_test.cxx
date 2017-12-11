/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//$Id: XMLFunctionality_test.cxx 780038 2016-10-24 13:04:52Z mnowak $
/*
  CPPUnit test of XML Catalog.
  Author: Zhen Xie
*/
#include<cppunit/extensions/HelperMacros.h>
#include<cstdlib>

#include "XMLCatalog/XMLFileCatalog.h"
#include "FileCatalog/IFCAction.h"


class XMLtest: public CppUnit::TestFixture{
  CPPUNIT_TEST_SUITE( XMLtest );
  CPPUNIT_TEST( testRegister );
  CPPUNIT_TEST( testDelete );
  CPPUNIT_TEST_SUITE_END();
public:
  pool::IFileCatalog* mycatalog;
  std::vector<std::string> names;
  pool::FileCatalog::FileID fid;
  pool::IFCAction a;
  pool::FCregister r;
  pool::FClookup l;
  pool::FCAdmin d;
  std::string pfn,ftype;
  void setUp(){
    mycatalog=new pool::IFileCatalog;
    mycatalog->setWriteCatalog("xmlcatalog_file:func.xml");
    std::string bigname;
    std::string sname("abcdefghij");
    for (unsigned int i=0; i<25; ++i){
      bigname+=sname;
    }
    //bigname+="hello";
    names.push_back(bigname);
    names.push_back("this-is-a-pfn-test1");
    names.push_back("this-is-a-pfn-test2");
    names.push_back("this-is-a-pfn-test3");
  }

  void tearDown() {
    if(mycatalog!=0) delete mycatalog;
    names.clear();
  }

  void testRegister(){
    std::cout<<"[OVAL]TEST --> testIFCAction"<<std::endl;
    try{
      mycatalog->setAction(a);
      mycatalog->setAction(r);
      mycatalog->connect();
      mycatalog->start();
      std::cout<<"[OVAL] registerFile "<<std::endl;
      for(size_t i=0; i<names.size(); ++i){
	 fid = "";
        r.registerPFN(names[i],"root tree", fid);
        r.registerLFN(names[i], std::string("lfN")+fid); 
        r.registerLFN(names[i], std::string("lfn1")+fid);
        r.registerLFN(names[i], std::string("LFN1")+fid);
        std::cout<<"[OVAL] lookupBestPFN "<<i<<std::endl;
        r.lookupBestPFN(fid,pool::FileCatalog::READ,pool::FileCatalog::SEQUENTIAL,pfn,ftype);
        CPPUNIT_ASSERT_MESSAGE("wrong PFN",names[i]==pfn);  
        CPPUNIT_ASSERT_MESSAGE("wrong filetype","root tree"==ftype);
        std::cout<<"[OVAL] result pfn: "<<pfn<<" filetype: "<<ftype<<std::endl;
      }
      std::cout<<"[OVAL] lookupFileByPFN"<<std::endl;
      pool::FileCatalog::FileID myfid;
      a.lookupFileByPFN(pfn,myfid,ftype);
      CPPUNIT_ASSERT_MESSAGE("wrong fid",fid==myfid);
      CPPUNIT_ASSERT_MESSAGE("wrong filetype","root tree"==ftype);
      std::cout<<"pfn :"<<pfn<<" fid: "<<myfid<<" filetype "<<ftype<<std::endl;
      std::cout<<"[OVAL] lookupFileByLFN"<<std::endl;
      a.lookupFileByLFN(std::string("lfN")+fid,myfid);
      std::cout<<"[OVAL] addReplicaFilename"<<std::endl;
      r.addReplicaPFN(pfn,"replicapfn");
      std::cout<<"[OVAL] renamePFN"<<std::endl;
      r.renamePFN("replicapfn","movedreplicapfn");
      mycatalog->commit(); 
      mycatalog->disconnect();
    }
    catch( const std::exception& er)
    {
      std::cerr << er.what() << std::endl;
      exit( 1 );
    }
  }
  
  void testDelete(){
    std::cout<<"[OVAL] TEST --> testDelete"<<std::endl;
    try{

      mycatalog->connect();
      mycatalog->start();
      mycatalog->setAction(d);
      for(size_t i=0; i<names.size(); ++i){      
        d.deletePFN( names[i] );
      }
      d.deletePFN(std::string("movedreplicapfn"));
      mycatalog->commit(); 
      mycatalog->disconnect();
    }
    catch( const std::exception& er )
    {
      std::cerr << er.what() << std::endl;
      exit( 2 );
    }
  }
   
};
CPPUNIT_TEST_SUITE_REGISTRATION(XMLtest);

#include "CppUnit_testdriver.cxx"
