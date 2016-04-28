/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// HGTD Sensitive Detector.
// The Hits are processed here. For every hit I get the position and
// an information on the sensor in which the interaction happened
//

// Class header
#include "HGTDSensorSD.h"

// Athena headers
#include "MCTruth/TrackHelper.h"

// Geant4 headers
#include "G4ChargedGeantino.hh"
#include "G4Geantino.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"


#include "LArSimEvent/LArHitContainer.h"

// CLHEP headers
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include <stdint.h>
#include <string.h>

// For make unique
#include "CxxUtils/make_unique.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HGTDSensorSD::HGTDSensorSD(const std::string& name, std::string timeBinType, double timeBinWidth, double timeWindowMin, double timeWindowMax)
  : G4VSensitiveDetector( name ), 
    m_timeBinType(timeBinType), 
    m_timeBinWidth(timeBinWidth),
    m_timeWindowMin(timeWindowMin),
    m_timeWindowMax(timeWindowMax)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
// Initialize from G4 - necessary to new the write handle for now
void HGTDSensorSD::Initialize(G4HCofThisEvent *)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool HGTDSensorSD::ProcessHits(G4Step* aStep, G4TouchableHistory* /*ROhist*/)
{
  if (verboseLevel>5) G4cout << "Process Hit" << G4endl;

  G4double edep = aStep->GetTotalEnergyDeposit();
  edep *= CLHEP::MeV;

  //  double double_edep = aStep->GetTotalEnergyDeposit();

  if(edep==0.) {
    if(aStep->GetTrack()->GetDefinition() != G4Geantino::GeantinoDefinition() &&
       aStep->GetTrack()->GetDefinition() != G4ChargedGeantino::ChargedGeantinoDefinition())
      return false;
  }

  //use the global time. i.e. the time from the beginning of the event
  //
  // Get the Touchable History:
  //
  G4TouchableHistory*  myTouch = (G4TouchableHistory*)(aStep->GetPreStepPoint()->GetTouchable());

  if(verboseLevel>5){
    for (int i=0;i<myTouch->GetHistoryDepth();i++){
      std::string detname=myTouch->GetVolume(i)->GetLogicalVolume()->GetName();
      int copyno=myTouch->GetVolume(i)->GetCopyNo();
      G4cout << "Volume " <<detname <<" Copy Nr. " << copyno << G4endl;
    }
  }

  std::string detname_layer=myTouch->GetVolume(0)->GetLogicalVolume()->GetName();

  //
  // Get the hit coordinates. Start and End Point
  //
  G4ThreeVector startCoord = aStep->GetPreStepPoint()->GetPosition();
  G4ThreeVector endCoord   = aStep->GetPostStepPoint()->GetPosition();
  //get the average position for writing
  G4ThreeVector globalPosition = (startCoord + endCoord)/2;
  //  G4double TrackTotalEnergy = aStep->GetTrack()->GetTotalEnergy();
  // TrackTotalEnergy *= CLHEP::MeV;

  // Calculate the local step begin and end position.
  // From a G4 FAQ:
  // http://geant4-hn.slac.stanford.edu:5090/HyperNews/public/get/geometry/17/1.html
  // This transformation allows to get the local position inside the physical volume
  // and with respect to it. This is safer for the definition of the cell index

  const G4AffineTransform transformation = myTouch->GetHistory()->GetTopTransform();
  G4ThreeVector position = transformation.TransformPoint(globalPosition);
  /* G4ThreeVector localPosition2 = transformation.TransformPoint(end);

  int BEcopyNo =  myTouch->GetVolume()->GetCopyNo(); 

  float timing=aStep->GetPreStepPoint()->GetGlobalTime(); */

  /*std::cout << " Hit Layer: " <<  detname_layer << std::endl;
  std::cout << " timing " << timing << std::endl;
  std::cout << "TrackTotalEnergy  " << TrackTotalEnergy << std::endl;
  std::cout << "TotalEnergyDeposit  " << edep << std::endl;
  
  std::cout << " PreStepPoint " <<  std::endl;
  std::cout << " x (global/local) " << startCoord.x()*CLHEP::mm << " " << localPosition1[0]*CLHEP::mm << std::endl;
  std::cout << " y (global/local) " << startCoord.y()*CLHEP::mm << " " << localPosition1[1]*CLHEP::mm << std::endl;
  std::cout << " z (global/local) " << startCoord.z()*CLHEP::mm << " " << localPosition1[2]*CLHEP::mm << std::endl;   
  
  std::cout << " PostStepPoint: " <<  std::endl;
  std::cout << " x (global/local) " << endCoord.x()*CLHEP::mm << " " << localPosition2[0]*CLHEP::mm << std::endl;
  std::cout << " y (global/local) " << endCoord.y()*CLHEP::mm << " " << localPosition2[1]*CLHEP::mm << std::endl;
  std::cout << " z (global/local) " << endCoord.z()*CLHEP::mm << " " << localPosition2[2]*CLHEP::mm << std::endl; */
  
  // the new LArHit stuff
  // from the timing we need to subtract the TOF to avoid 0 bins:
  double absoluteTime = aStep->GetPreStepPoint()->GetGlobalTime();
  // double theTime      = getTruncatedTime(absoluteTime);
  double theTime = getMinSubtractedMaxTruncatedTime(absoluteTime);

  // now deal with the Identifier
  int posNegEndcap = ( globalPosition.z() > 0. ? 1 : -1 );

  // make sure that it's a Sensor:
  if ( detname_layer.find("Sensor") == std::string::npos ) {
    // Do not expect other names. Need to fix HGTDGeoModel if this occurs.
    G4ExceptionDescription description;
    description << "ProcessHits: No HGTD sensitive detector with substring Sensor found. Check HGTD Detector Description.";
    G4Exception("HGTDSensorSD", "UnrecognizedHGTDGeometry", FatalException, description);
    abort();
  } 
  // Define the Silicon layer from the index
  int sampling = atoi(detname_layer.substr(detname_layer.size()-1,1).c_str());
  // we are at G4 level, so we want the coarse granularity = 0
  int granularity = 0;
  // now the x direction:
  int xIndex = (position.x() > 0. ? 1 : -1)*(fabs(position.x())/0.5+1);
  // then the y direction:
  int yIndex = (position.y() > 0. ? 1 : -1)*(fabs(position.y())/0.5+1);

  // now the identifier:
  Identifier id = m_hgtdID -> channel_id(posNegEndcap, sampling, granularity, xIndex, yIndex );

  // and the energy
  double energy = edep;

  // we can now put the stuff into the collection:
  SimpleHit(id,theTime,energy); 

  return true;
}

