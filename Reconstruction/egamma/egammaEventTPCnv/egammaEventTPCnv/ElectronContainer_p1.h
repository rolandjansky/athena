///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ElectronContainer_p1.h 
// Header file for class ElectronContainer_p1
// Author: K.Cranmer<cranmer@cern.ch>
// Author: S.Binet<binet@cern.ch>
// Date:   December 2006
/////////////////////////////////////////////////////////////////// 
#ifndef EGAMMAEVENTTPCNV_ELECTRONCONTAINER_P1_H 
#define EGAMMAEVENTTPCNV_ELECTRONCONTAINER_P1_H 

// STL includes

// egammaEventTPCnv includes
#include "egammaEventTPCnv/Electron_p1.h"

class ElectronContainer_p1 : public std::vector<Electron_p1>
{};

#endif //> EGAMMAEVENTTPCNV_ELECTRONCONTAINER_P1_H
