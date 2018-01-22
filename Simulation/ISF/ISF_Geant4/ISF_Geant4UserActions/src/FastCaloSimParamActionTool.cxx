/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "ISF_Geant4UserActions/FastCaloSimParamActionTool.h"
#include "LArG4Code/ILArCalculatorSvc.h"
#include "TileG4Interfaces/ITileCalculator.h"

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
    , m_tileCalculator("TileGeoG4SDCalc", name)
  {
    declareProperty("shift_lar_subhit",m_config.shift_lar_subhit, "");
    declareProperty("shorten_lar_step",m_config.shorten_lar_step, "");
    declareProperty("stepInfoCollName",m_config.stepInfoCollName, "");

    declareProperty("maxRadius",m_config.m_maxRadius, "");
    declareProperty("maxRadiusLAr",m_config.m_maxRadiusLAr, "");
    declareProperty("maxRadiusHEC",m_config.m_maxRadiusHEC, "");
    declareProperty("maxRadiusFCAL",m_config.m_maxRadiusFCAL, "");
    declareProperty("maxRadiusTile",m_config.m_maxRadiusTile, "");

    declareProperty("maxTime",m_config.m_maxTime, "");
    declareProperty("maxTimeLAr",m_config.m_maxTimeLAr, "");
    declareProperty("maxTimeHEC",m_config.m_maxTimeHEC, "");
    declareProperty("maxTimeFCAL",m_config.m_maxTimeFCAL, "");
    declareProperty("maxTimeTile",m_config.m_maxTimeTile, "");

    declareProperty("maxEtaPS", m_config.m_maxEtaPS, "");
    declareProperty("maxPhiPS", m_config.m_maxPhiPS, "");
    declareProperty("maxrPS", m_config.m_maxrPS, "");

    declareProperty("maxEtaEM1", m_config.m_maxEtaEM1, "");
    declareProperty("maxPhiEM1", m_config.m_maxPhiEM1, "");
    declareProperty("maxrEM1", m_config.m_maxrEM1, "");

    declareProperty("maxEtaEM2", m_config.m_maxEtaEM2, "");
    declareProperty("maxPhiEM2", m_config.m_maxPhiEM2, "");
    declareProperty("maxrEM2", m_config.m_maxrEM2, "");

    declareProperty("maxEtaEM3", m_config.m_maxEtaEM3, "");
    declareProperty("maxPhiEM3", m_config.m_maxPhiEM3, "");
    declareProperty("maxrEM3", m_config.m_maxrEM3, "");

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
    declareProperty("TileCalculator", m_tileCalculator);
  }

  StatusCode FastCaloSimParamActionTool::initialize()
  {
    m_config.calculator_EMECIW_pos.setTypeAndName(m_emepiwcalc.typeAndName());
    m_config.calculator_EMECIW_neg.setTypeAndName(m_emeniwcalc.typeAndName());
    m_config.calculator_EMECOW_pos.setTypeAndName(m_emepowcalc.typeAndName());
    m_config.calculator_EMECOW_neg.setTypeAndName(m_emenowcalc.typeAndName());
    m_config.calculator_BOB.setTypeAndName(m_emeobarcalc.typeAndName());
    m_config.calculator_EMB.setTypeAndName(m_embcalc.typeAndName());
    m_config.calculator_FCAL1.setTypeAndName(m_fcal1calc.typeAndName());
    m_config.calculator_FCAL2.setTypeAndName(m_fcal2calc.typeAndName());
    m_config.calculator_FCAL3.setTypeAndName(m_fcal3calc.typeAndName());
    m_config.calculator_HEC.setTypeAndName(m_heccalc.typeAndName());
    m_config.calculator_EMBPS.setTypeAndName(m_pscalc.typeAndName());
    m_config.calculator_EMEPS.setTypeAndName(m_emepscalc.typeAndName());
    m_config.calculator_TILE.setTypeAndName(m_tileCalculator.typeAndName());

    return StatusCode::SUCCESS;
  }

  std::unique_ptr<FastCaloSimParamAction>  FastCaloSimParamActionTool::makeAction()
  {
    ATH_MSG_DEBUG("makeAction");
    auto action = CxxUtils::make_unique<FastCaloSimParamAction>(m_config);
    return std::move(action);
  }

  StatusCode FastCaloSimParamActionTool::queryInterface(const InterfaceID& riid, void** ppvIf){

    if(riid == IG4RunActionTool::interfaceID()) {
      *ppvIf = (IG4RunActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    }
    if(riid == IG4EventActionTool::interfaceID()) {
      *ppvIf = (IG4EventActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    }
    if(riid == IG4SteppingActionTool::interfaceID()) {
      *ppvIf = (IG4SteppingActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    }
    return ActionToolBase<FastCaloSimParamAction>::queryInterface(riid, ppvIf);
  }

} // namespace G4UA
