/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <SelectionHelpers/OutOfValidityHelper.h>

#include <AsgMessaging/MessageCheck.h>
#include <AsgTools/StatusCode.h>
#include <PATInterfaces/CorrectionCode.h>

//
// method implementations
//

namespace CP
{
  ::StatusCode OutOfValidityHelper ::
  initialize ()
  {
    if (!m_decorationName.empty())
    {
      ANA_CHECK (makeSelectionAccessor (m_decorationName, m_accessor));
    }
      
    m_isInitialized = true;
    return StatusCode::SUCCESS;
  }


  ::StatusCode OutOfValidityHelper ::
  check (xAOD::IParticle& particle,
         const CP::CorrectionCode& code,
         const char *context) const
  {
    assert (m_isInitialized);

    switch (code)
    {
    case CP::CorrectionCode::Ok:
      if (m_accessor) m_accessor->setBool (particle, true);
      return StatusCode::SUCCESS;
    case CP::CorrectionCode::Error:
      return StatusCode::FAILURE;
    case CP::CorrectionCode::OutOfValidityRange:
      if (m_accessor) m_accessor->setBool (particle, false);
      switch (OutOfValidityAction (m_action))
      {
      case OutOfValidityAction::ABORT:
        ANA_MSG_ERROR ("encountered OutOfValidity: " << context);
        return StatusCode::FAILURE;
      case OutOfValidityAction::WARNING:
        ANA_MSG_WARNING ("encountered OutOfValidity: " << context);
        return StatusCode::SUCCESS;
      case OutOfValidityAction::SILENT:
        return StatusCode::SUCCESS;
      }
    }
    ANA_MSG_ERROR (__FILE__ << ":" << __LINE__ << ": invalid enum value encountered " << code << " " << int (m_action));
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
