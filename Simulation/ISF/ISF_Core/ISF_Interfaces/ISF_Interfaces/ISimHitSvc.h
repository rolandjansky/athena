/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ISimHitSvc.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef ISF_INTERFACES_ISIMHITSVC_H
#define ISF_INTERFACES_ISIMHITSVC_H 1

// Include Files
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

class SiHit;
class TRTHit;
class TRTUncompressedHit;
class IdentifierHash;

namespace ISF {
      
  /** @ class ISimHitSvc
  
      Interface for the central simulaiton hit service to be used
      by the different simulation flavors.
      
      @TODO add Calo, MS, et al.
  
      @ author Andreas.Salzburger -at- cern.ch
     */
  class ISimHitSvc : virtual public IInterface { 

      /////////////////////////////////////////////////////////////////// 
      // Public methods: 
      /////////////////////////////////////////////////////////////////// 
    public: 

      /// Creates the InterfaceID and interfaceID() method
      DeclareInterfaceID(ISimHitSvc, 1, 0);

      /** Initialize event chain */
      virtual StatusCode initializeEvent() = 0;                             
      
      /** Insert a SiHit - used for Pixels, SCT */
      virtual void insert(const SiHit& siHit) = 0;

      /* Insert a TRT Hit */
      virtual void insert(const TRTHit& trtHit) = 0;

      /* Insert a TRTUncompressed Hit */
      virtual void insert(const TRTUncompressedHit& trtHit) = 0;
      
      /** Release Event chain - @TODO: evoke callback chain ? */
      virtual StatusCode releaseEvent() = 0;                             
            
  }; 
}

#endif //> !ISF_INTERFACES_ISIMHITSVC_H
