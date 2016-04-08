// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloIdCnv/LArEM_SuperCell_IDDetDescrCnv.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2012
 * @brief LArEM_SuperCell_ID helper converter class
 */


#ifndef CALOIDCNV_LAREM_SUPERCELL_IDDETDESCRCNV_H
#define CALOIDCNV_LAREM_SUPERCELL_IDDETDESCRCNV_H

#include "CaloIdCnv/CaloIDHelper_IDDetDescrCnv.h"
#include "CaloIdentifier/LArEM_SuperCell_ID.h"


/// Alias for the converter class.
typedef CaloIdCnv::CaloIDHelper_IDDetDescrCnv_T<LArEM_SuperCell_ID>
        LArEM_SuperCell_IDDetDescrCnv;


#endif // CALOIDCNV_LAREM_SUPERCELL_IDDETDESCRCNV_H
