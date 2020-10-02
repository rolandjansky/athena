/* Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration */
/**
 * @file xAODEventFormatCnv/src/test/A_v1.cxx
 * @author frank berghaus <fberghaus@anl.gov>
 * @date May, 2020
 * @brief Class used for testing xAOD::EventFormat reading/writing.
 */
#include "xAODEventFormatCnv/test/versions/A_v1.h"
#include "xAODCore/AuxStoreAccessorMacros.h"


namespace xAODMakerTest {


AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (A_v1, float,        aFloat, aFloat)
AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (A_v1, int,          anInt,  anInt)

AUXSTORE_OBJECT_SETTER_AND_GETTER (A_v1, std::vector<int>, pVInt, pVInt)
AUXSTORE_OBJECT_MOVE (A_v1, std::vector<int>, pVInt, pVInt)

AUXSTORE_OBJECT_SETTER_AND_GETTER (A_v1, std::vector<float>, pVFloat, pVFloat)
AUXSTORE_OBJECT_MOVE (A_v1, std::vector<float>, pVFloat, pVFloat)


}  // namespace xAODMakerTest
