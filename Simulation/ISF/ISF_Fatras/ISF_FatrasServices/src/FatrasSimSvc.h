/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FATRASSIMSVC_H
#define ISF_FATRASSIMSVC_H 1

// STL includes
#include <string>

// FrameWork includes
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

// ISF includes
#include "ISF_Interfaces/BaseSimulationSvc.h"
#include "ISF_Interfaces/ISimulatorTool.h"

namespace iFatras {

  /** @class FatrasSimSvc

      @author Andreas.Salzburger -at- cern.ch
     */
  class FatrasSimSvc : public ISF::BaseSimulationSvc {

    public:
      //** Constructor with parameters */
      FatrasSimSvc( const std::string& name, ISvcLocator* pSvcLocator );

      /** Destructor */
      virtual ~FatrasSimSvc();

      /** Athena algorithm's interface methods */
      virtual StatusCode initialize() override;
      virtual StatusCode  finalize() override;

      /** Simulation Call  */
      virtual StatusCode simulate(const ISF::ISFParticle& isp, McEventCollection* mcEventCollection) override;

      /** Setup Event chain - in case of a begin-of event action is needed */
      virtual StatusCode setupEvent() override;

      /** Release Event chain - in case of an end-of event action is needed */
      virtual StatusCode releaseEvent() override;

    private:
      /** Default constructor */
      FatrasSimSvc();

      PublicToolHandle<ISF::ISimulatorTool> m_simulatorTool{this, "SimulatorTool", "", ""};
  };
}


#endif //> !ISF_FatrasSimSvc_H
