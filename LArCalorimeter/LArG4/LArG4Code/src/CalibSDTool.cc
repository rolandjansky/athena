/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4Code/CalibSDTool.h"

// Framework utilities
#include "CxxUtils/make_unique.h"

// ID helper includes
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArMiniFCAL_ID.h"
#include "CaloIdentifier/CaloDM_ID.h"

// Local includes
#include "LArG4Code/SDWrapper.h"
#include "LArG4Code/ILArCalibCalculatorSvc.h"
#include "LArG4Code/VolumeUtils.h"

namespace LArG4
{

  //---------------------------------------------------------------------------
  // Tool constructor
  //---------------------------------------------------------------------------
  CalibSDTool::CalibSDTool(const std::string& type, const std::string& name,
                           const IInterface* parent)
    : SensitiveDetectorBase(type, name, parent),
      m_doPID(false),
      m_larEmID(nullptr),
      m_larFcalID(nullptr),
      m_larHecID(nullptr),
      m_larMiniFcalID(nullptr),
      m_caloDmID(nullptr),
      m_id_helper(nullptr)
  {
    declareProperty("ParticleID", m_doPID);
  }

  //---------------------------------------------------------------------------
  // Initialize the tool
  //---------------------------------------------------------------------------
  StatusCode CalibSDTool::initialize()
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
    if( (m_caloDmID = idMgr->getDM_ID()) == nullptr) {
      ATH_MSG_ERROR("Invalid CaloDM ID helper");
      return StatusCode::FAILURE;
    }

    ATH_CHECK(detStore()->retrieve(m_id_helper));

    // No general volume list for SensitiveDetectorBase
    m_noVolumes = true;

    ATH_CHECK(this->initializeCalculators());

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Collect hits for this event
  //---------------------------------------------------------------------------
  StatusCode CalibSDTool::Gather()
  {
    auto sdWrapper = dynamic_cast<CalibSDWrapper*>( getSD() );
    if(!sdWrapper) {
      ATH_MSG_ERROR("Failed to cast SD to CalibSDWrapper");
      return StatusCode::FAILURE;
    }
    sdWrapper->EndOfAthenaEvent();
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Create one calib SD
  //---------------------------------------------------------------------------
  std::unique_ptr<LArG4CalibSD>
  CalibSDTool::makeOneSD(const std::string& sdName, ILArCalibCalculatorSvc* calc,
                         const std::vector<std::string>& volumes) const
  {
    ATH_MSG_VERBOSE( name() << " makeOneSD" );

    // Parse the wildcard patterns for existing volume names
    auto parsedVolumes = findLogicalVolumes(volumes, msg());

    // Create the calib SD
    auto sd = CxxUtils::make_unique<LArG4CalibSD>(sdName, calc, m_doPID);
    sd->setupHelpers(m_larEmID, m_larFcalID, m_larHecID, m_larMiniFcalID, m_caloDmID);

    const std::string dead("Dead");
    if(sdName.find(dead)==std::string::npos) {
      sd->addDetectorHelper(m_id_helper);
    }

    // Assign the volumes to the SD
    if( assignSD( sd.get(), parsedVolumes ).isFailure() ) {
      // TODO: can I just return NULL here?
      throw GaudiException("Failed to assign sd: " + sdName,
                           name(), StatusCode::FAILURE);
    }
    return std::move(sd);
  }

} // namespace LArG4
