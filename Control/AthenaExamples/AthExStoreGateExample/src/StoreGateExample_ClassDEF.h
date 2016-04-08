// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STOREGATEEXAMPLE_CLASSDEF_H
#define STOREGATEEXAMPLE_CLASSDEF_H

#include <list>
#include <vector>
#include "SGTools/StlVectorClids.h"
#include "SGTools/CLASS_DEF.h"
#include "AthContainers/DataVector.h"
#include "AthLinks/ElementLink.h"
#include "MyContObj.h"
#include "MapStringFloat.h"

CLASS_DEF(DataVector<MyContObj>, 9967, 1)
// CLASS_DEF(std::vector<float>, 9980, 1) //--> done by StlVectorClids
CLASS_DEF(std::list<MyContObj>, 9981, 1)
CLASS_DEF(std::vector< ElementLink<MapStringFloat> >, 9983, 1)
CLASS_DEF(std::list< ElementLink< std::vector<float> > >, 9984, 1)

#endif
