/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <JetAnalysisAlgorithms/JetSelectionAlg.h>

#include <SelectionHelpers/SelectionHelpers.h>

//
// method implementations
//

namespace CP
{
  JetSelectionAlg ::
  JetSelectionAlg (const std::string& name, 
                     ISvcLocator* pSvcLocator)
    : AnaAlgorithm (name, pSvcLocator)
    , m_selectionTool ("", this)
  {
    declareProperty ("selectionTool", m_selectionTool, "the selection tool we apply");
    declareProperty ("m_selectionDecoration", m_selectionDecoration, "the decoration for the jet selection");
  }



  StatusCode JetSelectionAlg ::
  initialize ()
  {
    ANA_CHECK (m_selectionTool.retrieve());
    m_systematicsList.addHandle (m_jetHandle);
    ANA_CHECK (m_systematicsList.initialize());

    if (m_selectionDecoration.empty())
    {
      ANA_MSG_ERROR ("no selection decoration name set");
      return StatusCode::FAILURE;
    }
    m_selectionAccessor = std::make_unique<SG::AuxElement::Accessor<SelectionType> > (m_selectionDecoration);

    return StatusCode::SUCCESS;
  }



  StatusCode JetSelectionAlg ::
  execute ()
  {
    return m_systematicsList.foreach ([&] (const CP::SystematicSet& sys) -> StatusCode {
        xAOD::JetContainer *jets = nullptr;
        ANA_CHECK (m_jetHandle.getCopy (jets, sys));
        for (xAOD::Jet *jet : *jets)
        {
          (*m_selectionAccessor) (*jet)
            = selectionFromBool(m_selectionTool->keep(*jet));
        }
        return StatusCode::SUCCESS;
      });
  }
}
