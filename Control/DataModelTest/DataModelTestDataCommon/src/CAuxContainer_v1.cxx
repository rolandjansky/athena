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
#include "AthContainersInterfaces/AuxDataOption.h"
#include <stdexcept>


#define CHECK_OPTION(ret)                       \
  do {                                          \
    if (!ret) {                                 \
      ATH_MSG_ERROR("setOption failed");        \
      return StatusCode::FAILURE;               \
    }                                           \
  } while(0)


namespace DMTest {


CAuxContainer_v1::CAuxContainer_v1()
  : xAOD::AuxContainerBase()
{
  AUX_VARIABLE (anInt);
  AUX_VARIABLE (aFloat);
  AUX_VARIABLE (pInt);
  AUX_VARIABLE (pFloat);
  AUX_VARIABLE (pvInt);
  AUX_VARIABLE (pvFloat);

  if (!pInt.setOption (SG::AuxDataOption ("nbits", 17)) ||
      !pFloat.setOption (SG::AuxDataOption ("nbits", 17)) || 
      !pFloat.setOption (SG::AuxDataOption ("signed", 0)) ||
      !pFloat.setOption (SG::AuxDataOption ("nmantissa", 17)) ||
      !pFloat.setOption (SG::AuxDataOption ("scale", 10)) ||

      !pvInt.setOption (SG::AuxDataOption ("nbits", 13)) ||
      !pvFloat.setOption (SG::AuxDataOption ("nbits", 13)) ||
      !pvFloat.setOption (SG::AuxDataOption ("nmantissa", 12)) )
  {
    throw std::runtime_error ("Can't set packing options in CAuxContainer_v1");
  }
}


} // namespace DMTest
