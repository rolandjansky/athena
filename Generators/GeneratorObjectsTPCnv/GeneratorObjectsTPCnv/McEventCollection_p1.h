///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// McEventCollection_p1.h 
// Header file for class McEventCollection_p1
// Author: S.Binet<binet@cern.ch>
// Date:   April 2006
/////////////////////////////////////////////////////////////////// 
#ifndef GENERATOROBJECTSTPCNV_MCEVENTCOLLECTION_P1_H 
#define GENERATOROBJECTSTPCNV_MCEVENTCOLLECTION_P1_H 

// STL includes
#include <vector>

// GeneratorObjectsTPCnv includes
#include "GeneratorObjectsTPCnv/GenEvent_p1.h"

// forward declarations

class McEventCollection_p1 : public std::vector<GenEvent_p1>
{
};

#endif //> GENERATOROBJECTSTPCNV_MCEVENTCOLLECTION_P1_H
