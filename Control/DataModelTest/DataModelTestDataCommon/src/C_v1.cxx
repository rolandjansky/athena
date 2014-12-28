/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataCommon/src/C_v1.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2014
 * @brief Class used for testing xAOD data reading/writing.
 */


#include "DataModelTestDataCommon/versions/C_v1.h"
#include "xAODCore/AuxStoreAccessorMacros.h"


namespace DMTest {


AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (C_v1, float,  aFloat, setAFloat)
AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (C_v1, int,    anInt,  setAnInt)
AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (C_v1, unsigned int,    pInt,  setPInt)
AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (C_v1, float,    pFloat,  setPFloat)

AUXSTORE_OBJECT_SETTER_AND_GETTER (C_v1, std::vector<int>,    pvInt,  setPVInt)
AUXSTORE_OBJECT_MOVE (C_v1, std::vector<int>,    pvInt,  setPVInt)

AUXSTORE_OBJECT_SETTER_AND_GETTER (C_v1, std::vector<float>,    pvFloat,  setPVFloat)
AUXSTORE_OBJECT_MOVE (C_v1, std::vector<float>,    pvFloat,  setPVFloat)


} // namespace DMTest
