///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IOffloadSvc.h 
// Header file for class IOffloadSvc
// Author: Sami Kama <sami.kama@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef OFFLOADSERVICE_IOFFLOADSVC_H
#define OFFLOADSERVICE_IOFFLOADSVC_H

/** @class IOffloadSvc
 *  This is the interface to a test service
 */

// STL includes

// FrameWork includes
#include <memory>
#include "GaudiKernel/IService.h"

// forward declaration
namespace APE{
  class BufferContainer;
}

class IOffloadSvc : virtual public IService
{ 
  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Destructor: 
   */
  virtual ~IOffloadSvc();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /// Delivers important informations
  //virtual StatusCode qotd( std::string& quote ) = 0;
  virtual StatusCode sendData(std::unique_ptr<APE::BufferContainer> &buff, int &token, bool requiresResponse=true)=0;
  virtual StatusCode receiveData(std::unique_ptr<APE::BufferContainer> &buff, int token, int timeOut=-1)=0;
  /// identifier for the framework
  static const InterfaceID& interfaceID();

}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 
inline const InterfaceID& IOffloadSvc::interfaceID() 
{ 
  static const InterfaceID IID_IOffloadSvc("IOffloadSvc", 1, 0);
  return IID_IOffloadSvc; 
}

#endif
