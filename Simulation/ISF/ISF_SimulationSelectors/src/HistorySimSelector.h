/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// HistorySimSelector.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_TOOLS_PASSPORTSIMSELECTOR_H
#define ISF_TOOLS_PASSPORTSIMSELECTOR_H 1

// Framework includes
#include "GaudiKernel/ServiceHandle.h"

// ISF includes
#include "BaseSimulationSelector.h"
#include "ISF_Event/SimSvcID.h"

namespace ISF
{

  // forward declarations
  class ISimulationSvc;


  /** @class HistorySimSelector

      Simplistic simulation selector using the particle's history

      @author Elmar.Ritsch -at- cern.ch
  */
  class HistorySimSelector : public BaseSimulationSelector
  {

  public:
    /** Constructor with parameters */
    HistorySimSelector( const std::string& t, const std::string& n, const IInterface* p );

    /** Destructor */
    ~HistorySimSelector();

    // Athena algtool's Hooks
    virtual StatusCode  initialize() override final;
    virtual StatusCode  finalize() override final;

    /** called at the beginning of each athena event */
    virtual void beginEvent() override final;

    /** check whether given particle passes all cuts -> will be used for routing decision*/
    virtual bool passSelectorCuts(const ISFParticle& particle) const override final;

  private:
    /** will check given particles if they were previously simulated by
        the SimulationSvc/ID defined in here */
    ServiceHandle<ISF::ISimulationSvc>          m_prevSimSvcHandle; //!< SimSvc handle
    SimSvcID                                    m_prevSimSvcID;     //!< same SimSvc ID

    bool                                        m_checkSameGeoID;   //!< check that geoID did not change
  };

}

#endif //> !ISF_TOOLS_PASSPORTSIMSELECTOR_H
