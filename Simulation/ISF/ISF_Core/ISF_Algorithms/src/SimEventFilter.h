/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_ALGORITHMS_SIMEVENTFILTER_H
#define ISF_ALGORITHMS_SIMEVENTFILTER_H 1

// STL includes
#include <string>

// FrameWork includes
#include "StoreGate/ReadHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthFilterAlgorithm.h"

// ISF_Interfaces includes
#include "ISF_HepMC_Interfaces/IGenParticleFilter.h"

// McEventCollection
#include "GeneratorObjects/McEventCollection.h"

namespace ISF {

  class SimEventFilter : public AthFilterAlgorithm {

  public:
    /** Constructor with parameters */
    SimEventFilter( const std::string& name, ISvcLocator* pSvcLocator );

    /** Destructor */
    virtual ~SimEventFilter() = default;

    /** Athena algorithm's interface method initialize() */
    virtual StatusCode  initialize() override final; /** Athena algorithm's interface method execute() */
    virtual StatusCode  execute() override final;

    /** Athena algorithm's interface method finalize() */
    virtual StatusCode  finalize() override final;

  private:
    bool passesFilters(const HepMC::GenParticle& part, ToolHandleArray<IGenParticleFilter>& filters) const;

    /** Input truth collections */
    SG::ReadHandle<McEventCollection>    m_inputHardScatterEvgen; //!< input hard scatter collection

    /** Filter passes if a difference between the decision of m_genParticleOldFilters and m_genParticleNewFilters is found.
        m_genParticleCommonFilters is applied before to select relevant particles.
        If only m_genParticleCommonFilters is specified, filter passes if any particle passes this one
    **/
    ToolHandleArray<IGenParticleFilter>  m_genParticleCommonFilters;    //!< HepMC::GenParticle filters for both selections
    ToolHandleArray<IGenParticleFilter>  m_genParticleOldFilters;       //!< HepMC::GenParticle filters for old selection
    ToolHandleArray<IGenParticleFilter>  m_genParticleNewFilters;       //!< HepMC::GenParticle filters for new selection

    bool                                 m_invertfilter = false; //!< invert filter decision at the end

    int                                  m_pass = 0; //< number of events passing the filter
    int                                  m_total = 0 ; //< number of total events seen
  };
}

#endif //> !ISF_ALGORITHMS_SIMEVENTFILTER_H
