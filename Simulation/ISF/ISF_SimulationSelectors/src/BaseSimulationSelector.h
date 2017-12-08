/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_SIMULATIONSELECTORS_BASESIMULATIONSELECTOR_H
#define ISF_SIMULATIONSELECTORS_BASESIMULATIONSELECTOR_H 1

// ISF includes
#include "ISF_Interfaces/ISimulationSelector.h"
#include "ISF_Interfaces/ISimulationSvc.h"
#include "ISF_Interfaces/SimulationFlavor.h"


// Gaudi & Athena basics
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"


namespace ISF {

class ISFParticle;

/**
 @class BaseSimulationSelector

 @author Elmar.Ritsch -at- cern.ch , Peter.Sherwood -at- cern.ch
 */

class BaseSimulationSelector : public extends<AthAlgTool, ISimulationSelector> {
  public:
    /** Constructor with parameters */
    BaseSimulationSelector( const std::string& t, const std::string& n, const IInterface* p );

    /** virtual destructor */
    virtual ~BaseSimulationSelector();

    /** Gaudi sysInitialize() method */
    virtual StatusCode sysInitialize() override;

    /** return a handle on the simulator */
    virtual ServiceHandle<ISimulationSvc>* simulator() override;

    /** return if is this a static or dynamic SimulationSelector
        (is used by fully dynamic particle routers) */
    virtual bool isDynamic() override;

    /** return the simulation service ID */
    virtual SimSvcID simSvcID() override;

    /** return the simulation flavor */
    virtual ISF::SimulationFlavor simFlavor() override;

    /** initialize Selector */
    virtual void initializeSelector() override;

    /** called at the beginning of each athena event
        (can be used for eg. resetting dynamic selectors) */
    virtual void beginEvent() override;

    /** called at the end of each athena event
        (can be used for eg. resetting dynamic selectors) */
    virtual void endEvent() override;

    /** update internal event representation */
    virtual void update(const ISFParticle& ) override;

    /** make the routing decision */
    virtual bool selfSelect(const ISFParticle& particle) override;

  private:
    ServiceHandle<ISimulationSvc>       m_simulator;  //!< simulation service assigned to a single advisor
    bool                                m_isDynamic;  //!< this selector is either dynamic or static
    bool                                m_invertCuts; //!< invert the result given by passesCuts(..) method
    ISF::SimulationFlavor               m_simflavor;  //!< simulation flavor
};


} // end of namespace


#endif // ISF_SIMULATIONSELECTORS_BASESIMULATIONSELECTOR_H
