// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloIdCnv/LArEM_IDDetDescrCnv.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Refactored Aug, 2012 from original code
 * @brief LArEM_ID helper converter class
 */


#ifndef CALOIDCNV_LAREM_IDDETDESCRCNV_H
#define CALOIDCNV_LAREM_IDDETDESCRCNV_H


#include "CaloIdCnv/CaloIDHelper_IDDetDescrCnv.h"
#include "CaloIdentifier/LArEM_ID.h"


/// Alias for the converter class.
typedef CaloIdCnv::CaloIDHelper_IDDetDescrCnv_T<LArEM_ID>
        LArEM_IDDetDescrCnv;


#endif // CALOIDCNV_LAREM_IDDETDESCRCNV_H
