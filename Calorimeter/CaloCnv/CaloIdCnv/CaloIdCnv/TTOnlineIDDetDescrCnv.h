// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloIdCnv/TTOnlineIDDetDescrCnv.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Refactored Aug, 2012 from original code
 * @brief TTOnlineID helper converter class
 */


#ifndef CALOIDCNV_TTONLINEIDDETDESCRCNV_H
#define CALOIDCNV_TTONLINEIDDETDESCRCNV_H


#include "CaloIdCnv/CaloIDHelper_IDDetDescrCnv.h"
#include "CaloIdentifier/TTOnlineID.h"


/// Alias for the converter class.
typedef CaloIdCnv::CaloIDHelper_IDDetDescrCnv_T<TTOnlineID>
        TTOnlineIDDetDescrCnv;


#endif // LARIDCNV_TTOnlineIDDETDESCRCNV_H
