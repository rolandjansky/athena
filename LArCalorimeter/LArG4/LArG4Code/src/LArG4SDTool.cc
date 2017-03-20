/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4Code/LArG4SDTool.h"

// For the ID helpers
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArMiniFCAL_ID.h"
#include "CaloIdentifier/CaloDM_ID.h"
#include "LArG4Code/LArG4SimpleSD.h"
#include "LArG4Code/LArG4CalibSD.h"

// Geant4 headers
#include "G4LogicalVolumeStore.hh"
#include "G4SDManager.hh"
#include "G4VSensitiveDetector.hh"

// Units!
#include "CLHEP/Units/SystemOfUnits.h"


LArG4SDTool::LArG4SDTool(const std::string& type, const std::string& name, const IInterface *parent)
  : SensitiveDetectorBase(type,name,parent)
  , m_doPID(false)
  , m_timeBinType("Default")
  , m_timeBinWidth(2.5*CLHEP::ns)
  , m_larEmID (nullptr)
  , m_larFcalID (nullptr)
  , m_larHecID (nullptr)
  , m_larMiniFcalID (nullptr)
  , m_caloDmID (nullptr)
{
  declareProperty("ParticleID",m_doPID=false);
  declareProperty("TimeBinType",m_timeBinType);
  declareProperty("TimeBinWidth",m_timeBinWidth);
  declareProperty("UseFrozenShowers",m_useFrozenShowers);
}

StatusCode LArG4SDTool::initialize()
{
  const CaloIdManager* caloIdManager=nullptr;
  ATH_CHECK( detStore()->retrieve(caloIdManager) );
  m_larEmID = caloIdManager->getEM_ID();
  if(m_larEmID==0)
    {
      ATH_MSG_ERROR("LArG4SDTool: Invalid LAr EM ID helper");
      return StatusCode::FAILURE;
    }
  m_larFcalID = caloIdManager->getFCAL_ID();
  if(m_larFcalID==0)
    {
      ATH_MSG_ERROR("LArG4SDTool: Invalid FCAL ID helper");
      return StatusCode::FAILURE;
    }
  m_larHecID = caloIdManager->getHEC_ID();
  if(m_larHecID==0)
    {
      ATH_MSG_ERROR("LArG4SDTool: Invalid HEC ID helper");
      return StatusCode::FAILURE;
    }
  m_larMiniFcalID = caloIdManager->getMiniFCAL_ID();
  if(m_larMiniFcalID==0)
    {
      ATH_MSG_ERROR("LArG4SDTool: Invalid Mini FCAL ID helper");
      return StatusCode::FAILURE;
    }
  m_caloDmID = caloIdManager->getDM_ID();
  if(!m_caloDmID)
    {
      ATH_MSG_ERROR("LArG4SDTool: Invalid CaloDM ID helper");
      return StatusCode::FAILURE;
    }
  ATH_CHECK(this->initializeCalculators());

  return StatusCode::SUCCESS;
}

void LArG4SDTool::setupHelpers( LArG4SimpleSD* an_sd ) const
{
  an_sd->setupHelpers( m_larEmID, m_larFcalID, m_larHecID, m_larMiniFcalID );
}

void LArG4SDTool::setupHelpers( LArG4CalibSD* an_sd ) const
{
  an_sd->setupHelpers( m_larEmID, m_larFcalID, m_larHecID, m_larMiniFcalID, m_caloDmID );
}

void LArG4SDTool::setupAllSDs(std::map<G4VSensitiveDetector*,std::vector<std::string>*> configuration) const
{
  //  // Grab the user detector construction
  //  G4RunManager* rm = G4RunManager::GetRunManager();
  //  G4VUserDetectorConstruction* dc=rm->GetUserDetectorConstruction();

  for (auto iter : configuration){
    G4VSensitiveDetector* anSD = iter.first;

    // Set the verbosity information on this thing
    if(msgLvl(MSG::VERBOSE)) anSD->SetVerboseLevel(10);
    else if(msgLvl(MSG::DEBUG)) anSD->SetVerboseLevel(5);

    if(iter.second->empty())
      {
        ATH_MSG_VERBOSE ("No volumes associated with " << anSD->GetName() << ", so just adding it to the SDManger.");
        G4SDManager* SDmanager = G4SDManager::GetSDMpointer();
        SDmanager->AddNewDetector(anSD);
        continue;
      }
    // Go through the logical volumes and hook the SDs up
    G4LogicalVolumeStore * logicalVolumeStore = G4LogicalVolumeStore::GetInstance();
    for (auto myvol : *(iter.second)){
      int found = 0;
      for (auto ilv : *logicalVolumeStore ){
        if ( match(myvol.data(),ilv->GetName() ) ){
          ++found; // Do not break on found to protect against multiple volumes with the same name
          //dc->SetSensitiveDetector( ilv , anSD );
          SetSensitiveDetector( ilv , anSD ); //getSD() );
        } // Found a volume!
      } // Loop over all the volumes in the geometry
      // Give notice if we have missed a volume in here
      if (0==found){
        ATH_MSG_WARNING( "Volume " << myvol << " not found in G4LogicalVolumeStore." );
      } else {
        ATH_MSG_VERBOSE( found << " copies of LV " << myvol << " found; SD " << name() << " assigned." );
      }
    } // Loop over my volumes
  } // Done going through all the SDs

}

// NOTE: there has GOT to be a non-custom solution to this!
// How about POSIX glob? Or std regex?

// Dear internet: thank you, you provide endless solutions to problems
// A C program to match wild card characters
// The main function that checks if two given strings match. The first
// string may contain wildcard characters
bool LArG4SDTool::match(const char *first, const char * second) const
{
  // If we reach at the end of both strings, we are done
  if (*first == '\0' && *second == '\0')
    return true;

  // Make sure that the characters after '*' are present in second string.
  // This function assumes that the first string will not contain two
  // consecutive '*'
  if (*first == '*' && *(first+1) != '\0' && *second == '\0')
    return false;

  // If the current characters of both strings match
  if (*first == *second)
    return match(first+1, second+1);

  // If there is *, then there are two possibilities
  // a) We consider current character of second string
  // b) We ignore current character of second string.
  if (*first == '*')
    return match(first+1, second) || match(first, second+1);
  return false;
}
