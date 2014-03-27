/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_G4_SD/ALFA_SensitiveDetector.h"

#include "FadsSensitiveDetector/SensitiveDetectorEntryT.h"
#include "Randomize.hh" 
#include "G4HCofThisEvent.hh"
#include "G4TouchableHistory.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"
#include "G4ParticleDefinition.hh"
#include "G4StepPoint.hh"
#include "G4ThreeVector.hh"

#include <sstream>
#include <iostream>
#include "AthenaBaseComps/AthMsgStreamMacros.h"

// Called by FADS/Goofy, where is, to the hell, a documentation?
  
static FADS::SensitiveDetectorEntryT<ALFA_SensitiveDetector> sd("ALFA_SensitiveDetector");


ALFA_SensitiveDetector::ALFA_SensitiveDetector(std::string name): FADS::FadsSensitiveDetector(name) {
  
  ATH_MSG_VERBOSE(" ALFA_SensitiveDetector::Constructor ");
  
  pHitCollection = 0;
  pODHitCollection = 0;
  
  hitID = -1;
  trackID = -1;
  particleEncoding = 0;
  kineticEnergy = 0;
  energyDeposit = 0;
  preStepX = 0;
  preStepY = 0;
  preStepZ = 0;
  postStepX = 0;
  postStepY = 0;
  postStepZ = 0;
  globalTime = 0;

  sign_fiber = 0;
  OD_side = 0;
  n_plate = 0;
  n_fiber = 0;
  n_station = 0;

  eventNumber = 0;
  numberOfHits = 0;
  numberOfODHits = 0;

  pos1 = 0;
  pos2 = 0;

  num[0] = 0;
  num[1] = 0;
  num[2] = 0;
}

ALFA_SensitiveDetector::~ALFA_SensitiveDetector() {
  
  ATH_MSG_VERBOSE(" ALFA_SensitiveDetector::Destructor ");
}
  
void ALFA_SensitiveDetector::Initialize(G4HCofThisEvent*) {
  
  pHitCollection = new ALFA_HitCollection("ALFA_HitCollection"); 
  pODHitCollection = new ALFA_ODHitCollection("ALFA_ODHitCollection");
    
  eventNumber = 0;
  numberOfHits = 0;
  numberOfODHits = 0;
}

