/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "ISF_Geant4UserActions/FastCaloSimParamActionTool.h"
#include "LArG4Code/ILArCalculatorSvc.h"

namespace G4UA{

  FastCaloSimParamActionTool::FastCaloSimParamActionTool(const std::string& type, const std::string& name,const IInterface* parent)
    : ActionToolBase<FastCaloSimParamAction>(type, name, parent)
    , m_config()
    , m_emepiwcalc("EMECPosInnerWheelCalculator", name)
    , m_emeniwcalc("EMECNegInnerWheelCalculator", name)
    , m_emepowcalc("EMECPosOuterWheelCalculator", name)
    , m_emenowcalc("EMECNegOuterWheelCalculator", name)
    , m_emeobarcalc("EMECBackOuterBarretteCalculator", name)
    , m_embcalc("EMBCalculator", name)
    , m_fcal1calc("FCAL1Calculator", name)
    , m_fcal2calc("FCAL2Calculator", name)
    , m_fcal3calc("FCAL3Calculator", name)
    , m_heccalc("HECWheelCalculator", name)
    , m_pscalc("EMBPresamplerCalculator", name)
    , m_emepscalc("EMECPresamplerCalculator", name)
  {
    declareProperty("shift_lar_subhit",m_config.shift_lar_subhit, "");
    declareProperty("shorten_lar_step",m_config.shorten_lar_step, "");

    declareProperty("EMECPosIWCalculator", m_emepiwcalc);
    declareProperty("EMECNegIWCalculator", m_emeniwcalc);
    declareProperty("EMECPosOWCalculator", m_emepowcalc);
    declareProperty("EMECNegOWCalculator", m_emenowcalc);
    declareProperty("EMECBOBCalculator", m_emeobarcalc);
    declareProperty("EMBCalculator",m_embcalc);
    declareProperty("FCAL1Calculator", m_fcal1calc);
    declareProperty("FCAL2Calculator", m_fcal2calc);
    declareProperty("FCAL3Calculator", m_fcal3calc);
    declareProperty("HECWheelCalculator", m_heccalc);
    declareProperty("EMBPSCalculator",m_pscalc);
    declareProperty("EMECPSCalculator", m_emepscalc);
    }

  StatusCode FastCaloSimParamActionTool::initialize()
  {
    ATH_CHECK(m_emepiwcalc.retrieve());
    ATH_CHECK(m_emeniwcalc.retrieve());
    ATH_CHECK(m_emepowcalc.retrieve());
    ATH_CHECK(m_emenowcalc.retrieve());
    ATH_CHECK(m_emeobarcalc.retrieve());
    ATH_CHECK(m_embcalc.retrieve());
    ATH_CHECK(m_fcal1calc.retrieve());
    ATH_CHECK(m_fcal2calc.retrieve());
    ATH_CHECK(m_fcal3calc.retrieve());
    ATH_CHECK(m_heccalc.retrieve());
    ATH_CHECK(m_pscalc.retrieve());
    ATH_CHECK(m_emepscalc.retrieve());
    m_config.calculator_EMECIW_pos = &*m_emepiwcalc;
    m_config.calculator_EMECIW_neg = &*m_emeniwcalc;
    m_config.calculator_EMECOW_pos = &*m_emepowcalc;
    m_config.calculator_EMECOW_neg = &*m_emenowcalc;
    m_config.calculator_BOB        = &*m_emeobarcalc;
    m_config.calculator_EMB        = &*m_embcalc;
    m_config.calculator_FCAL1      = &*m_fcal1calc;
    m_config.calculator_FCAL2      = &*m_fcal2calc;
    m_config.calculator_FCAL3      = &*m_fcal3calc;
    m_config.calculator_HEC        = &*m_heccalc;
    m_config.calculator_EMBPS      = &*m_pscalc;
    m_config.calculator_EMEPS      = &*m_emepscalc;

    return StatusCode::SUCCESS;
  }

  std::unique_ptr<FastCaloSimParamAction>  FastCaloSimParamActionTool::makeAction()
  {
    ATH_MSG_DEBUG("makeAction");
    auto action = CxxUtils::make_unique<FastCaloSimParamAction>(m_config);
    return std::move(action);
  }

  StatusCode FastCaloSimParamActionTool::queryInterface(const InterfaceID& riid, void** ppvIf){

    if(riid == IBeginRunActionTool::interfaceID()) {
      *ppvIf = (IBeginRunActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    }
    if(riid == IEndRunActionTool::interfaceID()) {
      *ppvIf = (IEndRunActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    }
    if(riid == IBeginEventActionTool::interfaceID()) {
      *ppvIf = (IBeginEventActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    }
    if(riid == IEndEventActionTool::interfaceID()) {
      *ppvIf = (IEndEventActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    }
    if(riid == ISteppingActionTool::interfaceID()) {
      *ppvIf = (ISteppingActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    }
    return ActionToolBase<FastCaloSimParamAction>::queryInterface(riid, ppvIf);
  }

} // namespace G4UA
