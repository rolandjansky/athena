// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloIdCnv/LArHEC_SuperCell_IDDetDescrCnv.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2013
 * @brief LArHEC_SuperCell_ID helper converter class
 */


#ifndef CALOIDCNV_LARHEC_SUPERCELL_IDDETDESCRCNV_H
#define CALOIDCNV_LARHEC_SUPERCELL_IDDETDESCRCNV_H

#include "CaloIdCnv/CaloIDHelper_IDDetDescrCnv.h"
#include "CaloIdentifier/LArHEC_SuperCell_ID.h"


/// Alias for the converter class.
typedef CaloIdCnv::CaloIDHelper_IDDetDescrCnv_T<LArHEC_SuperCell_ID>
        LArHEC_SuperCell_IDDetDescrCnv;


#endif // CALOIDCNV_LARHEC_SUPERCELL_IDDETDESCRCNV_H
