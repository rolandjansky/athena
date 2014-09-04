/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TUTORIAL_CLASSDEF_H
#define TUTORIAL_CLASSDEF_H
//Put here the CLASS_DEF macros for the STL containers you put in StoreGate

#include <vector>
#include "MyElement.h"

#include "SGTools/CLASS_DEF.h"

CLASS_DEF(std::vector<SGTutorial::MyElement>, 9903, 1)
     //the third field is the version which is currently ignored
#endif
