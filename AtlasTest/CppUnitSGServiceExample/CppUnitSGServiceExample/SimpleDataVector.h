/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include "DataModel/DataVector.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

using namespace std;

struct SimpleTestStruct {
  int m_int;
  float m_float;
  string m_string;
  virtual ~SimpleTestStruct()
  {
    //    cout << " ----> Destructor of SimpleTestStruct called for " << this << endl;
  }
  SimpleTestStruct() : m_int(1), m_float(2.), m_string("SimpleString") { }
  SimpleTestStruct(int j) : m_int(j), m_float(2.), m_string("SimpleString") { }
  SimpleTestStruct(const SimpleTestStruct& aaa) :
  m_int(aaa.m_int), m_float(2.),
  m_string("SimpleString") { }
  void foo() { /* cout << "foo called" << m_int << endl; */}
  void fooMessageSvc() {
    MsgStream log( Athena::getMessageSvc(), "SimpleDataVector" );
    log << MSG::INFO
	<< "In SimpleDataVector Gaudi message service is called -" << endl 
        << "and it prints the value of integer data member:" << m_int 
        << endreq;
  }
};

class SimpleTestContainer : public DataVector<SimpleTestStruct>
{
 public:
  SimpleTestContainer() : DataVector<SimpleTestStruct>() { };
  virtual ~SimpleTestContainer() {}
};
                                                                                
