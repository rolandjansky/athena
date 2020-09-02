/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <MetAnalysisAlgorithms/MetSystematicsAlg.h>

#include <xAODMissingET/MissingETAuxContainer.h>

//
// method implementations
//

namespace CP
{
  MetSystematicsAlg ::
  MetSystematicsAlg (const std::string& name, 
                     ISvcLocator* pSvcLocator)
    : AnaAlgorithm (name, pSvcLocator)
    , m_systematicsTool ("met::METSystematicsTool", this)
  {
    declareProperty ("systematicsTool", m_systematicsTool, "the systematics tool we apply");
    declareProperty ("softTerm", m_softTerm, "the key for the soft term");
  }



  StatusCode MetSystematicsAlg ::
  initialize ()
  {
    ANA_CHECK (m_systematicsTool.retrieve());
    m_systematicsList.addHandle (m_metHandle);
    ANA_CHECK (m_systematicsList.addAffectingSystematics (m_systematicsTool->affectingSystematics()));
    ANA_CHECK (m_systematicsList.initialize());
    return StatusCode::SUCCESS;
  }



  StatusCode MetSystematicsAlg ::
  execute ()
  {
    return m_systematicsList.foreach ([&] (const CP::SystematicSet& sys) -> StatusCode {
        ANA_CHECK (m_systematicsTool->applySystematicVariation (sys));

        xAOD::MissingETContainer *met {};
        ANA_CHECK (m_metHandle.getCopy (met, sys));

        xAOD::MissingET *softTerm = (*met)[m_softTerm];
        if (softTerm == nullptr)
        {
          ANA_MSG_ERROR ("failed to find MET soft-term \"" << m_softTerm << "\"");
          return StatusCode::FAILURE;
        }

        // This returns a `CorrectionCode`, so in principle this could
        // return an `OutOfValidity` result, but I have no idea what
        // that would mean or how to handle it, so I'm implicitly
        // converting it into a `FAILURE` instead.
        ANA_CHECK (m_systematicsTool->applyCorrection (*softTerm));

        return StatusCode::SUCCESS;
      });
  }
}
