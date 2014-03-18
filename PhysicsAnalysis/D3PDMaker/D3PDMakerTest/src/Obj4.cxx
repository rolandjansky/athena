/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerTest/src/Obj4.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2010
 * @brief Test class for D3PD maker.
 */


#include "D3PDMakerTest/Obj4.h"


namespace D3PDTest {


Obj4::Obj4()
  : m_obj4(0)
{
}


Obj4::Obj4 (int x)
  : Obj1 (x),
    m_obj4 (x+100)
{
}


} // namespace D3PDTest
