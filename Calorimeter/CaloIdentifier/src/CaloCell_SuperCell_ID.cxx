/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloIdentifier/src/CaloCell_SuperCell_ID.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2012
 * @brief 
 */


#include "CaloIdentifier/CaloCell_SuperCell_ID.h"


CaloCell_SuperCell_ID::CaloCell_SuperCell_ID(const LArEM_SuperCell_ID*   em_id, 
                                             const LArHEC_SuperCell_ID*  hec_id, 
                                             const LArFCAL_SuperCell_ID* fcal_id, 
                                             const LArMiniFCAL_ID*       minifcal_id,
                                             const Tile_SuperCell_ID*    tile_id) : 
  CaloCell_Base_ID (em_id, hec_id, fcal_id, minifcal_id, tile_id, true)
{
}


CaloCell_SuperCell_ID::~CaloCell_SuperCell_ID(void) 
{
}


