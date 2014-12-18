///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ILArNoisyROTool.h 
// Header file for class ILArNoisyROTool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef LARELECCALIB_ILARNOISYROTOOL_H
#define LARELECCALIB_ILARNOISYROTOOL_H 1

// STL includes
#include <memory>

// FrameWork includes
#include "GaudiKernel/IAlgTool.h"

// Forward declaration
class CaloCellContainer;
class LArNoisyROSummary;

static const InterfaceID IID_ILArNoisyROTool("ILArNoisyROTool", 1, 0);

class ILArNoisyROTool
  : virtual public ::IAlgTool
{ 
 public: 

  /** Destructor: 
   */
  virtual ~ILArNoisyROTool() {};
  static const InterfaceID& interfaceID();

  virtual 
  std::unique_ptr<LArNoisyROSummary> process(const CaloCellContainer*)=0;

}; 

inline const InterfaceID& ILArNoisyROTool::interfaceID() 
{ 
   return IID_ILArNoisyROTool; 
}


#endif //> !LARELECCALIB_ILARNOISYROTOOL_H
