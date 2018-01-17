/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ISimulationSelector.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_INTERFACES_ISIMULATIONSELECTOR_H
#define ISF_INTERFACES_ISIMULATIONSELECTOR_H 1

// Gaudi & Athena basics
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"  // for ToolHandleArray

// ISF includes
#include "ISF_Interfaces/ISimulationSvc.h"
#include "ISF_Interfaces/SimulationFlavor.h"



namespace ISF {

class ISFParticle;

/**
 @class ISimulationSelector

 @author Elmar.Ritsch -at- cern.ch , Peter.Sherwood -at- cern.ch
 */

class ISimulationSelector : virtual public IAlgTool {
  public:
    /** virtual destructor */
    virtual ~ISimulationSelector() {};

    /// Creates the InterfaceID and interfaceID() method
    DeclareInterfaceID(ISimulationSelector, 1, 0);

    /** return a handle on the simulator */
    virtual ServiceHandle<ISimulationSvc>* simulator() = 0;

    /** return if is this a static or dynamic SimulationSelector
        (is used by fully dynamic partilce routers) */
    virtual bool isDynamic() = 0;

    /** return the simulation service ID */
    virtual SimSvcID simSvcID() = 0;

    /** return the simulation flavor */
    virtual ISF::SimulationFlavor simFlavor() = 0;

    /** initialize Selector */
    virtual void initializeSelector() = 0;

    /** called at the beginning of each athena event
        (can be used for eg. resetting dynamic selectors) */
    virtual void beginEvent() = 0;

    /** called at the end of each athena event
        (can be used for eg. resetting dynamic selectors) */
    virtual void endEvent() = 0;

    /** update internal event representation */
    virtual void update(const ISFParticle& ) = 0;

    /** make the routing decision */
    virtual bool selfSelect(const ISFParticle& particle) = 0;

    /** check whether given particle passes all cuts -> will be used for routing decision*/
    virtual bool passSelectorCuts(const ISFParticle& particle) const = 0;
};

/** typedef for better readable code */
typedef ToolHandleArray<ISimulationSelector>   SimSelectorToolArray;

}  // end of `ISF` namespace

#endif // ISF_INTERFACES_ISIMULATIONSELECTOR_H
