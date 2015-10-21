/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_IATHENASELECTORTOOL_H
#define ATHENAKERNEL_IATHENASELECTORTOOL_H

/** @file IAthenaSelectorTool.h
 *  @brief This file contains the class definition for the IAthenaSelectorTool class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: IAthenaSelectorTool.h,v 1.4 2008-06-09 13:45:54 gemmeren Exp $
 **/

// Gaudi
#include "GaudiKernel/IAlgTool.h"

/** @class IAthenaSelectorTool
 *  @brief This class provides the interface for AthenaSelectorTool classes used by AthenaEventSelector.
 **/
class IAthenaSelectorTool : virtual public IAlgTool {

public:    
   /// Gaudi boilerplate
   static const InterfaceID& interfaceID();

   /// Initialize AlgTool
   virtual StatusCode initialize() = 0;
   /// Called at the end of initialize
   virtual StatusCode postInitialize() = 0;
   /// Called at the beginning of next
   virtual StatusCode preNext() = 0;
   /// Called at the end of next
   virtual StatusCode postNext() = 0;
   /// Called at the beginning of finalize
   virtual StatusCode preFinalize() = 0;
   /// Finalize AlgTool
   virtual StatusCode finalize() = 0;
};

inline const InterfaceID& IAthenaSelectorTool::interfaceID() {
   static const InterfaceID IID("IAthenaSelectorTool", 1, 0);
   return IID;
}

#endif
