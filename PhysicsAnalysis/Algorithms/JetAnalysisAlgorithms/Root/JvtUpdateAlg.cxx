/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <JetAnalysisAlgorithms/JvtUpdateAlg.h>

//
// method implementations
//

namespace CP
{
  JvtUpdateAlg ::
  JvtUpdateAlg (const std::string& name, 
                ISvcLocator* pSvcLocator)
    : AnaAlgorithm (name, pSvcLocator)
    , m_jvtTool ("", this)
  {
    declareProperty ("jvtTool", m_jvtTool, "the jvt tool we apply");
    declareProperty ("decorationName", m_decorationName, "the decoration name to use");
  }



  StatusCode JvtUpdateAlg ::
  initialize ()
  {
    if (m_decorationName.empty())
    {
      ANA_MSG_ERROR ("decoration name set to empty string, not allowed");
      return StatusCode::FAILURE;
    }
    m_decorationAccessor = std::make_unique
      <SG::AuxElement::Accessor<float> > (m_decorationName);

    ANA_CHECK (m_jvtTool.retrieve());
    m_systematicsList.addHandle (m_jetHandle);
    ANA_CHECK (m_systematicsList.initialize());
    ANA_CHECK (m_preselection.initialize());
    return StatusCode::SUCCESS;
  }



  StatusCode JvtUpdateAlg ::
  execute ()
  {
    return m_systematicsList.foreach ([&] (const CP::SystematicSet& sys) -> StatusCode {
        xAOD::JetContainer *jets = nullptr;
        ANA_CHECK (m_jetHandle.getCopy (jets, sys));
        for (xAOD::Jet *jet : *jets)
        {
          if (m_preselection.getBool (*jet))
          {
            // manually update jvt decoration using the tool
            const float jvt = m_jvtTool->updateJvt (*jet);
            (*m_decorationAccessor) (*jet) = jvt;
          }
        }
        return StatusCode::SUCCESS;
      });
  }
}
