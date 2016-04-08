// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloIdCnv/Tile_SuperCell_IDDetDescrCnv.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2013
 * @brief Tile_SuperCell_ID helper converter class
 */


#ifndef CALOIDCNV_TILE_SUPERCELL_IDDETDESCRCNV_H
#define CALOIDCNV_TILE_SUPERCELL_IDDETDESCRCNV_H

#include "CaloIdCnv/CaloIDHelper_IDDetDescrCnv.h"
#include "CaloIdentifier/Tile_SuperCell_ID.h"


/// Alias for the converter class.
typedef CaloIdCnv::CaloIDHelper_IDDetDescrCnv_T<Tile_SuperCell_ID>
        Tile_SuperCell_IDDetDescrCnv;


#endif // CALOIDCNV_TILE_SUPERCELL_IDDETDESCRCNV_H
