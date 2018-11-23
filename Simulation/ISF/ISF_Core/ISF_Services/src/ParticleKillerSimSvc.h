/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_PARTICLEKILLERSIMSVC_H
#define ISF_PARTICLEKILLERSIMSVC_H 1

// STL includes
#include <string>

// ISF includes
#include "ISF_Interfaces/BaseSimulationSvc.h"
#include "ISF_Interfaces/ISimulatorTool.h"
#include "ISF_Event/ISFParticleContainer.h"

namespace ISF {

  /** @class ParticleKillerSimSvc

      This ISF SimulationService does literally nothing with the particles
      handed over in the simulate(...) call. Thus those particles will be
      removed from further simulation in ISF.

      @author Elmar.Ritsch -at- cern.ch
  */
  class ParticleKillerSimSvc : public ISF::BaseSimulationSvc {

  public:
    //** Constructor with parameters */
    ParticleKillerSimSvc( const std::string& name, ISvcLocator* pSvcLocator );

    /** Destructor */
    virtual ~ParticleKillerSimSvc();

    /** Athena algorithm's interface methods */
    virtual StatusCode  initialize() override;

    /** Simulation Call  */
    virtual StatusCode simulate(const ISF::ISFParticle& isp) override;

  private:
    PublicToolHandle<ISF::ISimulatorTool> m_simulatorTool{this, "SimulatorTool", "ISF__ParticleKillerSimTool", ""};

  };
}


#endif //> !ISF_PARTICLEKILLERSIMSV_H
