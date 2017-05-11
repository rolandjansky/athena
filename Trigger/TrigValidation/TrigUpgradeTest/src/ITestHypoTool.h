/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGUPGRADETEST_ITESTHYPOTOOL_H
#define TRIGUPGRADETEST_ITESTHYPOTOOL_H 1

#include "GaudiKernel/IAlgTool.h"

namespace HLTTest {

static const InterfaceID IID_ITestHypoTool("ITestHypoTool", 1, 0);

/**
 * @class $(klass)s
 * @brief 
 **/

class ITestHypoTool
  : virtual public ::IAlgTool
{ 

 public: 

  virtual ~ITestHypoTool();

  static const InterfaceID& interfaceID();

 protected: 

 private:

}; 

inline const InterfaceID& ITestHypoTool::interfaceID() 
{ 
   return IID_ITestHypoTool; 
}

} //> end namespace HLTTest
#endif //> !TRIGUPGRADETEST_ITESTHYPOTOOL_H
