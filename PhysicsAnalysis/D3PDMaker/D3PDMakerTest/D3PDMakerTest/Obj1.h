// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerTest/Obj1.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2010
 * @brief Test class for D3PD maker.
 */


#ifndef D3PDMAKERTEST_OBJ1_H
#define D3PDMAKERTEST_OBJ1_H


#include "D3PDMakerTest/Obj2.h"
#include "AthContainers/DataVector.h"
#include "AthenaKernel/BaseInfo.h"
#include "AthenaKernel/CLASS_DEF.h"
#include <string>
#include <vector>


namespace D3PDTest {



/**
 * @brief Test class for D3PD maker.
 *
 * This gets put into StoreGate and we fill tuples from it.
 */
class Obj1
{
public:
  Obj1();
  Obj1 (int x);

  std::string        m_str;
  const char*        m_cstr;
  char               m_c;
  unsigned char      m_uc;
  short              m_s;
  unsigned short     m_us;
  int                m_i;
  unsigned int       m_ui;
  float              m_f;
  double             m_d;
  long long          m_ll;
  unsigned long long m_ull;
  bool               m_b;

  Obj2               m_o2;
  std::vector<Obj2>  m_vo2;
  std::vector<Obj2>  m_vo3;
};



class Obj1Container
  : public DataVector<Obj1>
{
public:
};



} // namespace D3PDTest


SG_BASE (D3PDTest::Obj1Container, DataVector<D3PDTest::Obj1>);

CLASS_DEF (D3PDTest::Obj1,          9372, 1)
CLASS_DEF (D3PDTest::Obj1Container, 9373, 1)


#endif // not D3PDMAKERTEST_OBJ1_H
