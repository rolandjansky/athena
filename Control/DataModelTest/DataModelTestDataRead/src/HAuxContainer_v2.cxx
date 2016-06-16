/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataRead/src/HAuxContainer_v2.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Test for xAOD schema evolution.
 */


#include "DataModelTestDataRead/versions/HAuxContainer_v2.h"


namespace DMTest {


HAuxContainer_v2::HAuxContainer_v2()
  : xAOD::AuxContainerBase()
{
  AUX_VARIABLE (aFloat);
}


} // namespace DMTest
