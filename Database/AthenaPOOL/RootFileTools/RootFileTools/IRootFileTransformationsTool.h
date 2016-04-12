/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ROOTFILETOOLS_IROOTFILETRANSFORMATIONSTOOL_H
#define ROOTFILETOOLS_IROOTFILETRANSFORMATIONSTOOL_H
/** @file RootFileTransformationsTool.cxx
 *  @brief Tool to call RootFileTransformations to optimise output files
 *  @author Gerhard Brandt <gbrandt@cern.ch>
 *  $Id:  $
 **/

#include "GaudiKernel/IAlgTool.h"

/** @file RootFileTransformationsTool.cxx
 *  @brief Tool to call RootFileTransformations to optimise output files
 **/
class IRootFileTransformationsTool : virtual public IAlgTool {

public:    
   /// Gaudi boilerplate
   static const InterfaceID& interfaceID();

   /// Initialize AlgTool
   virtual StatusCode initialize() = 0;
   /// Finalize AlgTool
   virtual StatusCode finalize() = 0;
   
   // Reorder Baskets
   virtual StatusCode reorderBaskets(std::string, std::string) = 0;
   
   // Resize Baskets
   virtual StatusCode resizeBaskets(std::string, std::string, std::string, std::string, int, float) = 0;

};

inline const InterfaceID& IRootFileTransformationsTool::interfaceID() {
   static const InterfaceID IID("IRootFileTransformationsTool", 1, 0);
   return IID;
}

#endif