void HGTDSensorSD::EndOfAthenaEvent( LArHitContainer * hitContainer )
{
  // For each time bin...
  for(const auto i : m_timeBins) {
    if (verboseLevel>5)
      G4cout << "EndOfEvent: time bin " << i.first << " - #hits = " << i.second->size() << G4endl;

    const hits_t* hitSet = i.second;
      
    // For each hit in the set...
    for(auto hit : *hitSet){
      // Because of the design, we are sure this is going into the right hit container
      hit->finalize();
      hitContainer->push_back(hit);
    } // End of loop over hits in the set

  } // End of loop over time bins
    
  for(auto i : m_timeBins) delete i.second;
  m_timeBins.clear();
}

double HGTDSensorSD::getTruncatedTime(G4double time) const {

  // we need to be between a minimal and a maximal time:
  // G4 works in ns
  // to do: these two values should be jobProperties
  //  G4double minTime = 3480./CLHEP::c_light/CLHEP::ns;
  //  G4double maxTime = 3600./CLHEP::c_light/CLHEP::ns;
  
  G4double truncatedTime = 0.;
  if ( time < m_timeWindowMin ) {
    truncatedTime = 0.;
  }
  else if ( time > m_timeWindowMax ) {
    truncatedTime = m_timeWindowMax - m_timeWindowMin;
  }
  else {
    // default calculation
    truncatedTime  = time - m_timeWindowMin;
  }

  return truncatedTime;
} 

