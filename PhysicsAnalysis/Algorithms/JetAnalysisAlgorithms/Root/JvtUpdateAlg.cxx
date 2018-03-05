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
  }



  StatusCode JvtUpdateAlg ::
  initialize ()
  {
    ANA_CHECK (m_jvtTool.retrieve());
    m_systematicsList.addHandle (m_jetHandle);
    ANA_CHECK (m_systematicsList.initialize());
    return StatusCode::SUCCESS;
  }



  StatusCode JvtUpdateAlg ::
  execute ()
  {
    static const SG::AuxElement::Accessor<float> jvtAccessor ("Jvt");

    return m_systematicsList.foreach ([&] (const CP::SystematicSet& sys) -> StatusCode {
        xAOD::JetContainer *jets = nullptr;
        ANA_CHECK (m_jetHandle.getCopy (jets, sys));
        for (xAOD::Jet *jet : *jets)
        {
          // manually update jvt decoration using the tool
          const float jvt = m_jvtTool->updateJvt (*jet);
          jvtAccessor (*jet) = jvt;
        }
        return StatusCode::SUCCESS;
      });
  }
}
