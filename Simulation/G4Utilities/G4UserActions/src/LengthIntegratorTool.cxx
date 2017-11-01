/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "G4UserActions/LengthIntegratorTool.h"

namespace G4UA
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  LengthIntegratorTool::LengthIntegratorTool(const std::string& type,
                                             const std::string& name,
                                             const IInterface* parent)
    : ActionToolBase<LengthIntegrator>(type, name, parent),
      m_hSvc("THistSvc", name)
  {
    declareInterface<IBeginEventActionTool>(this);
    declareInterface<IEndEventActionTool>(this);
    declareInterface<ISteppingActionTool>(this);
    declareProperty("HistoSvc", m_hSvc);
  }

  //---------------------------------------------------------------------------
  // Initialize - temporarily here for debugging
  //---------------------------------------------------------------------------
  StatusCode LengthIntegratorTool::initialize()
  {
    ATH_MSG_DEBUG("initialize");

    ATH_CHECK( m_hSvc.retrieve() );

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Create the action on request
  //---------------------------------------------------------------------------
  std::unique_ptr<LengthIntegrator>
  LengthIntegratorTool::makeAction()
  {
    ATH_MSG_DEBUG("makeAction");
    return CxxUtils::make_unique<LengthIntegrator>( m_hSvc.name() );
  }

}
