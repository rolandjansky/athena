/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataCommon/src/CInfoAuxContainer_v1.xcx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2014
 * @brief Class used for testing xAOD data reading/writing.
 */


#include "DataModelTestDataCommon/versions/CInfoAuxContainer_v1.h"


namespace DMTest {


CInfoAuxContainer_v1::CInfoAuxContainer_v1()
  : xAOD::AuxInfoBase()
{
  AUX_VARIABLE (anInt);
  AUX_VARIABLE (aFloat);
}


} // namespace DMTest
