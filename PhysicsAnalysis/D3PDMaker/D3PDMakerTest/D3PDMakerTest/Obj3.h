// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerTest/Obj3.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2010
 * @brief Test class for D3PD maker.
 */


#ifndef D3PDMAKERTEST_OBJ3_H
#define D3PDMAKERTEST_OBJ3_H


#include "ParticleEvent/ParticleImpl.h"
#include "FourMom/P4PtEtaPhiM.h"
#include "Navigation/NavigableTerminalNode.h"


namespace D3PDTest {


/**
 * @brief Test class for D3PD maker.
 */
class Obj3
  : public ParticleImpl<NavigableTerminalNode, P4PtEtaPhiM>
{
public:
  Obj3();
  Obj3 (double pt, double eta, double phi, double m);
};


}


#endif // not D3PDMAKERTEST_OBJ3_H
