/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataRead/src/G_v1.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2014
 * @brief Test for xAOD auto schema evolution.
 */


#include "DataModelTestDataRead/versions/G_v1.h"
#include "xAODCore/AuxStoreAccessorMacros.h"


namespace DMTest {


AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (G_v1, int,    anInt,  setAnInt)


} // namespace DMTest
