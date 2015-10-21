/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_IATHENAOUTPUTTOOL_H
#define ATHENAKERNEL_IATHENAOUTPUTTOOL_H

/** @file IAthenaOutputTool.h
 *  @brief This file contains the class definition for the IAthenaOutputTool class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: IAthenaOutputTool.h,v 1.2 2008-05-27 21:12:34 gemmeren Exp $
 **/

// Gaudi
#include "GaudiKernel/IAlgTool.h"

/** @class IAthenaOutputTool
 *  @brief This class provides the interface for AthenaOutputTool classes used by AthenaOutputStream.
 **/
class IAthenaOutputTool : virtual public IAlgTool {

public:    
   /// Gaudi boilerplate
   static const InterfaceID& interfaceID();

   /// Initialize AlgTool
   virtual StatusCode initialize() = 0;
   /// Called at the end of initialize
   virtual StatusCode postInitialize() = 0;
   /// Called at the beginning of execute
   virtual StatusCode preExecute() = 0;
   /// Called at the end of execute
   virtual StatusCode postExecute() = 0;
   /// Called at the beginning of finalize
   virtual StatusCode preFinalize() = 0;
   /// Finalize AlgTool
   virtual StatusCode finalize() = 0;
};

inline const InterfaceID& IAthenaOutputTool::interfaceID() {
   static const InterfaceID IID("IAthenaOutputTool", 1, 0);
   return IID;
}

#endif