bool ALFA_SensitiveDetector::ProcessHits(G4Step* pStep, G4TouchableHistory*)
{
  ATH_MSG_DEBUG("ALFA_SensitiveDetector::ProcessHits" );
 
  energyDeposit = pStep->GetTotalEnergyDeposit();  
  
  if (energyDeposit==0.) return true; 
  
  // Get kinetic energy of depositing particle
  
  G4StepPoint* pPreStepPoint = pStep->GetPreStepPoint();  
  G4StepPoint* pPostStepPoint = pStep->GetPostStepPoint();
   
  kineticEnergy = pPreStepPoint->GetKineticEnergy();
  
  G4ThreeVector preStepPoint = pPreStepPoint->GetPosition();
  G4ThreeVector postStepPoint = pPostStepPoint->GetPosition();
 
  // Get name of physical volume   
  G4TouchableHandle touch1 = pPreStepPoint->GetTouchableHandle();
  G4VPhysicalVolume* volume = touch1->GetVolume();
  G4String vol_name = volume->GetName(); 
  //  G4LogicalVolume* lVolume = volume->GetLogicalVolume();

  preStepX = preStepPoint.x();
  preStepY = preStepPoint.y();  
  preStepZ = preStepPoint.z();  
  
  postStepX = postStepPoint.x();
  postStepY = postStepPoint.y();  
  postStepZ = postStepPoint.z();   
  
  hitID++;  
  
  // particle encoding
  G4Track* pTrack = pStep->GetTrack();
  G4ParticleDefinition* pParticleDefinition = pTrack->GetDefinition();
  particleEncoding = pParticleDefinition->GetPDGEncoding();

  globalTime = pTrack->GetGlobalTime(); 
  trackID=pTrack->GetTrackID();
  
  std::string vol_test_str ("aaaaaaa");
  vol_test_str = vol_name.substr(0,7);    

  ATH_MSG_DEBUG("test volume name is " << vol_test_str  );
    
  if (vol_test_str.compare("ALFA_Fi") == 0)
    {
       
      pos2 = 10;
      std::string substring (vol_name);
      std::string num_string (vol_name);
  
      ATH_MSG_DEBUG(" volume name is " << vol_test_str  );
      ATH_MSG_DEBUG("string slope is " << substring.substr(pos2,1)  );
  
      std::string test_str ("A");  
      test_str = substring.substr(pos2,1);
  
      if (test_str.compare("U") == 0)
	{
	  sign_fiber = 1;
	  ATH_MSG_DEBUG("slope is "  << sign_fiber  );
	}
  
      if (test_str.compare("V") == 0)
	{
	  sign_fiber = -1;
	  ATH_MSG_DEBUG("slope is "  << sign_fiber  );
	}    
  
      
      for ( int k = 0; k < 3; k++ )
	{
	  substring = substring.substr(pos2+1);
	  ATH_MSG_DEBUG("remaining string is " << substring  );
	  pos1 = int(substring.find("["));
	  ATH_MSG_DEBUG("position 1 is " << pos1  );
	  pos2 = int(substring.find("]"));
	  ATH_MSG_DEBUG("position 2 is " << pos1  );	          
	  num_string = substring.substr(pos1+1,pos2-1);
	  ATH_MSG_DEBUG("num_string is " << substring );	     
	  std::istringstream is(num_string);
	  is >> num[k];
	  ATH_MSG_DEBUG("number got is " << num[k] );	
	}
  
      n_station = num[0];  
      n_plate   = num[1];
      n_fiber   = num[2];

      ATH_MSG_DEBUG("station=" << n_station << ", plate=" << n_plate << ", fiber=" << n_fiber << ", sign=" << sign_fiber );
     
      ALFA_Hit* pHit = new ALFA_Hit(hitID,
				    trackID, 
				    particleEncoding,  
				    (float) kineticEnergy,
				    (float) energyDeposit, 
				    (float) preStepX, (float) preStepY, (float) preStepZ, 
				    (float) postStepX, (float) postStepY, (float) postStepZ, 
				    (float) globalTime, 
				    (int) sign_fiber, (int) n_plate, (int) n_fiber, (int) n_station
				    );
      
      pHitCollection->Insert(*pHit);
      ++numberOfHits;
    }

  if (vol_test_str.compare("ODFiber") == 0)
    {

      pos2 = 7;
      std::string substring (vol_name);
      std::string num_string (vol_name);
  
      ATH_MSG_DEBUG(" volume name is " << vol_test_str  );
      ATH_MSG_DEBUG("string slope is " << substring.substr(pos2,1)  );  
     
      std::string test_str ("A");  
      test_str = substring.substr(pos2,1);
  
      if (test_str.compare("U") == 0)
	{
	  sign_fiber = 1;
	  ATH_MSG_DEBUG("slope is "  << sign_fiber  );      
	}
  
      if (test_str.compare("V") == 0)
	{
	  sign_fiber = -1;
	  ATH_MSG_DEBUG("slope is "  << sign_fiber  );          
	}    

      std::string test_str_side ("A");  
      test_str_side = substring.substr(pos2+1,1);
            
      ATH_MSG_DEBUG("remaining string is " << test_str_side );   
	     
      if (test_str_side.compare("0") == 0)
	{
	  OD_side = 0;
	  ATH_MSG_DEBUG("OD_side is "  << OD_side  ); 	      
	}
  
      if (test_str_side.compare("1") == 0)
	{
	  OD_side = 1;
	  ATH_MSG_DEBUG("OD_side is "  << OD_side  ); 	          
	}         
      
      for ( int k = 0; k < 3; k++ )
	{
	  substring = substring.substr(pos2+1);
	  ATH_MSG_DEBUG("OD: remaining string is " << substring  );        
	  pos1 = int(substring.find("["));
	  ATH_MSG_DEBUG("OD: position 1 is " << pos1  );        
	  pos2 = int(substring.find("]"));
	  ATH_MSG_DEBUG("OD: position 2 is " << pos1  );        
	  num_string = substring.substr(pos1+1,pos2-1);
	  ATH_MSG_DEBUG("OD: num_string is " << substring );	        
	  std::istringstream is(num_string);
	  is >> num[k];
	  ATH_MSG_DEBUG("OD: number got is " << num[k] );       
	}
  
      n_station = num[0];  
      n_plate   = num[1];
      n_fiber   = num[2];
    
      ATH_MSG_DEBUG("station=" << n_station << ", side=" << OD_side << ", plate= "<< n_plate << ", fiber=" << n_fiber << ", sign=" << sign_fiber );
           
      ALFA_ODHit* pODHit = new ALFA_ODHit(hitID,
					  trackID, 
					  particleEncoding,  
					  (float) kineticEnergy,
					  (float) energyDeposit, 
					  (float) preStepX, (float) preStepY, (float) preStepZ, 
					  (float) postStepX, (float) postStepY, (float) postStepZ, 
					  (float) globalTime, 
					  (int) sign_fiber, (int) OD_side, (int) n_plate, (int) n_fiber, (int) n_station
					  );
      
      pODHitCollection->Insert(*pODHit);
      ++numberOfODHits;
    }
  
  return true;
}

void ALFA_SensitiveDetector::EndOfEvent(G4HCofThisEvent* ) {  
     
  ATH_MSG_DEBUG(" Total number of hits in MD: " << numberOfHits  );  
  ATH_MSG_DEBUG(" Total number of hits in OD: " << numberOfODHits );  
  ATH_MSG_DEBUG("*************************************************************" );  
  
  m_hitCollHelp.ExportCollection<ALFA_HitCollection>(pHitCollection); 
  m_hitCollHelp.ExportCollection<ALFA_ODHitCollection>(pODHitCollection);   
   
  ++eventNumber;
    
  numberOfHits = 0;
  numberOfODHits = 0;
}

