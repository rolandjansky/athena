// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloIdCnv/TileIDDetDescrCnv.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Refactored Aug, 2012 from original code
 * @brief TileID helper converter class
 */


#ifndef CALOIDCNV_TILEIDDETDESCRCNV_H
#define CALOIDCNV_TILEIDDETDESCRCNV_H

#include "CaloIdCnv/CaloIDHelper_IDDetDescrCnv.h"
#include "CaloIdentifier/TileID.h"


/// Alias for the converter class.
typedef CaloIdCnv::CaloIDHelper_IDDetDescrCnv_T<TileID>
        TileIDDetDescrCnv;


#endif // CALOIDCNV_TILEIDDETDESCRCNV_H

