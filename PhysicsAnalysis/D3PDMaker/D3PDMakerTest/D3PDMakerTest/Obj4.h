// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerTest/Obj4.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2011
 * @brief Test class for D3PD maker.
 */


#ifndef D3PDMAKERTEST_OBJ4_H
#define D3PDMAKERTEST_OBJ4_H


#include "D3PDMakerTest/Obj1.h"
#include "AthContainers/DataVector.h"


namespace D3PDTest {


/**
 * @brief Test class for D3PD maker.
 *
 * This gets put into StoreGate and we fill tuples from it.
 */
class Obj4
  : public Obj1
{
public:
  Obj4();
  Obj4 (int x);

  int                m_obj4;
};


} // namespace D3PDTest


DATAVECTOR_BASE (D3PDTest::Obj4, D3PDTest::Obj1);


namespace D3PDTest {


class Obj4Container
  : public DataVector<Obj4>
{
public:
};



} // namespace D3PDTest


SG_BASE (D3PDTest::Obj4Container, DataVector<D3PDTest::Obj4>);

CLASS_DEF (D3PDTest::Obj4,          9382, 1)
CLASS_DEF (D3PDTest::Obj4Container, 9383, 1)



#endif // not D3PDMAKERTEST_OBJ4_H
