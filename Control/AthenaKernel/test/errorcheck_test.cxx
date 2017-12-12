/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: errorcheck_test.cxx,v 1.6 2009-04-09 15:11:18 ssnyder Exp $
/**
 * @file  errorcheck_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2006
 * @brief Regression tests for errorcheck.
 */

#undef NDEBUG

#include "AthenaKernel/errorcheck.h"
#include "TestTools/initGaudi.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/Bootstrap.h"
#include <cassert>


class Algtest
  : public Algorithm
{
public:
  Algtest()
    : Algorithm ("algname", Gaudi::svcLocator()) {}
  virtual StatusCode execute() { return StatusCode (StatusCode::SUCCESS); }
  StatusCode test1();
};


StatusCode Algtest::test1()
{
  REPORT_ERROR (StatusCode (StatusCode::FAILURE)) << "foomsg";
  REPORT_MESSAGE (MSG::INFO) << "some info";
  CHECK( StatusCode (StatusCode::SUCCESS) );
  CHECK( StatusCode (StatusCode::FAILURE) );
  return StatusCode (StatusCode::SUCCESS);
}


class Algtooltest
  : public AlgTool
{
public:
  Algtooltest (IInterface* parent)
    : AlgTool ("tooltype", "toolname", parent) {}
  StatusCode test1();
};


StatusCode Algtooltest::test1()
{
  REPORT_ERROR (StatusCode (StatusCode::FAILURE)) << "foomsg";
  REPORT_MESSAGE (MSG::INFO) << "some info";
  CHECK_CODE( StatusCode (StatusCode::SUCCESS), StatusCode(123) );
  CHECK_CODE( StatusCode (StatusCode::RECOVERABLE), StatusCode(123) );
  return StatusCode (StatusCode::SUCCESS);
}


class Servtest
  : public Service
{
public:
  Servtest()
    : Service ("servname", Gaudi::svcLocator()) {}
  StatusCode test1();
};


StatusCode Servtest::test1()
{
  REPORT_ERROR (StatusCode (StatusCode::FAILURE)) << "foomsg";
  REPORT_MESSAGE (MSG::INFO) << "some info";
  CHECK_RECOVERABLE( StatusCode (StatusCode::SUCCESS) );
  CHECK_RECOVERABLE( StatusCode (StatusCode::FAILURE) );
  return StatusCode (StatusCode::SUCCESS);
}


class Test
{
public:
  StatusCode test1();
  int test2();
};


StatusCode Test::test1()
{
  REPORT_ERROR (StatusCode (StatusCode::FAILURE)) << "foomsg";
  REPORT_MESSAGE (MSG::INFO) << "some info";
  CHECK_FATAL( StatusCode (StatusCode::SUCCESS) );
  CHECK_FATAL( StatusCode (StatusCode::RECOVERABLE) );
  return StatusCode (StatusCode::SUCCESS);
}

int Test::test2()
{
  CHECK( StatusCode (StatusCode::SUCCESS), 0 );
  CHECK( StatusCode (StatusCode::FAILURE), -1 );
  return 0;
}



StatusCode test1a()
{
  CHECK_WITH_CONTEXT( StatusCode (StatusCode::SUCCESS), "alg" );
  CHECK_WITH_CONTEXT( StatusCode (StatusCode::FAILURE), "alg" );
  return StatusCode (StatusCode::SUCCESS);
}


StatusCode test1b()
{
  CHECK_RECOVERABLE_WITH_CONTEXT( StatusCode (StatusCode::SUCCESS), "alg" );
  CHECK_RECOVERABLE_WITH_CONTEXT( StatusCode (StatusCode::FAILURE), "alg" );
  return StatusCode (StatusCode::SUCCESS);
}


StatusCode test1c()
{
  CHECK_RECOVERABLE_WITH_CONTEXT( StatusCode (StatusCode::SUCCESS), "alg" );
  CHECK_RECOVERABLE_WITH_CONTEXT( StatusCode (StatusCode::RECOVERABLE),"alg" );
  return StatusCode (StatusCode::SUCCESS);
}


