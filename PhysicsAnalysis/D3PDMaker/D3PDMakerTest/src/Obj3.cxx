/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerTest/src/Obj3.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2010
 * @brief Test class for D3PD maker.
 */


#include "D3PDMakerTest/Obj3.h"


namespace D3PDTest {


Obj3::Obj3()
{
}


Obj3::Obj3 (double pt, double eta, double phi, double m)
{
  momentumBase().setPt  (pt);
  momentumBase().setEta (eta);
  momentumBase().setPhi (phi);
  momentumBase().setM   (m);
}


} // namespace D3PDTest
