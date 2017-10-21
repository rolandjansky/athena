/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FCS_StepInfoSDTool.h"

#include <memory>

// External includes
#include "CLHEP/Units/SystemOfUnits.h"

// Athena includes
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArMiniFCAL_ID.h"
#include "LArG4Code/ILArCalculatorSvc.h"
#include "LArG4Code/VolumeUtils.h"
#include "TileG4Interfaces/ITileCalculator.h"

// Local includes
#include "LArFCS_StepInfoSD.h"
#include "TileFCS_StepInfoSD.h"
#include "SDWrapper.h"

/// Template instantiation for FCS_StepInfoSD
using FCS_StepInfoSDWrapper = FCS_Param::detail::SDWrapper<FCS_StepInfoSD, ISF_FCS_Parametrization::FCS_StepInfoCollection>;

namespace FCS_Param
{

  //---------------------------------------------------------------------------
  // Tool constructor
  //---------------------------------------------------------------------------
  FCS_StepInfoSDTool::FCS_StepInfoSDTool(const std::string& type, const std::string& name,
                             const IInterface* parent)
    : SensitiveDetectorBase(type, name, parent)
    , m_hitCollName("EventSteps")
    , m_bpsmodcalc("EMBPresamplerCalculator", name)
    , m_embcalc("EMBCalculator", name)
    , m_emepiwcalc("EMECPosInnerWheelCalculator", name)
    , m_emeniwcalc("EMECNegInnerWheelCalculator", name)
    , m_emepowcalc("EMECPosOuterWheelCalculator", name)
    , m_emenowcalc("EMECNegOuterWheelCalculator", name)
    , m_emepscalc("EMECPresamplerCalculator", name)
    , m_emeobarcalc("EMECBackOuterBarretteCalculator", name)
    , m_heccalc("HECWheelCalculator", name)
    , m_fcal1calc("FCAL1Calculator", name)
    , m_fcal2calc("FCAL2Calculator", name)
    , m_fcal3calc("FCAL3Calculator", name)
    , m_minfcalcalc("MiniFCALCalculator", name)
    , m_tileCalculator("TileGeoG4SDCalc", name)
    , m_larEmID(nullptr)
    , m_larFcalID(nullptr)
    , m_larHecID(nullptr)
    , m_larMiniFcalID(nullptr)
    , m_config()
  {
    declareProperty("HitCollectionName", m_hitCollName);
    declareProperty("StacVolumes", m_stacVolumes);
    declareProperty("PresamplerVolumes", m_presBarVolumes);
    declareProperty("PosIWVolumes", m_posIWVolumes);
    declareProperty("NegIWVolumes", m_negIWVolumes);
    declareProperty("PosOWVolumes", m_posOWVolumes);
    declareProperty("NegOWVolumes", m_negOWVolumes);
    declareProperty("PresVolumes", m_presECVolumes);
    declareProperty("BOBarretteVolumes", m_bobVolumes);
    declareProperty("FCAL1Volumes", m_fcal1Volumes);
    declareProperty("FCAL2Volumes", m_fcal2Volumes);
    declareProperty("FCAL3Volumes", m_fcal3Volumes);
    declareProperty("SliceVolumes", m_sliceVolumes);
    declareProperty("MiniVolumes", m_miniVolumes);
    declareProperty("TileVolumes", m_tileVolumes);

    declareProperty("EMBPSCalibrationCalculator",m_bpsmodcalc);
    declareProperty("EMBCalibrationCalculator",m_embcalc);
    declareProperty("EMECPosIWCalibrationCalculator",m_emepiwcalc);
    declareProperty("EMECNegIWCalibrationCalculator",m_emeniwcalc);
    declareProperty("EMECPosOWCalibrationCalculator",m_emepowcalc);
    declareProperty("EMECNegOWCalibrationCalculator",m_emenowcalc);
    declareProperty("EMECPSCalibrationCalculator",m_emepscalc);
    declareProperty("EMECBOBCalibrationCalculator",m_emeobarcalc);
    declareProperty("HECWActiveCalculator",m_heccalc);
    declareProperty("FCAL1CalibCalculator",m_fcal1calc);
    declareProperty("FCAL2CalibCalculator",m_fcal2calc);
    declareProperty("FCAL3CalibCalculator",m_fcal3calc);
    declareProperty("MiniFCALActiveCalibrationCalculator",m_minfcalcalc);
    declareProperty("TileCalculator", m_tileCalculator);

    declareProperty("shift_lar_subhit",m_config.shift_lar_subhit, "");
    declareProperty("shorten_lar_step",m_config.shorten_lar_step, "");

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
  }

