/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArCalibParamsCnv.cxx
 * @brief Custom implementation of Pool converter for the LArCalibParams data object
 * @author Walter Lampl <walter.lampl@cern.ch>
 */

#include "LArCalibParamsCnv.h"

LArCalibParamsCnv::LArCalibParamsCnv(ISvcLocator* svcloc) : 
  LArCalibParamsCnvBase(svcloc) {}

LArCalibParamsCnv::~LArCalibParamsCnv()
{}


/// Extend base-class conversion method
StatusCode LArCalibParamsCnv::PoolToDataObject(DataObject*& pObj,
                                               const Token* token,
                                               const std::string& key)
{
  StatusCode sc;
  sc=LArCalibParamsCnvBase::PoolToDataObject(pObj,token,key);
  if (sc.isFailure()) return sc;
  LArCalibParams* larCalibParams=NULL;
  SG::fromStorable(pObj,larCalibParams);
  if (!larCalibParams) return StatusCode::FAILURE;
  sc=larCalibParams->initialize();
  return sc;
}
