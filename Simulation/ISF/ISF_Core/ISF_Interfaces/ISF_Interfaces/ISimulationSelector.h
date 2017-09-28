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
#include "AthenaBaseComps/AthAlgTool.h"

// ISF includes
#include "ISF_Interfaces/ISimulationSvc.h"
#include "ISF_Interfaces/SimulationFlavor.h"


 
namespace ISF {

  class ISFParticle;

  /**
   @class ISimulationSelector
       
   @author Elmar.Ritsch -at- cern.ch , Peter.Sherwood -at- cern.ch
   */
     
  class ISimulationSelector : virtual public IAlgTool, public AthAlgTool {
    public:
      /** Constructor with parameters */
      ISimulationSelector( const std::string& t, const std::string& n, const IInterface* p );

      /** Gaudi sysInitialize() method */
      StatusCode sysInitialize();

      /** virtual destructor */
      virtual ~ISimulationSelector();

      /// Creates the InterfaceID and interfaceID() method
      DeclareInterfaceID(ISimulationSelector, 1, 0);

      /** return a handle on the simulator */
      inline ServiceHandle<ISimulationSvc>* simulator();

      /** return if is this a static or dynamic SimulationSelector
          (is used by fully dynamic partilce routers) */
      inline bool isDynamic();

      /** return the simulation service ID */
      inline SimSvcID simSvcID();

      /** return the simulation flavor */
      inline ISF::SimulationFlavor simFlavor();
      
      /** initialize Selector */
      inline virtual void initializeSelector();

      /** called at the beginning of each athena event
          (can be used for eg. resetting dynamic selectors) */
      inline virtual void beginEvent();

      /** called at the end of each athena event
          (can be used for eg. resetting dynamic selectors) */
      inline virtual void endEvent();

      /** update internal event representation */
      inline virtual void update(const ISFParticle& );

      /** make the routing decision */
      bool selfSelect(const ISFParticle& particle);

      /** check whether given particle passes all cuts -> will be used for routing decision*/
      virtual bool passSelectorCuts(const ISFParticle& particle) const = 0;

    private:
      ServiceHandle<ISimulationSvc>       m_simulator;  //!< simulation service assigned to a single advisor
      bool                                m_isDynamic;  //!< this selector is either dynamic or static
      bool                                m_invertCuts; //!< invert the result given by passesCuts(..) method
      ISF::SimulationFlavor               m_simflavor;  //!< simulation flavor
  };


  //
  // inline methods
  //

  /** return a handle on the simulator */
  ServiceHandle<ISimulationSvc>* ISF::ISimulationSelector::simulator() {
    return &m_simulator;
  }

  /** return if is this a static or dynamic SimulationSelector
      (is used by fully dynamic partilce routers) */
  bool ISF::ISimulationSelector::isDynamic() {
    return m_isDynamic;
  }

  /** return the simulation service ID */
  SimSvcID ISF::ISimulationSelector::simSvcID() {
    return m_simulator->simSvcID();
  }

  /** return the simulation service ID */
  ISF::SimulationFlavor ISF::ISimulationSelector::simFlavor() {
    return m_simflavor;
  }

  /** initialize Selector */
  void ISF::ISimulationSelector::initializeSelector() { }

  /** called at the beginning of each athena event
      (can be used for eg. resetting dynamic selectors) */
  void ISF::ISimulationSelector::beginEvent() { }

  /** called at the end of each athena event
      (can be used for eg. resetting dynamic selectors) */
  void ISF::ISimulationSelector::endEvent() { }

  /** update internal event representation */
  void ISF::ISimulationSelector::update(const ISFParticle& ) { }

  /** typedef for better readable code */
  typedef ToolHandleArray<ISimulationSelector>   SimSelectorToolArray;

} // end of namespace


#endif // ISF_INTERFACES_ISIMULATIONSELECTOR_H
