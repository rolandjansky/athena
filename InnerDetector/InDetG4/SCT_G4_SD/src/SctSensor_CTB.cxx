/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//
// SCT Sensitive Detector.
// The Hits are processed here. For every hit I get the position and
// an information on the sensor in which the interaction happened
//
//#define SCTSD_CTB_DEBUG

// class header
#include "SctSensor_CTB.h"

// athena includes
#include "MCTruth/TrackHelper.h"

// Geant4 includes
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4Geantino.hh"
#include "G4ChargedGeantino.hh"

// CLHEP transform
#include "CLHEP/Geometry/Transform3D.h"

// For make unique
#include "CxxUtils/make_unique.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SctSensor_CTB::SctSensor_CTB(const std::string& name, const std::string& hitCollectionName)
  : G4VSensitiveDetector(name)
  , m_HitColl(hitCollectionName)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SctSensor_CTB::Initialize(G4HCofThisEvent *)
{
  if (!m_HitColl.isValid()) m_HitColl = CxxUtils::make_unique<SiHitCollection>();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool SctSensor_CTB::ProcessHits(G4Step* aStep, G4TouchableHistory* /*ROhist*/)
{
  G4double edep = aStep->GetTotalEnergyDeposit();
  if(edep==0.){
    if(aStep->GetTrack()->GetDefinition()!=G4Geantino::GeantinoDefinition() &&
       aStep->GetTrack()->GetDefinition()!=G4ChargedGeantino::ChargedGeantinoDefinition())
      return false;
  }
  edep *= CLHEP::MeV;
  //use the global time. i.e. the time from the beginning of the event
  //
  // Get the Touchable History:
  //
  const G4TouchableHistory*  myTouch = dynamic_cast<const G4TouchableHistory*>(aStep->GetPreStepPoint()->GetTouchable());
  //
  // Get the hit coordinates. Start and End Point
  //
  G4ThreeVector coord1 = aStep->GetPreStepPoint()->GetPosition();
  G4ThreeVector coord2 = aStep->GetPostStepPoint()->GetPosition();
  //
  // Calculate the local step begin and end position.
  // From a G4 FAQ:
  // http://geant4-hn.slac.stanford.edu:5090/HyperNews/public/get/geometry/17/1.html
  //
  const G4AffineTransform transformation = myTouch->GetHistory()->GetTopTransform();
  G4ThreeVector localPosition1 = transformation.TransformPoint(coord1);
  G4ThreeVector localPosition2 = transformation.TransformPoint(coord2);

  HepGeom::Point3D<double> lP1,lP2;
  lP1[SiHit::xEta] = localPosition1[2]*CLHEP::mm;
  lP1[SiHit::xPhi] = localPosition1[1]*CLHEP::mm;
  lP1[SiHit::xDep] = localPosition1[0]*CLHEP::mm;

  lP2[SiHit::xEta] = localPosition2[2]*CLHEP::mm;
  lP2[SiHit::xPhi] = localPosition2[1]*CLHEP::mm;
  lP2[SiHit::xDep] = localPosition2[0]*CLHEP::mm;

  //
  // Get it into a vector in local coords and with the right units:


  // Now Navigate the history to know in what detector the step is:
  // and finally set the ID of det element in which the hit is.
  //
  // G4int History;
  //
  // Is it TB, barrel or endcap?
  //
  int BrlEcap = 0;
  int LayerDisk = 0;
  int etaMod = 1;
  int phiMod = 0;
  int side = 0;
  //
  // In the case of the TB the positioning integers won't be initialized
  // and the identifying integer will be zero. There is no need to do
  // anything else

  int BEcopyNo =  myTouch->GetVolume()->GetCopyNo();

#ifdef SCTSD_CTB_DEBUG
  for (int i=0;i<myTouch->GetHistoryDepth();i++){
    std::string detname=myTouch->GetVolume(i)->GetLogicalVolume()->GetName();
    int copyno=myTouch->GetVolume(i)->GetCopyNo();
    if (verboseLevel>1) G4cout << "Volume "<<detname <<" Copy Nr. "<<copyno << G4endl;
  }
#endif

  // Testbeam geometry from GeoModel.
  //
  int BEcopyNoTest =  BEcopyNo/100;
  if(BEcopyNoTest == 5 || BEcopyNoTest == 6 || BEcopyNo == 1000 || BEcopyNo == 2000) {
    BrlEcap=0;
    etaMod=1;

    if(BEcopyNoTest == 5 || BEcopyNoTest == 6 ) {
      side      = myTouch->GetVolume(0)->GetCopyNo()%2;
      phiMod=myTouch->GetVolume(1)->GetCopyNo()%2;
      LayerDisk = myTouch->GetVolume(1)->GetCopyNo()/2;
    } else if ( BEcopyNo == 1000) {
      side      = myTouch->GetVolume(1)->GetCopyNo();
      phiMod=myTouch->GetVolume(2)->GetCopyNo()%2;
      LayerDisk = myTouch->GetVolume(2)->GetCopyNo()/2;
    } // otherwise use side = phiMod = LayerDisk = 0.
    // Some printout
#ifdef SCTSD_CTB_DEBUG
    if (verboseLevel>1) G4cout << "In the SCT TestBeam" << G4endl;
    if (verboseLevel>1) G4cout << " * Module Number: " << LayerDisk << "," << phiMod << " Side: " << side << G4endl;
    if (verboseLevel>5) G4cout << " Identifier will be [2.2."
                     << BrlEcap << "."
                     << LayerDisk << "."
                     << phiMod << "."
                     << etaMod << "."
                     << side << "]" << G4endl;
#endif
  } else {
    G4ExceptionDescription description;
    description << "ProcessHits: Unexpected copy number for sensor";
    G4Exception("SctSensor_CTB", "UnexpectedCopyNumberForSctSensor", FatalException, description);
    abort();
  }
  TrackHelper trHelp(aStep->GetTrack());
  m_HitColl->Emplace(lP1,
                     lP2,
                     edep,
                     aStep->GetPreStepPoint()->GetGlobalTime(),
                     trHelp.GetParticleLink(),
                     1,BrlEcap,LayerDisk,etaMod,phiMod,side);
  return true;
}