double HGTDSensorSD::getMinSubtractedMaxTruncatedTime(G4double time) const {
  
  G4double subtractedTime = 0.;
  if ( time > m_timeWindowMax ) {
    subtractedTime = m_timeWindowMax - m_timeWindowMin;
  }
  else {
    // default calculation
    subtractedTime  = time - m_timeWindowMin;
  }

  return subtractedTime;
} 

G4bool HGTDSensorSD::SimpleHit( Identifier id , G4double time , G4double energy )
{
  // Build the hit from the calculator results.
  /*  Identifier id = ConvertID( lar_id );
      if (!id.is_valid()) return false; */

  G4int timeBin = getTimeBin( time );

  // Find the set of hits for this time bin.  If this is the
  // first hit in this bin, create a new set.
    
  hits_t* hitCollection = 0;
  auto setForThisBin = m_timeBins.find( timeBin );
    
  if (setForThisBin == m_timeBins.end()) {
    // New time bin
    hitCollection = new hits_t;
    m_timeBins[ timeBin ] = hitCollection;
  } else {
    // Get the existing set of hits for this time bin.
    // Reminders:
    // setForThisBin = iterator (pointer) into the m_timeBins map
    // (*setForThisBin) = pair< G4int, m_hits_t* >
    // (*setForThisBin).second = m_hits_t*, the pointer to the set of hits
        
    hitCollection = (*setForThisBin).second;
  }
    
  LArHit* hit = new LArHit(id,energy,time);

  // If we haven't had a hit in this cell before, create one and add
  // it to the hit collection.
    
  // If we've had a hit in this cell before, then add the energy to
  // the existing hit.
    
  // Look for the key in the hitCollection (this is a binary search).
  auto bookmark = hitCollection->lower_bound(hit);
    
  // The lower_bound method of a map finds the first element
  // whose key is not less than the identifier.  If this element
  // == our hit, we've found a match.
    
  // Reminders:
  // bookmark = iterator (pointer) into the hitCollection set.
  // (*bookmark) = a member of the set, which is a LArG4Hit*.
    
  // Equals() is a function defined in LArG4Hit.h; it has the value of
  // "true" when a LArG4Hit* points to the same identifier.
    
  if (bookmark == hitCollection->end() ||
      !(*bookmark)->Equals(hit)) {
    // We haven't had a hit in this readout cell before.  Add it
    // to our set.
    if (hitCollection->empty() ||
        bookmark == hitCollection->begin()) {
      // Insert the hit before the first entry in the map.
      hitCollection->insert(hit);
    } else {
      // We'just done a binary search of hitCollection, so we should use
      // the results of that search to speed up the insertion of a new
      // hit into the map.  The "insert" method is faster if the new
      // entry is right _after_ the bookmark.  If we left bookmark
      // unchanged, the new entry would go right _before_ the
      // bookmark.  We therefore want to decrement the bookmark from
      // the lower_bound search.
            
      hitCollection->insert(--bookmark, hit);
    }
  } else {
    // Update the existing hit.
    (*bookmark)->Add(hit);
        
    // We don't need our previously-created hit anymore.
    delete hit;
  }
  
  return true;
}

G4int HGTDSensorSD::getTimeBin(G4double time) const {
 
  G4int timeBin = INT_MAX;
    
  if(m_timeBinType != "Uniform" )
    {
      if(time<0.)
	time=0.;
      if(time<10.)
	timeBin = int(time/2.5);
      else if(time<50.)
	timeBin = 4 + int((time-10.)/5.);
      else if(time<100.)
	timeBin = 12 + int((time-50.)/25.);
      else
	timeBin = 14;
    }
  else {
    // Uniform binning by 2.5 ns
    G4double timeBinf = time/m_timeBinWidth;
    
    if (timeBinf < G4double(INT_MAX))
      timeBin = G4int(timeBinf);
  }
  
  return timeBin;
} 
