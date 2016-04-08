// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloIdCnv/CaloCell_SuperCell_IDDetDescrCnv.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2012
 * @brief CaloCell_SuperCell_ID helper converter class
 */


#ifndef CALOIDCNV_CALOCELL_SUPERCELL_IDDETDESCRCNV_H
#define CALOIDCNV_CALOCELL_SUPERCELL_IDDETDESCRCNV_H


#include "CaloIdCnv/CaloIDHelper_IDDetDescrCnv.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"


/**
 * @brief Helper function to actually create a new helper.
 * @param svcloc Gaudi service locator.
 * @param helperKey The StoreGate key of the helper being created.
 * @param idhelper[out] Set to the created helper.
 *
 * We override this for the case of @c CaloCell_SuperCell_ID.
 */
StatusCode make_CaloIDHelper (ISvcLocator* /*svcloc*/,
                              const std::string& /*helperKey*/,
                              CaloCell_SuperCell_ID* & idhelper);


/// Alias for the converter class.
typedef CaloIdCnv::CaloIDHelper_IDDetDescrCnv_T<CaloCell_SuperCell_ID>
        CaloCell_SuperCell_IDDetDescrCnv;


#endif // CALOIDCNV_CALOCELL_SUPERCELL_IDDETDESCRCNV_H
