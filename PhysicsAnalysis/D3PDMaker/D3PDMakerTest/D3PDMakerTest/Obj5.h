// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerTest/Obj5.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2014
 * @brief Test class for D3PD maker (aux data).
 */


#ifndef D3PDMAKERTEST_OBJ5_H
#define D3PDMAKERTEST_OBJ5_H


#include "AthContainers/DataVector.h"
#include "AthContainers/AuxElement.h"


namespace D3PDTest {


/**
 * @brief Test class for D3PD maker (aux data).
 *
 * This gets put into StoreGate and we fill tuples from it.
 */
class Obj5
  : public SG::AuxElement
{
public:
  Obj5();
  Obj5 (int x);

  int                m_obj5;
};


typedef DataVector<Obj5> Obj5Container;


} // namespace D3PDTest


SG_BASE (D3PDTest::Obj5, SG::AuxElement);
CLASS_DEF (D3PDTest::Obj5,          9384, 1)
CLASS_DEF (D3PDTest::Obj5Container, 9385, 1)


#endif // not D3PDMAKERTEST_OBJ5_H
