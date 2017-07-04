/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKDetectorToolI_H
#define FTKDetectorToolI_H

#include "GaudiKernel/IAlgTool.h"

#include <set>

class FTKRegionMap;
class FTKPlaneMap;
class FTKSSMap;

/** Must declare this, with name of interface*/
static const InterfaceID IID_FTKDetectorTool("FTKDetectorToolI", 1, 0);

class FTKDetectorToolI : virtual public IAlgTool 
{
  
 public:
  /**Declared here, and defined below*/
  static const InterfaceID& interfaceID();
  
  /**Pure virtual*/
  virtual void dumpDeadModuleSummary() = 0;
  virtual std::set<int> *getWildcardList(int) = 0;
  virtual void makeBadModuleMap() = 0;

  virtual void dumpModuleIDMap() = 0;

  virtual void dumpGlobalToLocalModuleMap() = 0;

  virtual void dumpModulePositions() =0;

  virtual void dumpIDMap() = 0;
};

inline const InterfaceID& FTKDetectorToolI::interfaceID()
{ 
  return IID_FTKDetectorTool; 
}

#endif //FTKDetectorToolI_H
