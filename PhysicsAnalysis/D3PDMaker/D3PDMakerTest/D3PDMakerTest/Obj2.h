// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerTest/Obj2.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2010
 * @brief Test class for D3PD maker.
 */


#ifndef D3PDMAKERTEST_OBJ2_H
#define D3PDMAKERTEST_OBJ2_H


namespace D3PDTest {



/**
 * @brief Test class for D3PD maker.
 */
class Obj2
{
public:
  Obj2();
  Obj2(int x);

  int   m_i;
  float m_f;
};


} // namespace D3PDTest


#endif // not D3PDMAKERTEST_OBJ2_H
