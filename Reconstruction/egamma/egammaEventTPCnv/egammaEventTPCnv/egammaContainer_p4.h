///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// egammaContainer_p4.h 
// Header file for class egammaContainer_p4
/////////////////////////////////////////////////////////////////// 
#ifndef EGAMMAEVENTTPCNV_EGAMMACONTAINER_P4_H 
#define EGAMMAEVENTTPCNV_EGAMMACONTAINER_P4_H 1

// STL includes
#include <vector>

// egammaEventTPCnv includes
#include "egammaEventTPCnv/egamma_p4.h"

class egammaContainer_p4 : public std::vector<egamma_p4>
{};

#endif //> EGAMMAEVENTTPCNV_EGAMMACONTAINER_P4_H
