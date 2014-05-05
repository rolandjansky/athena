/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
    MyObj *obj = new MyObj("cccc");
    obj->setA<float>(0);    
    obj->setA<double>(0);    
    obj->setA<int>(0);
    obj->setA<long>(0);
    obj->setA<bool>(false);    
    delete obj;
  }
}

#endif
