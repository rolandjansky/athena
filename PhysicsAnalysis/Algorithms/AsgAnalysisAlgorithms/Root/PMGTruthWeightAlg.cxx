/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Tadej Novak


//
// includes
//

#include <AsgAnalysisAlgorithms/PMGTruthWeightAlg.h>

//
// method implementations
//

namespace CP
{
  PMGTruthWeightAlg ::
  PMGTruthWeightAlg (const std::string& name, 
                     ISvcLocator* pSvcLocator)
    : AnaAlgorithm (name, pSvcLocator)
    , m_truthWeightTool ("PMGTools::PMGTruthWeightTool", this)
  {
    declareProperty ("truthWeightTool", m_truthWeightTool, "the truth weight tool");
  }



  StatusCode PMGTruthWeightAlg ::
  initialize ()
  {
    if (m_decoration.empty())
    {
      ANA_MSG_ERROR ("no decoration name set");
      return StatusCode::FAILURE;
    }

    ANA_CHECK (m_truthWeightTool.retrieve());
    ANA_CHECK (m_systematicsList.addAffectingSystematics (m_truthWeightTool->affectingSystematics()));
    m_systematicsList.addHandle (m_eventInfoHandle);
    ANA_CHECK (m_systematicsList.initialize());

    return StatusCode::SUCCESS;
  }


  StatusCode PMGTruthWeightAlg ::
  execute ()
  {
    ANA_CHECK (m_decoration.preExecute (m_systematicsList));

    return m_systematicsList.foreach ([&] (const CP::SystematicSet& sys) -> StatusCode
    {
      ANA_CHECK (m_truthWeightTool->applySystematicVariation (sys));
      xAOD::EventInfo *eventInfo = nullptr;
      ANA_CHECK (m_eventInfoHandle.getCopy (eventInfo, sys));

      m_decoration.set (*eventInfo, m_truthWeightTool->getSysWeight (), sys);

      return StatusCode::SUCCESS;
    });
  }
}
