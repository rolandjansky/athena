/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerTest/src/Obj2.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2010
 * @brief Test class for D3PD maker.
 */


#include "D3PDMakerTest/Obj2.h"


namespace D3PDTest {


Obj2::Obj2()
  : m_i(0),
    m_f(0)
{
}



Obj2::Obj2 (int x)
  : m_i (x),
    m_f (x + 0.5)
{
}



} // namespace D3PDTest
