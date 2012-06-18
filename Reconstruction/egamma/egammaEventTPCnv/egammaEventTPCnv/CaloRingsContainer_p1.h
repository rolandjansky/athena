///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CaloRingsContainer_p1.h 
// Header file for class CaloRingsContainer_p1
// Author: D.E.Ferreira de Lima<dferreir@mail.cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef RECTPCNV_CALORINGSCONTAINER_P1_H 
#define RECTPCNV_CALORINGSCONTAINER_P1_H 

// STL includes
#include <vector>

// RecTPCnv includes
#include "egammaEventTPCnv/CaloRings_p1.h"

class CaloRingsContainer_p1 : public std::vector<CaloRings_p1>
{};

#endif //> RECTPCNV_CALORINGSCONTAINER_P1_H
