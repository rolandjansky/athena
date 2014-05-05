/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PYANALYSISEXAMPLES_MYDETOBJ_H
#define PYANALYSISEXAMPLES_MYDETOBJ_H

#include <string>

#include "CLIDSvc/CLASS_DEF.h"

class MyDetObj
{
public:
  MyDetObj (const std::string &name) { m_name = name; }
  virtual ~MyDetObj () {}

  const std::string name () { return m_name; }

private:
  std::string m_name;
};

CLASS_DEF(MyDetObj, 28914872, 1)

#endif

