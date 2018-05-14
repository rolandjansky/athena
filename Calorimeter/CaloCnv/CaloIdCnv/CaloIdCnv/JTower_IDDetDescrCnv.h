/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**
 * @file CaloIdCnv/JTower_IDDetDescrCnv.h
 * @author Walter Hopkins <whopkins@uoregon.edu>
 * @date Refactored Oct, 2014 from original code
 * @brief JTower_ID helper converter class
 */


#ifndef CALOIDCNV_JTOWER_IDDETDESCRCNV_H
#define CALOIDCNV_JTOWER_IDDETDESCRCNV_H


#include "CaloIdCnv/CaloIDHelper_IDDetDescrCnv.h"
#include "CaloIdentifier/JTower_ID.h"


/// Alias for the converter class.
typedef CaloIdCnv::CaloIDHelper_IDDetDescrCnv_T<JTower_ID>
        JTower_IDDetDescrCnv;


#endif // CALOIDCNV_JTOWER_IDDETDESCRCNV_H
