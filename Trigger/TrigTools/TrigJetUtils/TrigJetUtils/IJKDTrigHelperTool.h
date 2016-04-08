///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IJKDTrigHelperTool.h 
// Header file for class IJKDTrigHelperTool
/////////////////////////////////////////////////////////////////// 
#ifndef TRIGJETUTILS_IJKDTRIGHELPERTOOL_H
#define TRIGJETUTILS_IJKDTRIGHELPERTOOL_H 1


// FrameWork includes
#include "GaudiKernel/IAlgTool.h"

class StoreGateSvc;
namespace HLT { class Navigation; }

static const InterfaceID IID_IJKDTrigHelperTool("IJKDTrigHelperTool", 1, 0);

class IJKDTrigHelperTool
  : virtual public ::IAlgTool
{ 

 public: 

  virtual ~IJKDTrigHelperTool() {};

  static const InterfaceID& interfaceID();
  /////////////////////////////////////////////////////////////////// 

  virtual StatusCode setupJetKeyDescriptor(StoreGateSvc*, HLT::Navigation*) = 0;

 protected: 

}; 

inline const InterfaceID& IJKDTrigHelperTool::interfaceID() 
{ 
   return IID_IJKDTrigHelperTool; 
}


#endif //> !TRIGJETUTILS_IJKDTRIGHELPERTOOL_H
