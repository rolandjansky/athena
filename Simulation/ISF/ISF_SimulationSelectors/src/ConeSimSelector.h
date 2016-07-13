/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ConeSimSelector.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_TOOLS_CONESIMSELECTOR_H
#define ISF_TOOLS_CONESIMSELECTOR_H 1

// ISF includes
#include "ISF_Event/ConeParticleCuts.h"
#include "ISF_Interfaces/ISimulationSelector.h"
#include "ISF_Event/HepMCHelper.h"

// Framework
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

namespace ISF
{

  // forward delcarations
  class ITrkExtrapolator;

  /** @class ConeSimSelector

      This SimulationSelector implementation registers cones around particles that
      are given to the updateInsideSubDet(..) or the updateOutsideSubDet(..) method
      and pass certain cuts (pT, pdg, HepMC ancestor). PassSelectorCuts(..) determines
      whether the given particle is inside such a cone or not.

      @author Andreas.Salzburger -at- cern.ch , Elmar.Ritsch -at- cern.ch
  */
  class ConeSimSelector : public ISimulationSelector, public ConeParticleCuts
  {

  public:
    /** Constructor with parameters */
    ConeSimSelector( const std::string& t, const std::string& n, const IInterface* p );

    /** Destructor */
    ~ConeSimSelector();

    /** Athena AlgTool initialize */
    StatusCode  initialize() override final;

    /** Athena AlgTool finalize */
    StatusCode  finalize() override final;

    /** called at the beginning of each event (used for resetting dynamic selectors) */
    virtual void beginEvent() override final;

    /** called at the beginning of each event (used for resetting dynamic selectors) */
    virtual void endEvent() override final;

    /** update internal event representation (create cones in our case)*/
    virtual void update(const ISFParticle& p ) override final;

    /** check whether given particle is within any of the registered cones
        -> will be used for routing decision*/
    virtual bool passSelectorCuts(const ISFParticle& particle) const override final;

  private:
    std::vector<int>                          m_absPDGVector;  //!< abs(PDG) for particles to create cones around
    /** ISFParticle has to have a relative which is in this list to create a cone*/
    bool                                      m_checkRelatives;//!< on/off for checking relatives
    std::vector<int>                          m_relativesVec;  //!< Python property
    std::set<int>                             m_relatives;     //!< used during runtime (faster)

    /** ISFParticle relation to the given pdg codes */
    int                                       m_relationProp;  //!< Python property
    HepMC::IteratorRange                      m_relation;      //!< HepMC

    /** Track extrapolation to estimate impact point of particle on next sub-detector*/
    bool                                      m_extrapolateToCalo; //!< enable/disable
    ToolHandle<ISF::ITrkExtrapolator>         m_extrapolator;      //!< extrapolator tool
  };

}

#endif //> !ISF_TOOLS_CONESIMSELECTOR_H
