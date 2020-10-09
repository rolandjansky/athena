/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file CaloIdCnv/src/CaloCell_SuperCell_IDDetDescrCnv.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2012
 * @brief CaloCell_SuperCell_ID helper converter class
 */


#include "CaloIdCnv/CaloCell_SuperCell_IDDetDescrCnv.h"
#include "StoreGate/StoreGateSvc.h" 
#include "CaloIdentifier/CaloID.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "AthenaKernel/errorcheck.h"

/**
 * @brief Helper function to actually create a new helper.
 * @param svcloc Gaudi service locator.
 * @param helperKey The StoreGate key of the helper being created.
 * @param idhelper[out] Set to the created helper.
 *
 * We override this for the case of @c CaloCell_SuperCell_ID.
 */
StatusCode make_CaloIDHelper (ISvcLocator* svcloc,
                              const std::string& /*helperKey*/,
                              CaloCell_SuperCell_ID* & idhelper)
{
  const char* ctx = "CaloCell_SuperCell_IDDetDescrCnv";
  // get DetectorStore service
  StoreGateSvc * detStore = 0;
  CHECK_WITH_CONTEXT( svcloc->service("DetectorStore", detStore),  ctx );

  // retrieve LAr and Tile helpers
#define GET_HELPER(TYPE, HELPER) \
  const TYPE* HELPER=0; \
  CHECK_WITH_CONTEXT( detStore->retrieve (HELPER, #TYPE), ctx )

  GET_HELPER (LArEM_SuperCell_ID, em_id);
  GET_HELPER (LArHEC_SuperCell_ID, hec_id);
  GET_HELPER (LArFCAL_SuperCell_ID, fcal_id);
  GET_HELPER (LArMiniFCAL_ID, minifcal_id);
  GET_HELPER (Tile_SuperCell_ID, tile_id);

  // create the helper
  idhelper = new CaloCell_SuperCell_ID (em_id,
                                        hec_id,
                                        fcal_id,
                                        minifcal_id,
                                        tile_id);
  return StatusCode::SUCCESS; 
}


