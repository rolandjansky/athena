/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArGeoH62004EventActionTool.h"

#include "LArG4RunControl/LArGeoTB2004Options.h"

namespace G4UA
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  LArGeoH62004EventActionTool::LArGeoH62004EventActionTool(const std::string& type,
                                                                 const std::string& name,
                                                                 const IInterface* parent)
    : ActionToolBase<LArGeoH62004EventAction>(type, name, parent)
  {
    declareProperty("yTable", m_config.yTable);
    declareProperty("CryoXPosition", m_config.cryoXposition);
    //declareProperty("PrintStep", m_config.printstep);
  }


  StatusCode LArGeoH62004EventActionTool::initialize()
  {
    //FIXME Old-style configuration - to be removed
    const LArGeoTB2004Options *largeoTB2004Options = nullptr;
    if(detStore()->retrieve(largeoTB2004Options, "LArGeoTB2004Options").isFailure())
      {
        ATH_MSG_WARNING ( "Can't access LArGeoTB2004Options, using default values" );
      }
    m_config.yTable = largeoTB2004Options->TableYPosition();
    m_config.cryoXposition = largeoTB2004Options->CryoXPosition();

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Create the action on request
  //---------------------------------------------------------------------------
  std::unique_ptr<LArGeoH62004EventAction>
  LArGeoH62004EventActionTool::makeAction()
  {
    ATH_MSG_DEBUG("makeAction");
    return std::make_unique<LArGeoH62004EventAction>(m_config);
  }

  //---------------------------------------------------------------------------
  // Query interface
  //---------------------------------------------------------------------------
  StatusCode LArGeoH62004EventActionTool::queryInterface(const InterfaceID& riid, void** ppvIf)
  {
    if(riid == IG4EventActionTool::interfaceID()) {
      *ppvIf = (IG4EventActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    }

    return ActionToolBase<LArGeoH62004EventAction>::queryInterface(riid, ppvIf);
  }

}