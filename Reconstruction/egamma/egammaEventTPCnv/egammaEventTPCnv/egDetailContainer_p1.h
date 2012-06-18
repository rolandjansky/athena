///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// egDetailContainer_p1.h 
// Header file for class egDetailContainer_p1
// Author: K.Cranmer<cranmer@cern.ch>
// Author: S.Binet<binet@cern.ch>
// Date:   December 2006
/////////////////////////////////////////////////////////////////// 
#ifndef EGAMMAEVENTTPCNV_EGDETAILCONTAINER_P1_H 
#define EGAMMAEVENTTPCNV_EGDETAILCONTAINER_P1_H 

// STL includes
#include <vector>

// egammaEventTPCnv includes
#include "egammaEventTPCnv/egDetail_p1.h"

class egDetailContainer_p1 : public std::vector<egDetail_p1>
{};

#endif //> EGAMMAEVENTTPCNV_EGDETAILCONTAINER_P1_H
