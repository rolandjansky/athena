/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PYANALYSISEXAMPLES_PYANALYSISEXAMPLESDICT_H
#define PYANALYSISEXAMPLES_PYANALYSISEXAMPLESDICT_H

#include "PyAnalysisExamples/MyObj.h"
#include "PyAnalysisExamples/MyCutClass.h"
#include "PyAnalysisExamples/MyDetObj.h"
#include "PyAnalysisExamples/MyTool.h"

namespace  PyAnalysisExamplesDict
{
  void tmp ()
  {
    auto obj = std::make_unique<AthPyEx::MyObj>("cccc");
    obj->setA<float>(0);    
    obj->setA<double>(0);    
    obj->setA<int>(0);
    obj->setA<long>(0);
    obj->setA<bool>(false);    
  }
}

#endif
