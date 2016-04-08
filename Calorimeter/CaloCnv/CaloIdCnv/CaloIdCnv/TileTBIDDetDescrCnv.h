// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloIdCnv/TileTBIDDetDescrCnv.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Refactored Aug, 2012 from original code
 * @brief TileTBID helper converter class
 */


#ifndef CALOIDCNV_TILETBIDDETDESCRCNV_H
#define CALOIDCNV_TILETBIDDETDESCRCNV_H


#include "CaloIdCnv/CaloIDHelper_IDDetDescrCnv.h"
#include "CaloIdentifier/TileTBID.h"


/// Alias for the converter class.
typedef CaloIdCnv::CaloIDHelper_IDDetDescrCnv_T<TileTBID>
        TileTBIDDetDescrCnv;


#endif // CALOIDCNV_TILETBIDDETDESCRCNV_H

