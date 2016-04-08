// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloIdCnv/LArFCAL_SuperCell_IDDetDescrCnv.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2012
 * @brief LArFCAL_SuperCell_ID helper converter class
 */


#ifndef CALOIDCNV_LARFCAL_SUPERCELL_IDDETDESCRCNV_H
#define CALOIDCNV_LARFCAL_SUPERCELL_IDDETDESCRCNV_H

#include "CaloIdCnv/CaloIDHelper_IDDetDescrCnv.h"
#include "CaloIdentifier/LArFCAL_SuperCell_ID.h"


/// Alias for the converter class.
typedef CaloIdCnv::CaloIDHelper_IDDetDescrCnv_T<LArFCAL_SuperCell_ID>
        LArFCAL_SuperCell_IDDetDescrCnv;


#endif // CALOIDCNV_LARFCAL_SUPERCELL_IDDETDESCRCNV_H
