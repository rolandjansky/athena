/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FCS_StepInfoSDTool.h"

// External includes
#include "CLHEP/Units/SystemOfUnits.h"

// Framework includes
#include "CxxUtils/make_unique.h"

// ID helper includes
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArMiniFCAL_ID.h"

// Local includes
#include "FCS_StepInfoSD.h"
#include "SDWrapper.h"
#include "LArG4Code/VolumeUtils.h"

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
    , m_larEmID(nullptr)
    , m_larFcalID(nullptr)
    , m_larHecID(nullptr)
    , m_larMiniFcalID(nullptr)
    , m_config()
  {
    declareProperty("shift_lar_subhit",m_config.shift_lar_subhit, "");
    declareProperty("shorten_lar_step",m_config.shorten_lar_step, "");

    declareProperty("maxRadius",m_config.m_maxRadius, "");
    declareProperty("maxRadiusLAr",m_config.m_maxRadiusLAr, "");
    declareProperty("maxRadiusHEC",m_config.m_maxRadiusHEC, "");
    declareProperty("maxRadiusFCAL",m_config.m_maxRadiusFCAL, "");
    //declareProperty("maxRadiusTile",m_config.m_maxRadiusTile, "");

    declareProperty("maxTime",m_config.m_maxTime, "");
    declareProperty("maxTimeLAr",m_config.m_maxTimeLAr, "");
    declareProperty("maxTimeHEC",m_config.m_maxTimeHEC, "");
    declareProperty("maxTimeFCAL",m_config.m_maxTimeFCAL, "");
    //declareProperty("maxTimeTile",m_config.m_maxTimeTile, "");

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
  // Create one SD
  //---------------------------------------------------------------------------
  std::unique_ptr<FCS_StepInfoSD>
  FCS_StepInfoSDTool::makeOneSD(const std::string& sdName, ILArCalculatorSvc* calc,
                          const std::vector<std::string>& volumes) const
  {
    ATH_MSG_VERBOSE( name() << " makeOneSD" );

    // Parse the wildcard patterns for existing volume names
    auto parsedVolumes = LArG4::findLogicalVolumes(volumes, msg()); // from LArG4Code/VolumeUtils.h

    // Create the simple SD
    auto sd = CxxUtils::make_unique<FCS_StepInfoSD>
      (sdName, calc, m_config);
    sd->setupHelpers(m_larEmID, m_larFcalID, m_larHecID, m_larMiniFcalID);

    // Assign the volumes to the SD
    if( this->assignSD( sd.get(), parsedVolumes ).isFailure() ) {
      // TODO: can I just return NULL here?
      throw GaudiException("Failed to assign sd: " + sdName,
                           name(), StatusCode::FAILURE);
    }
    return std::move(sd);
  }

} // namespace LArG4
