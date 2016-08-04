/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// FastCaloSimSvcV2.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_FASTCALOSIMSVCV2_H
#define ISF_FASTCALOSIMSVCV2_H 1

// ISF includes
#include "ISF_Interfaces/BaseSimulationSvc.h"

// FastCaloSim includes
#include "ISF_FastCaloSimEvent/TFCSPCAEnergyParametrization.h"

namespace ISF {
  /** @class FastCaloSimSvcV2
  
      @author Elmar.Ritsch -at- cern.ch, Geraldine.Conti -at- cern.ch, Flavia.Dias -at- cern.ch
  */
  class FastCaloSimSvcV2 : public BaseSimulationSvc { 
    public: 
      /** Constructor with parameters */
      FastCaloSimSvcV2(const std::string& name, ISvcLocator* pSvcLocator);
      
      /** Destructor */
      virtual ~FastCaloSimSvcV2(); 
      
      /** Athena algorithm's interface methods */
      StatusCode  initialize();
      StatusCode  finalize();

      /** Simulation Call */
      StatusCode simulate(const ISFParticle& isp);
                                                                 
      /** Setup Event chain - in case of a begin-of event action is needed */
      StatusCode setupEvent();

      /** Release Event chain - in case of an end-of event action is needed */
      StatusCode releaseEvent();

    private:
      /** Dummy energy parametrization */
      ::TFCSPCAEnergyParametrization    m_epara;
  }; 
}

#endif //> !ISF_FASTCALOSIMSVCV2_H