StatusCode test1d()
{
  CHECK_FATAL_WITH_CONTEXT( StatusCode (StatusCode::SUCCESS), "alg" );
  CHECK_FATAL_WITH_CONTEXT( StatusCode (StatusCode::FAILURE), "alg" );
  return StatusCode (StatusCode::SUCCESS);
}


StatusCode test1e()
{
  CHECK_FATAL_WITH_CONTEXT( StatusCode (StatusCode::SUCCESS), "alg" );
  CHECK_FATAL_WITH_CONTEXT( StatusCode (StatusCode::RECOVERABLE), "alg" );
  return StatusCode (StatusCode::SUCCESS);
}


StatusCode test1f()
{
  CHECK_CODE_WITH_CONTEXT( StatusCode (StatusCode::SUCCESS), "alg", StatusCode(123) );
  CHECK_CODE_WITH_CONTEXT( StatusCode (StatusCode::RECOVERABLE), "alg", StatusCode(123) );
  return StatusCode (StatusCode::SUCCESS);
}

int test1g()
{
  CHECK_WITH_CONTEXT( StatusCode (StatusCode::SUCCESS), "alg", 0 );
  CHECK_WITH_CONTEXT( StatusCode (StatusCode::FAILURE), "alg", -1 );
  return 0;
}

StatusCode test1()
{
  Algtest algtest;  algtest.addRef();
  Algtooltest algtooltest (&algtest);  algtooltest.addRef();
  Servtest servtest;  servtest.addRef();
  Test test;
  REPORT_ERROR_WITH_CONTEXT (StatusCode (StatusCode::FAILURE), "alg")
    << "foomsg";
  REPORT_MESSAGE_WITH_CONTEXT (MSG::INFO, "alg")
    << "some message";
  {
    errorcheck::ReportMessage msg (MSG::INFO, ERRORCHECK_ARGS, "alg");
    msg << "a... ";
    msg << "b";
  }
  REPORT_MESSAGE_WITH_CONTEXT (MSG::INFO, "alg") << "foo" << endmsg;
  assert( test1a().isFailure() );
  assert( test1b().isFailure() );
  assert( test1c().isFailure() );
  assert( test1d().isFailure() );
  assert( test1e().isFailure() );
  assert( test1f().isFailure() );
  assert( test1g()==-1 );
  assert( algtest.test1().isFailure() );
  assert( algtooltest.test1().isFailure() );
  assert( servtest.test1().isFailure() );
  assert( test.test1().isFailure() );
  assert( test.test2()==-1 );

  errorcheck::ReportMessage::hideErrorLocus();
  REPORT_ERROR_WITH_CONTEXT (StatusCode (StatusCode::FAILURE), "alg")
    << "foox";
  errorcheck::ReportMessage::hideErrorLocus (false);
  REPORT_ERROR_WITH_CONTEXT (StatusCode (StatusCode::FAILURE), "alg")
    << "fooy";
  return StatusCode (StatusCode::SUCCESS);
}


namespace errorcheck {
std::string clean_allocator (std::string f);
}


void test2 (std::vector<int> = std::vector<int>(),
            const int* = 0,
            int (*)() = 0,
            int [] = 0)
{
  REPORT_MESSAGE_WITH_CONTEXT (MSG::INFO, "test2") << "test2" << endmsg;

  assert (errorcheck::clean_allocator ("void test2(std::vector<int, std::allocator<int> >, const int*, int (*)(), int*)") ==
          "void test2(std::vector<int>, const int*, int (*)(), int*)");
}


void test3 (const std::string& = "", int = 0)
{
  REPORT_MESSAGE_WITH_CONTEXT (MSG::INFO, "test3") << "test3" << endmsg;
}


int main()
{
  ISvcLocator* loc;
  assert( Athena_test::initGaudi (loc) );
  test1();
  test2();
  test3();
  return 0;
}
