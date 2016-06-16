/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataWrite/src/HAuxContainer_v1.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Test for xAOD schema evolution.
 */


#include "DataModelTestDataWrite/versions/HAuxContainer_v1.h"
#include "AthContainersInterfaces/AuxDataOption.h"
#include <stdexcept>


namespace DMTest {


HAuxContainer_v1::HAuxContainer_v1()
  : xAOD::AuxContainerBase()
{
  AUX_VARIABLE (anInt);
}


} // namespace DMTest
