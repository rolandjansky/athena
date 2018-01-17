/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_INTERFACES_ISIMHITSVC_H
#define ISF_INTERFACES_ISIMHITSVC_H 1

// Include Files
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"

namespace ISF {

  /** @class ISimHitSvc

      Largely obsolete - just maintaining the class until it is safe
      to move the functionality to SimKernel.

      @todo add Calo, MS, et al.

      @author Andreas.Salzburger -at- cern.ch
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

      /** Release Event chain - @todo : evoke callback chain ? */
      virtual StatusCode releaseEvent() = 0;

  };
}

#endif //> !ISF_INTERFACES_ISIMHITSVC_H
