/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <MetAnalysisAlgorithms/MetBuilderAlg.h>

#include <METUtilities/METHelpers.h>
#include <xAODMissingET/MissingETAuxContainer.h>

//
// method implementations
//

namespace CP
{
  MetBuilderAlg ::
  MetBuilderAlg (const std::string& name, 
                     ISvcLocator* pSvcLocator)
    : AnaAlgorithm (name, pSvcLocator)
  {
    declareProperty ("finalKey", m_finalKey, "the key for the final met term");
    declareProperty ("softTerm", m_softTerm, "the key for the soft term");
  }



  StatusCode MetBuilderAlg ::
  initialize ()
  {
    m_systematicsList.addHandle (m_metHandle);
    ANA_CHECK (m_systematicsList.initialize());
    return StatusCode::SUCCESS;
  }



  StatusCode MetBuilderAlg ::
  execute ()
  {
    return m_systematicsList.foreach ([&] (const CP::SystematicSet& sys) -> StatusCode {
        xAOD::MissingETContainer *met {};
        ANA_CHECK (m_metHandle.getCopy (met, sys));

        xAOD::MissingET *softTerm = (*met)[m_softTerm];
        if (softTerm == nullptr)
        {
          ANA_MSG_ERROR ("could not find MET soft-term: " << m_softTerm);
          return StatusCode::FAILURE;
        }
        ATH_CHECK (met::buildMETSum (m_finalKey, met, softTerm->source()));

        return StatusCode::SUCCESS;
      });
  }
}
