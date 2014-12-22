/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerTest/src/Obj5.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2014
 * @brief Test class for D3PD maker (aux data).
 */


#include "D3PDMakerTest/Obj5.h"


namespace D3PDTest {


Obj5::Obj5()
  : m_obj5(0)
{
}


Obj5::Obj5 (int x)
  : m_obj5 (x+100)
{
}


} // namespace D3PDTest
