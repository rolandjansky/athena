/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Class header
#include "ALFA_SensitiveDetector.h"

// Athena headers
#include "CxxUtils/make_unique.h" // For make unique

// Geant4 headers
#include "G4ParticleDefinition.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4TouchableHistory.hh"
#include "G4ThreeVector.hh"
#include "G4Track.hh"

// STL header
#include <sstream>
#include <limits>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ALFA_SensitiveDetector::ALFA_SensitiveDetector(const std::string& name, const std::string& hitCollectionName, const std::string& ODhitCollectionName)
  : G4VSensitiveDetector( name ),
    m_HitCollection(hitCollectionName),
    m_ODHitCollection(ODhitCollectionName)
{

  m_hitID = -1;

  m_eventNumber = 0;
  m_numberOfHits = 0;
  m_numberOfODHits = 0;

  m_pos1 = 0;
  m_pos2 = 0;

  m_num[0] = 0;
  m_num[1] = 0;
  m_num[2] = 0;

}

void ALFA_SensitiveDetector::StartOfAthenaEvent()
{
  m_numberOfHits = 0;
  m_numberOfODHits = 0;
}

// Initialize from G4 - necessary to new the write handle for now
void ALFA_SensitiveDetector::Initialize(G4HCofThisEvent *)
{
  if (!m_HitCollection.isValid()) m_HitCollection = CxxUtils::make_unique<ALFA_HitCollection>(m_HitCollection.name());
  if (!m_ODHitCollection.isValid()) m_ODHitCollection = CxxUtils::make_unique<ALFA_ODHitCollection>(m_ODHitCollection.name());
}

