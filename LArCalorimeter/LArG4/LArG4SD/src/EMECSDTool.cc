/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "EMECSDTool.h"

#include "LArG4EC/EnergyCalculator.h"
#include "LArG4EC/LArEndcapPresamplerCalculator.h"
#include "LArG4Code/SDWrapper.h"

namespace LArG4
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  EMECSDTool::EMECSDTool(const std::string& type, const std::string& name,
                                 const IInterface *parent)
    : SimpleSDTool(type, name, parent),
      m_hitCollName("LArHitEMEC")
  {
    declareProperty("HitCollectionName", m_hitCollName);
    declareProperty("PosIWVolumes", m_posIWVolumes);
    declareProperty("NegIWVolumes", m_negIWVolumes);
    declareProperty("PosOWVolumes", m_posOWVolumes);
    declareProperty("NegOWVolumes", m_negOWVolumes);
    declareProperty("PresVolumes", m_presVolumes);
    declareProperty("BOBarretteVolumes", m_bobVolumes);
  }

  //---------------------------------------------------------------------------
  // Create the SD wrapper for current worker thread
  //---------------------------------------------------------------------------
  G4VSensitiveDetector* EMECSDTool::makeSD()
  {
    // Create the wrapper
    auto sdWrapper = new SimpleSDWrapper("LArEMECSDWrapper", m_hitCollName);

    // Add the SDs
    sdWrapper->addSD(
      makeOneSD( "LAr::EMEC::Pos::InnerWheel",
                  new EC::EnergyCalculator(LArWheelCalculator::InnerAbsorberWheel,
                                           EC::EnergyCalculator::EMEC_ECOR_ROPT, 1),
                  m_posIWVolumes )
    );
    sdWrapper->addSD(
      makeOneSD( "LAr::EMEC::Neg::InnerWheel",
                 new EC::EnergyCalculator(LArWheelCalculator::InnerAbsorberWheel,
                                          EC::EnergyCalculator::EMEC_ECOR_ROPT, -1),
                 m_negIWVolumes )
    );
    sdWrapper->addSD(
      makeOneSD( "LAr::EMEC::Pos::OuterWheel",
                 new EC::EnergyCalculator(LArWheelCalculator::OuterAbsorberWheel,
                                          EC::EnergyCalculator::EMEC_ECOR_ROPT, 1),
                 m_posOWVolumes )
    );
    sdWrapper->addSD(
      makeOneSD( "LAr::EMEC::Neg::OuterWheel",
                 new EC::EnergyCalculator(LArWheelCalculator::OuterAbsorberWheel,
                                          EC::EnergyCalculator::EMEC_ECOR_ROPT, -1),
                 m_negOWVolumes )
    );
    sdWrapper->addSD(
      makeOneSD( "LAr::Endcap::Presampler::LiquidArgon",
                 LArEndcapPresamplerCalculator::GetCalculator(),
                 m_presVolumes )
    );
    sdWrapper->addSD(
      makeOneSD( "LAr::EMEC::BackOuterBarrette::Module::Phidiv",
                 new EC::EnergyCalculator(LArWheelCalculator::BackOuterBarretteWheel),
                 m_bobVolumes )
    );

    // Setup frozen shower SD
    if(useFrozenShowers()) {
      sdWrapper->addFastSimSD("EndcapFastSimDedicatedSD");
    }

    // Return the wrapper as my SD
    return sdWrapper;
  }

}