  //---------------------------------------------------------------------------
  // Initialize the tool
  //---------------------------------------------------------------------------
  StatusCode FCS_StepInfoSDTool::initialize()
  {
    ATH_MSG_DEBUG( "Initializing " << name() );

    const CaloIdManager* idMgr = nullptr;
    CHECK( detStore()->retrieve(idMgr) );
    if( (m_larEmID = idMgr->getEM_ID()) == nullptr) {
      ATH_MSG_ERROR("Invalid LAr EM ID helper");
      return StatusCode::FAILURE;
    }
    if( (m_larFcalID = idMgr->getFCAL_ID()) == nullptr) {
      ATH_MSG_ERROR("Invalid LAr FCAL ID helper");
      return StatusCode::FAILURE;
    }
    if( (m_larHecID = idMgr->getHEC_ID()) == nullptr) {
      ATH_MSG_ERROR("Invalid LAr HEC ID helper");
      return StatusCode::FAILURE;
    }
    if( (m_larMiniFcalID = idMgr->getMiniFCAL_ID()) == nullptr) {
      ATH_MSG_ERROR("Invalid LAr Mini FCAL ID helper");
      return StatusCode::FAILURE;
    }

    // No general volume list for SensitiveDetectorBase
    m_noVolumes = true;

    ATH_CHECK(this->initializeCalculators());

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Initialization of Athena-components
  //---------------------------------------------------------------------------
  StatusCode FCS_StepInfoSDTool::initializeCalculators()
  {
    // Lots of calculators !!!
    ATH_CHECK(m_bpsmodcalc.retrieve());
    ATH_CHECK(m_embcalc.retrieve());
    ATH_CHECK(m_emepiwcalc.retrieve());
    ATH_CHECK(m_emeniwcalc.retrieve());
    ATH_CHECK(m_emepowcalc.retrieve());
    ATH_CHECK(m_emenowcalc.retrieve());
    ATH_CHECK(m_emepscalc.retrieve());
    ATH_CHECK(m_emeobarcalc.retrieve());
    ATH_CHECK(m_heccalc.retrieve());
    ATH_CHECK(m_fcal1calc.retrieve());
    ATH_CHECK(m_fcal2calc.retrieve());
    ATH_CHECK(m_fcal3calc.retrieve());
    ATH_CHECK(m_minfcalcalc.retrieve());
    ATH_CHECK(m_tileCalculator.retrieve());

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Collect hits for this event
  //---------------------------------------------------------------------------
  StatusCode FCS_StepInfoSDTool::Gather()
  {
    ATH_MSG_DEBUG("Gathering hits to write out in " << name());
    auto sdWrapper = dynamic_cast<FCS_StepInfoSDWrapper*>( getSD() );
    if(!sdWrapper) {
      ATH_MSG_ERROR("Failed to cast SD to FCS_StepInfoSDWrapper");
      return StatusCode::FAILURE;
    }
    sdWrapper->EndOfAthenaEvent();
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Create SD wrapper for current thread
  //---------------------------------------------------------------------------
  G4VSensitiveDetector* FCS_StepInfoSDTool::makeSD()
  {
    // Create the wrapper
    auto sdWrapper = new FCS_StepInfoSDWrapper("FCS_StepInfoSDWrapper", m_hitCollName);

    // Create the SDs.
    sdWrapper->addSD( makeOneLArSD( "Barrel::Presampler::Module::StepInfo", &*m_bpsmodcalc, m_presBarVolumes ) );
    sdWrapper->addSD( makeOneLArSD( "EMB::STAC::StepInfo", &*m_embcalc, m_stacVolumes ) );
    sdWrapper->addSD( makeOneLArSD( "EMEC::Pos::InnerWheel::StepInfo", &*m_emepiwcalc, m_posIWVolumes ) );
    sdWrapper->addSD( makeOneLArSD( "EMEC::Neg::InnerWheel::StepInfo", &*m_emeniwcalc, m_negIWVolumes ) );
    sdWrapper->addSD( makeOneLArSD( "EMEC::Pos::OuterWheel::StepInfo", &*m_emepowcalc, m_posOWVolumes ) );
    sdWrapper->addSD( makeOneLArSD( "EMEC::Neg::OuterWheel::StepInfo", &*m_emenowcalc, m_negOWVolumes ) );
    sdWrapper->addSD( makeOneLArSD( "Endcap::Presampler::LiquidArgon::StepInfo", &*m_emepscalc, m_presECVolumes ) );
    sdWrapper->addSD( makeOneLArSD( "EMEC::BackOuterBarrette::StepInfo", &*m_emeobarcalc, m_bobVolumes ) );
    sdWrapper->addSD( makeOneLArSD( "FCAL::Module1::Gap::StepInfo", &*m_fcal1calc, m_fcal1Volumes ) );
    sdWrapper->addSD( makeOneLArSD( "FCAL::Module2::Gap::StepInfo", &*m_fcal2calc, m_fcal2Volumes ) );
    sdWrapper->addSD( makeOneLArSD( "FCAL::Module3::Gap::StepInfo", &*m_fcal3calc, m_fcal3Volumes ) );
    sdWrapper->addSD( makeOneLArSD( "HEC::Module::Depth::Slice::Wheel::StepInfo", &*m_heccalc, m_sliceVolumes ) );
    sdWrapper->addSD( makeOneLArSD( "MiniFCAL::Wafer::StepInfo", &*m_minfcalcalc, m_miniVolumes ) );
    sdWrapper->addSD( makeOneTileSD( "Tile::Scintillator::StepInfo", &*m_tileCalculator, m_tileVolumes ) );

    return sdWrapper;
  }

  //---------------------------------------------------------------------------
  // Create one LAr SD
  //---------------------------------------------------------------------------
  std::unique_ptr<FCS_StepInfoSD>
  FCS_StepInfoSDTool::makeOneLArSD(const std::string& sdName, ILArCalculatorSvc* calc,
                          const std::vector<std::string>& volumes) const
  {
    ATH_MSG_VERBOSE( name() << " makeOneSD" );

    // Parse the wildcard patterns for existing volume names
    auto parsedVolumes = LArG4::findLogicalVolumes(volumes, msg()); // from LArG4Code/VolumeUtils.h

    // Inject the Calculator into m_config
    FCS_Param::Config config(m_config);
    config.m_LArCalculator = calc;
    if(!calc) {
      throw GaudiException("nullptr for ILArCalculatorSvc provided to constructor for: " + sdName,
                           name(), StatusCode::FAILURE);
    }
    if(msgLvl(MSG::VERBOSE))    { config.verboseLevel = 10; }
    else if(msgLvl(MSG::DEBUG)) { config.verboseLevel = 5;  }
    // Create the simple SD
   std::unique_ptr<FCS_StepInfoSD> sd =
      std::make_unique<LArFCS_StepInfoSD>(sdName, config);
    sd->setupHelpers(m_larEmID, m_larFcalID, m_larHecID, m_larMiniFcalID);

    // Assign the volumes to the SD
    if( this->assignSD( sd.get(), parsedVolumes ).isFailure() ) {
      // TODO: can I just return NULL here?
      throw GaudiException("Failed to assign sd: " + sdName,
                           name(), StatusCode::FAILURE);
    }
    return std::move(sd);
  }

  //---------------------------------------------------------------------------
  // Create one Tile SD
  //---------------------------------------------------------------------------
  std::unique_ptr<FCS_StepInfoSD>
  FCS_StepInfoSDTool::makeOneTileSD(const std::string& sdName, ITileCalculator* calc,
                          const std::vector<std::string>& volumes) const
  {
    ATH_MSG_VERBOSE( name() << " makeOneSD" );

    // Inject the Calculator into m_config
    FCS_Param::Config config(m_config);
    config.m_TileCalculator = calc;
    if(!calc) {
      throw GaudiException("nullptr for ITileCalculator provided to constructor for: " + sdName,
                           name(), StatusCode::FAILURE);
    }
    if(msgLvl(MSG::VERBOSE))    { config.verboseLevel = 10; }
    else if(msgLvl(MSG::DEBUG)) { config.verboseLevel = 5;  }
     // Create the simple SD
    std::unique_ptr<FCS_StepInfoSD> sd =
      std::make_unique<TileFCS_StepInfoSD>(sdName, config);
    sd->setupHelpers(m_larEmID, m_larFcalID, m_larHecID, m_larMiniFcalID);

    // Assign the volumes to the SD
    if( this->assignSD( sd.get(), volumes ).isFailure() ) {
      // TODO: can I just return NULL here?
      throw GaudiException("Failed to assign sd: " + sdName,
                           name(), StatusCode::FAILURE);
    }
    return std::move(sd);
  }

} // namespace FCS_Param