bool ALFA_SensitiveDetector::ProcessHits(G4Step* pStep, G4TouchableHistory*)
{
  //////ATH_MSG_DEBUG("ALFA_SensitiveDetector::ProcessHits" );

  const double energyDeposit(pStep->GetTotalEnergyDeposit());

  // Get kinetic energy of depositing particle

  const G4StepPoint *pPreStepPoint(pStep->GetPreStepPoint());
  const G4StepPoint *pPostStepPoint(pStep->GetPostStepPoint());

  const double kineticEnergy(pPreStepPoint->GetKineticEnergy());

  const G4ThreeVector preStepPoint(pPreStepPoint->GetPosition());
  const G4ThreeVector postStepPoint(pPostStepPoint->GetPosition());

  // Get name of physical volume
  const G4String vol_name(pPreStepPoint->GetTouchableHandle()->GetVolume()->GetName());

  const double preStepX(preStepPoint.x());
  const double preStepY(preStepPoint.y());
  const double preStepZ(preStepPoint.z());

  const double postStepX(postStepPoint.x());
  const double postStepY(postStepPoint.y());
  const double postStepZ(postStepPoint.z());

  int n_plate(0);
  int n_fiber(0);
  int n_station(0);

  m_hitID++;

  // particle encoding
  const G4Track *pTrack(pStep->GetTrack());
  const int particleEncoding(pTrack->GetDefinition()->GetPDGEncoding());

  const double globalTime(pTrack->GetGlobalTime());
  const int trackID(pTrack->GetTrackID()); //DC 2-29-04 use it as barcode

  std::string vol_test_str ("aaaaaaa");
  vol_test_str = vol_name.substr(0,7);

  ////ATH_MSG_DEBUG("test volume name is " << vol_test_str  );

  if(vol_name.find("GVS") != std::string::npos)
    {

      if(vol_name.find("B7L1") != std::string::npos) n_station=0;
      else if(vol_name.find("A7L1") != std::string::npos) n_station=1;
      else if(vol_name.find("A7R1") != std::string::npos) n_station=2;
      else if(vol_name.find("B7R1") != std::string::npos) n_station=3;
      else n_station=-1;
      if(m_HitCollection.isValid())
        {
          m_HitCollection->Emplace(m_hitID, trackID, particleEncoding, (float) kineticEnergy,
                                   (float) energyDeposit,(float) preStepX, (float) preStepY, (float) preStepZ,
                                   (float) postStepX, (float) postStepY, (float) postStepZ,(float) globalTime,
                                   -1, 100, -1, (int) n_station);
          m_numberOfHits++;
        }
      else
        {
          G4ExceptionDescription description;
          description << "ProcessHits: Can't access HitCollection with key " << m_HitCollection.key() << " from store " << m_HitCollection.store();
          G4Exception("ALFA_SensitiveDetector", "InvalidHitColl1", FatalException, description);
          return false; //The G4Exception call above should abort the job, but Coverity does not seem to pick this up.
        }
    }

  if (vol_test_str.compare("ALFA_Fi") == 0)
    {
      if (std::abs(energyDeposit)<std::numeric_limits<double>::epsilon()) { return true; }
      m_pos2 = 10;
      std::string substring (vol_name);
      std::string num_string (vol_name);

      ////ATH_MSG_DEBUG(" volume name is " << vol_test_str  );
      ////ATH_MSG_DEBUG("string slope is " << substring.substr(m_pos2,1)  );

      std::string test_str ("A");
      test_str = substring.substr(m_pos2,1);
      int sign_fiber(0);
      if (test_str.compare("U") == 0)
        {
          sign_fiber = 1;
          ////ATH_MSG_DEBUG("slope is "  << sign_fiber  );
        }

      if (test_str.compare("V") == 0)
        {
          sign_fiber = -1;
          ////ATH_MSG_DEBUG("slope is "  << sign_fiber  );
        }


      for ( int k = 0; k < 3; k++ )
        {
          substring = substring.substr(m_pos2+1);
          ////ATH_MSG_DEBUG("remaining string is " << substring  );
          m_pos1 = int(substring.find("["));
          ////ATH_MSG_DEBUG("position 1 is " << m_pos1  );
          m_pos2 = int(substring.find("]"));
          ////ATH_MSG_DEBUG("position 2 is " << m_pos1  );
          num_string = substring.substr(m_pos1+1,m_pos2-1);
          ////ATH_MSG_DEBUG("num_string is " << substring );
          std::istringstream is(num_string);
          is >> m_num[k];
          ////ATH_MSG_DEBUG("number got is " << m_num[k] );
        }

      n_station = m_num[0];
      n_plate   = m_num[1];
      n_fiber   = m_num[2];

      ////ATH_MSG_DEBUG("station=" << n_station << ", plate=" << n_plate << ", fiber=" << n_fiber << ", sign=" << sign_fiber );

      if(m_HitCollection.isValid())
        {
          m_HitCollection->Emplace(m_hitID,
                                   trackID,
                                   particleEncoding,
                                   (float) kineticEnergy,
                                   (float) energyDeposit,
                                   (float) preStepX, (float) preStepY, (float) preStepZ,
                                   (float) postStepX, (float) postStepY, (float) postStepZ,
                                   (float) globalTime,
                                   (int) sign_fiber, (int) n_plate, (int) n_fiber, (int) n_station
                                   );
          ++m_numberOfHits;
        }
      else
        {
          G4ExceptionDescription description;
          description << "ProcessHits: Can't access HitCollection with key " << m_HitCollection.key() << " from store " << m_HitCollection.store();
          G4Exception("ALFA_SensitiveDetector", "InvalidHitColl2", FatalException, description);
          return false; //The G4Exception call above should abort the job, but Coverity does not seem to pick this up.
        }
    }

  if (vol_test_str.compare("ODFiber") == 0)
    {
      if (std::abs(energyDeposit)<std::numeric_limits<double>::epsilon()) { return true; }
      m_pos2 = 7;
      std::string substring (vol_name);
      std::string num_string (vol_name);

      //////ATH_MSG_DEBUG(" volume name is " << vol_test_str  );
      //////ATH_MSG_DEBUG("string slope is " << substring.substr(m_pos2,1)  );

      std::string test_str ("A");
      test_str = substring.substr(m_pos2,1);
      int sign_fiber(0);
      if (test_str.compare("U") == 0)
        {
          sign_fiber = 1;
          //////ATH_MSG_DEBUG("slope is "  << sign_fiber  );
        }

      if (test_str.compare("V") == 0)
        {
          sign_fiber = -1;
          //////ATH_MSG_DEBUG("slope is "  << sign_fiber  );
        }

      std::string test_str_side ("A");
      test_str_side = substring.substr(m_pos2+1,1);

      //////ATH_MSG_DEBUG("remaining string is " << test_str_side );
      int OD_side(0);
      if (test_str_side.compare("0") == 0)
        {
          OD_side = 0;
          //////ATH_MSG_DEBUG("OD_side is "  << OD_side  );
        }

      if (test_str_side.compare("1") == 0)
        {
          OD_side = 1;
          //////ATH_MSG_DEBUG("OD_side is "  << OD_side  );
        }

      for ( int k = 0; k < 3; k++ )
        {
          substring = substring.substr(m_pos2+1);
          ////ATH_MSG_DEBUG("OD: remaining string is " << substring  );
          m_pos1 = int(substring.find("["));
          ////ATH_MSG_DEBUG("OD: position 1 is " << m_pos1  );
          m_pos2 = int(substring.find("]"));
          ////ATH_MSG_DEBUG("OD: position 2 is " << m_pos1  );
          num_string = substring.substr(m_pos1+1,m_pos2-1);
          ////ATH_MSG_DEBUG("OD: num_string is " << substring );
          std::istringstream is(num_string);
          is >> m_num[k];
          ////ATH_MSG_DEBUG("OD: number got is " << m_num[k] );
        }

      n_station = m_num[0];
      n_plate   = m_num[1];
      n_fiber   = m_num[2];

      ////ATH_MSG_DEBUG("station=" << n_station << ", side=" << OD_side << ", plate= "<< n_plate << ", fiber=" << n_fiber << ", sign=" << sign_fiber );
      if(m_ODHitCollection.isValid())
        {
          m_ODHitCollection->Emplace(m_hitID,
                                     trackID,
                                     particleEncoding,
                                     (float) kineticEnergy,
                                     (float) energyDeposit,
                                     (float) preStepX, (float) preStepY, (float) preStepZ,
                                     (float) postStepX, (float) postStepY, (float) postStepZ,
                                     (float) globalTime,
                                     (int) sign_fiber, (int) OD_side, (int) n_plate, (int) n_fiber, (int) n_station
                                     );
          ++m_numberOfODHits;
        }
      else
        {
          G4ExceptionDescription description;
          description << "ProcessHits: Can't access HitCollection with key " << m_ODHitCollection.key() << " from store " << m_ODHitCollection.store();
          G4Exception("ALFA_SensitiveDetector", "InvalidHitColl3", FatalException, description);
          return false; //The G4Exception call above should abort the job, but Coverity does not seem to pick this up.
        }

    }


  return true;
}

void ALFA_SensitiveDetector::EndOfAthenaEvent()
{
  G4cout << " Total number of hits in MD: " << m_numberOfHits << G4endl;
  G4cout << " Total number of hits in OD: " << m_numberOfODHits << G4endl;
  G4cout << "*************************************************************" << G4endl;

  ++m_eventNumber;

  m_numberOfHits = 0;
  m_numberOfODHits = 0;
}
