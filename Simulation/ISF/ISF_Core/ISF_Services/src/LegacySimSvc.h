/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_SERVICES_ISF_LEGACYSIMSVC_H
#define ISF_SERVICES_ISF_LEGACYSIMSVC_H 1

// STL includes
#include <string>

// Gaudi
#include "GaudiKernel/ToolHandle.h"

// ISF includes
#include "ISF_Interfaces/BaseSimulationSvc.h"
#include "ISF_Interfaces/ISimulatorTool.h"

namespace ISF {

  /** @class LegacySimSvc
  */
  class LegacySimSvc : public BaseSimulationSvc {
  public:

    //** Constructor with parameters */
    LegacySimSvc( const std::string& name, ISvcLocator* pSvcLocator );

    /** Destructor */
    virtual ~LegacySimSvc() = default;

    /** Athena algorithm's interface methods */
    virtual StatusCode  initialize() override;

    /** Simulation Call */
    virtual StatusCode simulate(const ISFParticle& isp, McEventCollection* mcEventCollection) override;

    /** Setup Event chain - in case of a begin-of event action is needed */
    virtual StatusCode setupEvent() override;

     /** Release Event chain - in case of an end-of event action is needed */
    virtual StatusCode releaseEvent() override;

  private:
    PublicToolHandle<ISimulatorTool> m_simulatorTool{this, "SimulatorTool", "", ""};

  };
}

#endif //> !ISF_SERVICES_ISF_LEGACYSIMSVC_H
