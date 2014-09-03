/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataCommon/src/CAuxContainer_v1.xcx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2014
 * @brief Class used for testing xAOD data reading/writing.
 */


#include "DataModelTestDataCommon/versions/CAuxContainer_v1.h"


namespace DMTest {


CAuxContainer_v1::CAuxContainer_v1()
  : xAOD::AuxContainerBase()
{
  AUX_VARIABLE (anInt);
  AUX_VARIABLE (aFloat);
}


} // namespace DMTest
