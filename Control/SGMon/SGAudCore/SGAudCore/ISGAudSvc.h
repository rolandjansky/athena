///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ISGAudSvc.h 
// Header file for class ISGAudSvc
// Author: Ilija Vukotic<ivukotic@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef SGAUDCORE_ISGAUDSVC_H 
#define SGAUDCORE_ISGAUDSVC_H 

// STL includes

// FrameWork includes
#include "GaudiKernel/IService.h"
#include "GaudiKernel/ClassID.h"

class ISGAudSvc : virtual public IService
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Destructor: 
   */
  virtual ~ISGAudSvc();
 
  virtual bool SGSetCurrentAlg(){return true;};//std::string CurrAlg

  virtual void SGAudit(const std::string& /*key*/, const CLID& /*id*/, 
		       const int& /*fnc*/, const int& /*store_id*/) {};

  virtual void setFakeCurrentAlg(const std::string&){}//For custom increased granularity
  virtual void clearFakeCurrentAlg(){}
 
  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////
  static const InterfaceID& interfaceID();

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 
inline const InterfaceID& ISGAudSvc::interfaceID() 
{ 
  static const InterfaceID IID_ISGAudSvc("ISGAudSvc", 1, 0);
  return IID_ISGAudSvc; 
}

#endif 
