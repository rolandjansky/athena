/* Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration */
/**
 * @file xAODEventFormatCnv/src/test/AAuxContainer_v1.xcx
 * @author frank berghaus <fberghaus@anl.gov>
 * @date Sep, 2020
 * @brief Class used for testing xAOD::EventFormat reading/writing.
 */


#include "xAODEventFormatCnv/test/versions/AAuxContainer_v1.h"
#include "AthContainersInterfaces/AuxDataOption.h"
#include <stdexcept>


#define CHECK_OPTION(ret)                       \
  do {                                          \
    if (!ret) {                                 \
      ATH_MSG_ERROR("setOption failed");        \
      return StatusCode::FAILURE;               \
    }                                           \
  } while(0)


namespace xAODMakerTest {


AAuxContainer_v1::AAuxContainer_v1()
  : xAOD::AuxContainerBase()
{
  AUX_VARIABLE (anInt);
  AUX_VARIABLE (aFloat);
  AUX_VARIABLE (pVInt);
  AUX_VARIABLE (pVFloat);

  if (!pVInt.setOption(SG::AuxDataOption("nbits", 13)) ||
      !pVFloat.setOption(SG::AuxDataOption("nbits", 13)) ||
      !pVFloat.setOption(SG::AuxDataOption("nmantissa", 12)) )
  {
    throw std::runtime_error("Can't set packing options in AAuxContainer_v1");
  }
}


} // namespace DMTest
