/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_ALGORITHMS_SIMEVENTFILTER_H
#define ISF_ALGORITHMS_SIMEVENTFILTER_H 1

#ifndef SIMULATIONBASE

// STL includes
#include <string>

// FrameWork includes
#include "StoreGate/ReadHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include <EventBookkeeperTools/FilterReporterParams.h>

// ISF_Interfaces includes
#include "ISF_HepMC_Interfaces/IGenParticleFilter.h"

// McEventCollection
#include "GeneratorObjects/McEventCollection.h"

namespace ISF {

  class SimEventFilter : public AthReentrantAlgorithm {

  public:
    /** Constructor with parameters */
    SimEventFilter( const std::string& name, ISvcLocator* pSvcLocator );

    /** Destructor */
    virtual ~SimEventFilter() = default;

    /** Athena algorithm's interface method initialize() */
    virtual StatusCode  initialize() override final; /** Athena algorithm's interface method execute() */
    virtual StatusCode  execute(const EventContext& ctx) const override final;

    /** Athena algorithm's interface method finalize() */
    virtual StatusCode  finalize() override final;

  private:
    bool passesFilters(HepMC::ConstGenParticlePtr part, const ToolHandleArray<IGenParticleFilter>& filters) const;

    /** Input truth collections */
    SG::ReadHandleKey<McEventCollection> m_inputHardScatterEvgenKey{this, "InputHardScatterCollection", "", "Input Hard Scatter EVGEN collection."}; //!< input hard scatter collection

    /** Filter passes if a difference between the decision of m_genParticleOldFilters and m_genParticleNewFilters is found.
        m_genParticleCommonFilters is applied before to select relevant particles.
        If only m_genParticleCommonFilters is specified, filter passes if any particle passes this one
    **/
    ToolHandleArray<IGenParticleFilter>  m_genParticleCommonFilters{this, "GenParticleCommonFilters", {}, "Tools for filtering out GenParticles for both selections."};    //!< HepMC::GenParticle filters for both selections
    ToolHandleArray<IGenParticleFilter>  m_genParticleOldFilters{this, "GenParticleOldFilters", {}, "Tools for filtering out GenParticles with the old selection."};       //!< HepMC::GenParticle filters for old selection
    ToolHandleArray<IGenParticleFilter>  m_genParticleNewFilters{this, "GenParticleNewFilters", {}, "Tools for filtering out GenParticles with the new selection."};       //!< HepMC::GenParticle filters for new selection

    FilterReporterParams m_filterParams {this, "ISF_SimEventFilter", "Decides whether particles should be resimulated or simply copied."};
    Gaudi::Property<bool>  m_invertfilter{this, "InvertFilter", false, "Invert filter decision."}; //!< invert filter decision at the end
  };
}

#endif // SimEventFilter currently will not compile in the AthSimulation Project
#endif //> !ISF_ALGORITHMS_SIMEVENTFILTER_H
