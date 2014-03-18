/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerTest/src/Obj1.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2010
 * @brief Test class for D3PD maker.
 */


#include "D3PDMakerTest/Obj1.h"
#include <sstream>


namespace D3PDTest {


Obj1::Obj1()
  : m_cstr(0),
    m_c(0),
    m_uc(0),
    m_s(0),
    m_us(0),
    m_i(0),
    m_ui(0),
    m_f(0),
    m_d(0),
    m_ll(0),
    m_ull(0),
    m_b(0)
{
}


Obj1::Obj1 (int x)
  : m_o2 (x + 50)
{
  std::ostringstream ss;
  ss << x;
  m_str = ss.str();
  m_cstr = m_str.c_str();

  m_c = x;
  m_uc = x+1;
  m_s = x+2;
  m_us = x+3;
  m_i = x+4;
  m_ui = x+5;
  m_f = x+6.5;
  m_d = x+7.5;
  m_ll = x+8;
  m_ull = x+9;
  m_b = !!x;
}


} // namespace D3PDTest
