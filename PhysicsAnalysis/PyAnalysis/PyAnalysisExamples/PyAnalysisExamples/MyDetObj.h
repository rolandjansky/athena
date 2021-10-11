/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PYANALYSISEXAMPLES_MYDETOBJ_H
#define PYANALYSISEXAMPLES_MYDETOBJ_H

#include <string>

#include "AthenaKernel/CLASS_DEF.h"

namespace AthPyEx {

class MyDetObj
{
public:
  MyDetObj (const std::string &name) { m_name = name; }
  virtual ~MyDetObj () {}

  const std::string name () { return m_name; }

private:
  std::string m_name;
};

} // namespace AthPyEx

CLASS_DEF(AthPyEx::MyDetObj, 105730742, 1)

#endif

