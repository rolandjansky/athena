/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETRUTHVERTEXVALIDATION_LINKDEF_H
#define INDETRUTHVERTEXVALIDATION_LINKDEF_H

#include "InDetTruthVertexValidation/InDetVertexTruthMatchUtils.h"
#include "InDetTruthVertexValidation/InDetVertexTruthMatchTool.h"

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclass;

#pragma link C++ class InDetVertexTruthMatchTool;
#pragma link C++ class InDetVertexTruthMatchUtils;

#endif

#endif