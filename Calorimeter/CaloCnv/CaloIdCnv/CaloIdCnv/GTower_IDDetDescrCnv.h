/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CaloIdCnv/GTower_IDDetDescrCnv.h
 * @author Walter Hopkins <whopkins@uoregon.edu>
 * @date Refactored Oct, 2014 from original code
 * @brief GTower_ID helper converter class
 */


#ifndef CALOIDCNV_GTOWER_IDDETDESCRCNV_H
#define CALOIDCNV_GTOWER_IDDETDESCRCNV_H


#include "CaloIdCnv/CaloIDHelper_IDDetDescrCnv.h"
#include "CaloIdentifier/GTower_ID.h"


/// Alias for the converter class.
typedef CaloIdCnv::CaloIDHelper_IDDetDescrCnv_T<GTower_ID>
        GTower_IDDetDescrCnv;


#endif // CALOIDCNV_GTOWER_IDDETDESCRCNV_H
