///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthService.h 
// Header file for class AthService
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENABASECOMPS_ATHSERVICE_H 
#define ATHENABASECOMPS_ATHSERVICE_H 1

// STL includes
#include <string>

// Framework includes
#include "GaudiKernel/Service.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "AthenaBaseComps/AthCheckMacros.h"

#include "AthenaBaseComps/AthCommonMsg.h"

#include "StoreGate/VarHandleProperty.h"

// Forward declaration
class ISvcLocator;
template <class TYPE> class SvcFactory;

class AthService : 
  public AthCommonMsg<Service>
{ 
 public: 

  using AthCommonMsg<Service>::AthCommonMsg;

private:

  AthService();
  AthService (const AthService&);
  AthService& operator= (const AthService&);

}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> ATHENABASECOMPS_ATHSERVICE_H
