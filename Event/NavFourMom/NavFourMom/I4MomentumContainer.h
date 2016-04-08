///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// I4MomentumContainer.h 
// Header file for class I4MomentumContainer
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef NAVFOURMOM_I4MOMENTUMCONTAINER_H 
#define NAVFOURMOM_I4MOMENTUMCONTAINER_H 

// STL includes

// HepMC / CLHEP includes

// Framework includes
#include "CLIDSvc/CLASS_DEF.h"

// DataModel includes
#include "DataModel/DataVector.h"

// EventKernel includes
#include "EventKernel/I4Momentum.h"

typedef DataVector<I4Momentum> I4MomentumContainer;

// re-use the 'old' way of declaring classes to the ClassIDSvc
CLASS_DEF( DataVector<I4Momentum> , 45630318 , 1 )

#endif //> NAVFOURMOM_I4MOMENTUMCONTAINER_H
