/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <SystematicsHandles/OutOfValidityHelper.h>

#include <AsgTools/MessageCheck.h>
#include <AsgTools/StatusCode.h>
#include <PATInterfaces/CorrectionCode.h>
#include <SystematicsHandles/SelectionHelpers.h>

//
// method implementations
//

namespace EL
{
  ::StatusCode OutOfValidityHelper ::
  initialize ()
  {
    if (m_decoName.empty())
    {
      ANA_MSG_ERROR ("failed to set decoration name for OutOfValidityRange selection");
      return StatusCode::FAILURE;
    }
    if (m_decoName == "SUCCESS")
    {
      m_action = Action::SUCCESS;
    } else if (m_decoName == "FAILURE")
    {
      m_action = Action::FAILURE;
    } else if (m_decoName.find ("WARNING") == 0)
    {
      // warnings can be combined with adding a decoration, or be used
      // on their own
      m_action = Action::WARNING;
      if (m_decoName != "WARNING")
      {
        m_accessor = std::make_unique<const SG::AuxElement::Accessor<SelectionType> >
          (m_decoName.substr (8));
      }
    } else
    {
      m_action = Action::SUCCESS;
      m_accessor = std::make_unique<const SG::AuxElement::Accessor<SelectionType> >
        (m_decoName);
    }
    return StatusCode::SUCCESS;
  }


  ::StatusCode OutOfValidityHelper ::
  check (xAOD::IParticle& particle,
         const CP::CorrectionCode& code,
         const char *context) const
  {
    assert (m_action != Action::UNDEFINED);

    switch (code)
    {
    case CP::CorrectionCode::Ok:
      if (m_accessor) (*m_accessor) (particle) = selectionAccept();
      return StatusCode::SUCCESS;
    case CP::CorrectionCode::Error:
      return StatusCode::FAILURE;
    case CP::CorrectionCode::OutOfValidityRange:
      if (m_accessor) (*m_accessor) (particle) = selectionReject();
      switch (m_action)
      {
      case Action::UNDEFINED:
        ANA_MSG_ERROR ("failed to initialize OutOfValidityHelper: " << context);
        return StatusCode::FAILURE;
      case Action::FAILURE:
        ANA_MSG_ERROR ("encountered OutOfValidity: " << context);
        return StatusCode::FAILURE;
      case Action::SUCCESS:
        return StatusCode::SUCCESS;
      case Action::WARNING:
        ANA_MSG_WARNING ("encountered OutOfValidity: " << context);
        return StatusCode::SUCCESS;
      }
    }
    ANA_MSG_ERROR ("invalid enum value encountered " << __FILE__ << ":" << __LINE__);
    return StatusCode::FAILURE;
  }



  MsgStream& OutOfValidityHelper ::
  msg (MSG::Level lvl) const
  {
    assert (m_msg != nullptr);
    *m_msg << lvl;
    return *m_msg;
  }
}
