// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerTest/MyVec.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2014
 * @brief Test class for D3PD maker.
 */


#ifndef D3PDMAKERTEST_MYVEC_H
#define D3PDMAKERTEST_MYVEC_H


#include "TLorentzVector.h"


namespace D3PDTest {


class MyVec
  : public TLorentzVector
{
public:
  ClassDef(D3PDTest::MyVec, 1);
};


} // namespace D3PDTest


#endif // not D3PDMAKERTEST_MYVEC_H
