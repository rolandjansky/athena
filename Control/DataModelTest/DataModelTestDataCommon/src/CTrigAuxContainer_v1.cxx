/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataCommon/src/CTrigAuxContainer_v1.xcx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2014
 * @brief Class used for testing xAOD data reading/writing.
 */


#include "DataModelTestDataCommon/versions/CTrigAuxContainer_v1.h"


namespace DMTest {


CTrigAuxContainer_v1::CTrigAuxContainer_v1()
  : xAOD::ByteStreamAuxContainer_v1()
{
  AUX_VARIABLE (anInt);
  AUX_VARIABLE (aFloat);
}


} // namespace DMTest
