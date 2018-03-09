/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4Code/SimpleSDTool.h"

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
#include "LArG4Code/LArG4SimpleSD.h"
#include "LArG4Code/SDWrapper.h"
#include "LArG4Code/VolumeUtils.h"

namespace LArG4
{

  //---------------------------------------------------------------------------
  // Tool constructor
  //---------------------------------------------------------------------------
  SimpleSDTool::SimpleSDTool(const std::string& type, const std::string& name,
                             const IInterface* parent)
    : SensitiveDetectorBase(type, name, parent),
      m_timeBinType("Default"),
      m_timeBinWidth(2.5*CLHEP::ns),
      m_useFrozenShowers(false),
      m_larEmID(nullptr),
      m_larFcalID(nullptr),
      m_larHecID(nullptr),
      m_larMiniFcalID(nullptr)
  {
    declareProperty("TimeBinType", m_timeBinType);
    declareProperty("TimeBinWidth", m_timeBinWidth);
    declareProperty("UseFrozenShowers", m_useFrozenShowers);
  }

  //---------------------------------------------------------------------------
  // Initialize the tool
  //---------------------------------------------------------------------------
  StatusCode SimpleSDTool::initialize()
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
  StatusCode SimpleSDTool::Gather()
  {
    ATH_MSG_DEBUG("Gathering hits to write out in " << name());
    auto sdWrapper = dynamic_cast<SimpleSDWrapper*>( getSD() );
    if(!sdWrapper) {
      ATH_MSG_ERROR("Failed to cast SD to SimpleSDWrapper");
      return StatusCode::FAILURE;
    }
    sdWrapper->EndOfAthenaEvent();
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Create one SD
  //---------------------------------------------------------------------------
  std::unique_ptr<LArG4SimpleSD>
  SimpleSDTool::makeOneSD(const std::string& sdName, ILArCalculatorSvc* calc,
                          const std::vector<std::string>& volumes) const
  {
    ATH_MSG_VERBOSE( name() << " makeOneSD" );

    // Parse the wildcard patterns for existing volume names
    auto parsedVolumes = findLogicalVolumes(volumes, msg());

    // Create the simple SD
    auto sd = CxxUtils::make_unique<LArG4SimpleSD>
      (sdName, calc, m_timeBinType, m_timeBinWidth);
    sd->setupHelpers(m_larEmID, m_larFcalID, m_larHecID, m_larMiniFcalID);

    // Assign the volumes to the SD
    if( assignSD( sd.get(), parsedVolumes ).isFailure() ) {
      // TODO: can I just return NULL here?
      throw GaudiException("Failed to assign sd: " + sdName,
                           name(), StatusCode::FAILURE);
    }
    return std::move(sd);
  }

} // namespace LArG4
