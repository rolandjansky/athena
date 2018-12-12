/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FASTCALOSIMSVC_H
#define ISF_FASTCALOSIMSVC_H 1

// STL includes
#include <string>
// #include <set>

// Gaudi
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

// ISF includes
#include "ISF_Interfaces/BaseSimulationSvc.h"
#include "ISF_Interfaces/ISimulatorTool.h"

namespace ISF {

  /** @class FastCaloSimSvc
      @author Michael.Duehrssen -at- cern.ch
  */
  class FastCaloSimSvc : public BaseSimulationSvc {
  public:

    //** Constructor with parameters */
    FastCaloSimSvc( const std::string& name, ISvcLocator* pSvcLocator );

    /** Destructor */
    virtual ~FastCaloSimSvc();

    /** Athena algorithm's interface methods */
    virtual StatusCode  initialize() override;

    /** Simulation Call */
    virtual StatusCode simulate(const ISFParticle& isp, McEventCollection* mcEventCollection) override;

    /** Setup Event chain - in case of a begin-of event action is needed */
    virtual StatusCode setupEvent() override;

     /** Release Event chain - in case of an end-of event action is needed */
    virtual StatusCode releaseEvent() override;

  private:
    /** Default constructor */
    FastCaloSimSvc();

    PublicToolHandle<ISimulatorTool> m_simulatorTool{this, "SimulatorTool", "", ""};

  };
}

#endif //> !ISF_FASTCALOSIMSVC_H
