/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITruthSvc.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef ISF_INTERFACES_ITRUTHSVC_H
#define ISF_INTERFACES_ITRUTHSVC_H 1

// framework includes
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"

// DetectorDescription
#include "AtlasDetDescr/AtlasRegion.h"

// forward declarations
class G4Step;

namespace ISF {

  class ITruthIncident;
    
  /** @ class ITruthSvc
  
      Interface for the central truth record service to be used by
      the different simulation services.
  
      @ author Andreas.Salzburger -at- cern.ch , Elmar.Ritsch -at- cern.ch
     */
    class ITruthSvc : virtual public IInterface { 

      public: 
        /// Creates the InterfaceID and interfaceID() method
        DeclareInterfaceID(ITruthSvc, 1, 0);
      
        /** virtual desctructor */
        virtual ~ITruthSvc() { }

        /** Register a truth incident */
        virtual void registerTruthIncident( ITruthIncident& truthincident) = 0;
        
        /** Initialize the Truth Svc at the beginning of each event */
        virtual StatusCode initializeTruthCollection() = 0;
        
        /** Finalize the Truth Svc at the end of each event*/
        virtual StatusCode releaseEvent() = 0;
  }; 
}

#endif //> !ISF_INTERFACES_ITRUTHSVC_H

