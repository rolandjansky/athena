/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file StoreGate/test/VarHandleKey_parseKey_test.cxx
 * @author Charles Leggett
 * @date Jun, 2017
 * @brief Tests for VarHandleKey::parseKey.
 */


#undef NDEBUG
#include "StoreGate/VarHandleKey.h"
#include "StoreGate/exceptions.h"
#include "AthenaKernel/errorcheck.h"
#include "TestTools/initGaudi.h"
#include "TestTools/expect_exception.h"
#include <cassert>
#include <iostream>


void test(const std::string& key, const std::string& store, bool b,
          const std::string& fkey) {
  try {
    SG::VarHandleKey h(1234,key,Gaudi::DataHandle::Reader,store);
    std::cout << "   key: \"" << key << "\" store: \"" << store 
              << "\"   DataObjID: \"" << h.objKey() << "\"  sg: \"" << h.key() 
              << "\"" << std::endl;

    if (fkey != h.objKey()) {
      std::cout << "  Key mismatch. expected \"" << fkey << "\" got \""
                << h.objKey() << "\"\n";
    }
    assert( fkey == h.objKey() );

  } catch (std::exception &e) {
      
    std::cout << "   key: \"" << key << "\" store: \"" << store << "\"";
    if ( b ) {
      std::cout << "  -> UNEXPECTED: " << e.what() << std::endl;
    } else {
      std::cout << "  -> expected failure: " << e.what() << std::endl;
    }

    assert( !b );

  }

}

void test(const std::string& key, bool b, const std::string& fkey) {
  try {
    SG::VarHandleKey h(1234,key,Gaudi::DataHandle::Reader);
    std::cout << "   key: \"" << key << "\" store: DEFAULT"
              << "   DataObjID: \"" << h.objKey() << "\"  sg: \"" << h.key() 
              << "\"" << std::endl;

    if (fkey != h.objKey()) {
      std::cout << "  Key mismatch. expected \"" << fkey << "\" got \""
                << h.objKey() << "\"\n";
    }
    assert( fkey == h.objKey() );

  } catch (std::exception &e) {
      
    std::cout << "   key: \"" << key << "\" store: DEFAULT";
    if ( b ) {
      std::cout << "  -> UNEXPECTED: " << e.what() << std::endl;
    } else {
      std::cout << "  -> excpected failure: " << e.what() << std::endl;
    }

    assert( !b );

  }
}




int main()
{
  errorcheck::ReportMessage::hideErrorLocus();
  ISvcLocator* pDum;
  Athena_test::initGaudi(pDum); //need MessageSvc

  test("foo",true,"StoreGateSvc+foo");
  test("/foo",false,"");
  test("foo","StoreGateSvc",true,"StoreGateSvc+foo");
  test("foo","StoreGateSvc/StoreGateSvc",true,"StoreGateSvc+foo");
  test("StoreGateSvc+foo", true, "StoreGateSvc+foo");
  test("StoreGateSvc+foo", "StoreGateSvc",true, "StoreGateSvc+foo");

  // compound Condition store
  test("/foo/bar","ConditionStore",true, "ConditionStore+/foo/bar");
  test("/foo/bar","StoreGateSvc/ConditionStore",true,"ConditionStore+/foo/bar");
  test("foo/bar/aaa", "ConditionStore", true, "ConditionStore+foo/bar/aaa");
  test("foo/bar/aaa", "StoreGateSvc/ConditionStore", true, "ConditionStore+foo/bar/aaa");
  test("ConditionStore+foo/bar/aaa", "ConditionStore", true, "ConditionStore+foo/bar/aaa");

  // compound non-Condition store : all fail
  test("/foo/bar",false,"");  
  test("StoreGateSvc+foo/bar",false,"");
  test("foo/bar",false,"");
  test("/foo/bar","OtherStore",false,"");
  test("OtherStore+foo/bar","OtherStore",false,"");
  test("foo/bar/aaa", "OtherStore", false,"");
        
  // blank keys
  test("StoreGateSvc+",true,"");
  test("", "StoreGateSvc",true,"");
  test("/", "StoreGateSvc",false,"");
  test("",true,"");
  test("/",false,"");
  test("", "ConditionStore",true,"");
  test("/", "ConditionStore",false,"");
  test("", "OtherStore",true,"");
  test("OtherStore+", "OtherStore",true,"");
  test("OtherStore+", "StoreGateSvc/OtherStore",true,"");

  // other errors
  test("ConditonStore+foo/",false,"");
  test("foo/","ConditonStore",false,"");
  test("foo/bar/","ConditionStore",false,"");

}
