///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// egammaContainer_p1.h 
// Header file for class egammaContainer_p1
// Author: K.Cranmer<cranmer@cern.ch>
// Author: S.Binet<binet@cern.ch>
// Date:   December 2006
/////////////////////////////////////////////////////////////////// 
#ifndef EGAMMAEVENTTPCNV_EGAMMACONTAINER_P1_H 
#define EGAMMAEVENTTPCNV_EGAMMACONTAINER_P1_H 

// STL includes
#include <vector>

// egammaEventTPCnv includes
#include "egammaEventTPCnv/egamma_p1.h"

class egammaContainer_p1 : public std::vector<egamma_p1>
{};

#endif //> EGAMMAEVENTTPCNV_EGAMMACONTAINER_P1_H
