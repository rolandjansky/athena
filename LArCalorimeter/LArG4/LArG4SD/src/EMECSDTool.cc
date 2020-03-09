/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "EMECSDTool.h"

#include "LArG4Code/SDWrapper.h"

namespace LArG4
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  EMECSDTool::EMECSDTool(const std::string& type, const std::string& name,
                                 const IInterface *parent)
    : SimpleSDTool(type, name, parent)
    , m_emepiwcalc("EMECPosInnerWheelCalculator", name)
    , m_emeniwcalc("EMECNegInnerWheelCalculator", name)
    , m_emepowcalc("EMECPosOuterWheelCalculator", name)
    , m_emenowcalc("EMECNegOuterWheelCalculator", name)
    , m_emepscalc("EMECPresamplerCalculator", name)
    , m_emepobarcalc("EMECPosBackOuterBarretteCalculator", name)
    , m_emenobarcalc("EMECNegBackOuterBarretteCalculator", name)
  {
    declareProperty("PosIWVolumes", m_posIWVolumes);
    declareProperty("NegIWVolumes", m_negIWVolumes);
    declareProperty("PosOWVolumes", m_posOWVolumes);
    declareProperty("NegOWVolumes", m_negOWVolumes);
    declareProperty("PresVolumes", m_presVolumes);
    declareProperty("PosBOBarretteVolumes", m_posBOBVolumes);
    declareProperty("NegBOBarretteVolumes", m_negBOBVolumes);

    declareProperty("EMECPosIWCalculator", m_emepiwcalc);
    declareProperty("EMECNegIWCalculator", m_emeniwcalc);
    declareProperty("EMECPosOWCalculator", m_emepowcalc);
    declareProperty("EMECNegOWCalculator", m_emenowcalc);
    declareProperty("EMECPSCalculator", m_emepscalc);
    declareProperty("EMECPosBOBCalculator", m_emepobarcalc);
    declareProperty("EMECNegBOBCalculator", m_emenobarcalc);
  }

  //---------------------------------------------------------------------------
  // Initialization of Athena-components
  //---------------------------------------------------------------------------
  StatusCode EMECSDTool::initializeCalculators()
  {
    ATH_CHECK(m_emepiwcalc.retrieve());
    ATH_CHECK(m_emeniwcalc.retrieve());
    ATH_CHECK(m_emepowcalc.retrieve());
    ATH_CHECK(m_emenowcalc.retrieve());
    ATH_CHECK(m_emepscalc.retrieve());
    ATH_CHECK(m_emepobarcalc.retrieve());
    ATH_CHECK(m_emenobarcalc.retrieve());

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Create the SD wrapper for current worker thread
  //---------------------------------------------------------------------------
  G4VSensitiveDetector* EMECSDTool::makeSD() const
  {
    // Create the wrapper
    auto sdWrapper = new SimpleSDWrapper("LArEMECSDWrapper", m_outputCollectionNames[0]);

    // Add the SDs
    sdWrapper->addSD(
      makeOneSD( "LAr::EMEC::Pos::InnerWheel", &*m_emepiwcalc, m_posIWVolumes )
    );
    sdWrapper->addSD(
      makeOneSD( "LAr::EMEC::Neg::InnerWheel", &*m_emeniwcalc, m_negIWVolumes )
    );
    sdWrapper->addSD(
      makeOneSD( "LAr::EMEC::Pos::OuterWheel", &*m_emepowcalc, m_posOWVolumes )
    );
    sdWrapper->addSD(
      makeOneSD( "LAr::EMEC::Neg::OuterWheel", &*m_emenowcalc, m_negOWVolumes )
    );
    sdWrapper->addSD(
      makeOneSD( "LAr::Endcap::Presampler::LiquidArgon", &*m_emepscalc, m_presVolumes )
    );
    sdWrapper->addSD(
      makeOneSD( "LAr::EMEC::Pos::BackOuterBarrette::Module::Phidiv", &*m_emepobarcalc, m_posBOBVolumes )
    );
    sdWrapper->addSD(
      makeOneSD( "LAr::EMEC::Neg::BackOuterBarrette::Module::Phidiv", &*m_emenobarcalc, m_negBOBVolumes )
    );

    // Setup frozen shower SD
    if(useFrozenShowers())
      {
        sdWrapper->addFastSimSD("EndcapFastSimDedicatedSD");
      }

    // Return the wrapper as my SD
    return sdWrapper;
  }

}
