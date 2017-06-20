/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// Pixel Sensitive Detector.
// The Hits are processed here. For every hit I get the position and
// an information on the sensor in which the interaction happened
//

// Class header
#include "PixelSensorSD.h"

// Athena headers
#include "MCTruth/TrackHelper.h"

// Geant4 headers
#include "G4ChargedGeantino.hh"
#include "G4Geantino.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"

// CLHEP headers
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Units/SystemOfUnits.h"

// For make unique
#include "CxxUtils/make_unique.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PixelSensorSD::PixelSensorSD(const std::string& name, const std::string& hitCollectionName)
  : G4VSensitiveDetector( name )
  , m_HitColl( hitCollectionName )
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
// Initialize from G4 - necessary to new the write handle for now
void PixelSensorSD::Initialize(G4HCofThisEvent *)
{
  if (!m_HitColl.isValid()) m_HitColl = CxxUtils::make_unique<SiHitCollection>();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool PixelSensorSD::ProcessHits(G4Step* aStep, G4TouchableHistory* /*ROhist*/)
{
  if (verboseLevel>5) G4cout << "Process Hit" << G4endl;

  G4double edep = aStep->GetTotalEnergyDeposit();
  edep *= CLHEP::MeV;
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
  //
  // Get the hit coordinates. Start and End Point
  //
  G4ThreeVector coord1 = aStep->GetPreStepPoint()->GetPosition();
  G4ThreeVector coord2 = aStep->GetPostStepPoint()->GetPosition();

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
  //G4int History;
  //
  // Is it TB, barrel or endcap?
  //
  int BrlEcap = 0;
  int LayerDisk = 0;
  int etaMod = 0;
  int phiMod = 0;
  int side = 0;

  //   G4VPhysicalVolume* TheVolume = myTouch->GetVolume();
  //
  // In the case of the TB the positioning integers won't be initialized
  // and the identifying integer will be zero. There is no need to do
  // anything else

  int BEcopyNo =  myTouch->GetVolume()->GetCopyNo();

  // Barrel
  if(BEcopyNo == 100) {
    // Move up of one level in the tree to get the module number:
    //
    etaMod = myTouch->GetVolume(1)->GetCopyNo();
    //
    // Another level for the ladder
    //
    phiMod = myTouch->GetVolume(2)->GetCopyNo();
    //
    // Another one for the layer
    //
    LayerDisk = myTouch->GetVolume(3)->GetCopyNo();
    //
    // Some printout
    //
    if (verboseLevel>5){
      G4cout << "In the Pixel Barrel" << G4endl;
      G4cout << "----- Phi Module # " << phiMod << G4endl;
      G4cout << "----- Eta Ladder # " << etaMod << G4endl;
      G4cout << "----- Layer      # " << LayerDisk << G4endl;
    }

    // Standard ATLAS EndCap
  } else if (BEcopyNo == 200) {

    // Use the copy no. to get the id:
    phiMod = myTouch->GetVolume(1)->GetCopyNo();
    if (phiMod == 48) phiMod = 0;

    // etaMod always 0.

    // Move up of one level in the tree to get the disk number:
    LayerDisk = myTouch->GetVolume(2)->GetCopyNo();

    // Move up of one level in the tree to get the EC number:
    BrlEcap = myTouch->GetVolume(3)->GetCopyNo();
    //The following is no longer necessary
    //if(BrlEcap == -2) BrlEcap = 1;

    // Workaround for bug in cosmic setup
    if (BrlEcap == 0) {
      BrlEcap = (coord1.z() > 0) ? 2 : -2;
    }

    // Some printout
    if (verboseLevel>5){
      G4cout << "In the Pixel EndCap" << G4endl;
      G4cout << "----- PhiModule # " << phiMod << G4endl;
      G4cout << "----- Disk #      " << LayerDisk << G4endl;
      G4cout << "----- Endcap #    " << BrlEcap << G4endl;
    }

    // SLHC EndCap
  } else if (BEcopyNo == 300) {

    // Use the copy no. to get the id:
    phiMod = myTouch->GetVolume(1)->GetCopyNo();

    // Note: copyNo is 2*ieta for front face, 2*ieta+1 for back face
    etaMod = myTouch->GetVolume(2)->GetCopyNo()/2;

    // Move up of one level in the tree to get the disk number:
    LayerDisk = myTouch->GetVolume(3)->GetCopyNo();

    // Move up of one level in the tree to get the EC number:
    BrlEcap = myTouch->GetVolume(4)->GetCopyNo();
    //if(BrlEcap == -2) BrlEcap = 1;

    // Some printout
    if (verboseLevel>5){
      G4cout << "In the SLHC Pixel EndCap" << G4endl;
      G4cout << "----- PhiModule # " << phiMod << G4endl;
      G4cout << "----- Ring/Eta #  " << etaMod << G4endl;
      G4cout << "----- Disk #      " << LayerDisk << G4endl;
      G4cout << "----- Endcap #    " << BrlEcap << G4endl;
    }

  } else if(BEcopyNo == 400) { //DBM

    // Note: there is one volume (3-layers unit) contain the diamond+chip volume
    // so one increment is needed after 'layerDisk'

    // only one eta module for the DBM
    etaMod = 0;

    // Move up one level to get the layer number, 3 layers per telescope
    LayerDisk = myTouch->GetVolume(1)->GetCopyNo();

    // Move up two level to get the phi module, azimuthal position of telescope
    phiMod = myTouch->GetVolume(3)->GetCopyNo();

    // Move up one level to get the DBM side, A side or C side
    BrlEcap = myTouch->GetVolume(4)->GetCopyNo();

    // Some printout
    if (verboseLevel>5){
      G4cout << "In the DBM" << G4endl;
      G4cout << "----- PhiModule # " << phiMod << G4endl;
      G4cout << "----- Ring/Eta #  " << etaMod << G4endl;
      G4cout << "----- Disk #      " << LayerDisk << G4endl;
      G4cout << "----- Endcap #    " << BrlEcap << G4endl;
    }

  } else if(BEcopyNo == 500) { //Inclined

    std::string volName = myTouch->GetVolume()->GetName();

    // split the volname vs '_'
    std::replace(volName.begin(),volName.end(),'_',' ');

    std::vector<std::string> v;
    std::istringstream s(volName);
    std::string tmp;
    while (s >> tmp) {
      v.push_back(tmp);
    }

    BrlEcap = 0;   // no endcap defined in alpine
    LayerDisk = atoi(v[1].c_str());
    phiMod = atoi(v[2].c_str());
    etaMod = atoi(v[3].c_str());

    if (verboseLevel>5){
      G4cout << "Volume name " << volName <<G4endl;
      double xpos = coord1.x();
      double ypos = coord1.y();
      double zpos = coord1.z();
      double r = sqrt(xpos*xpos+ypos*ypos);
      G4cout << "In the Alpine " << G4endl;
      G4cout << "----- PhiModule # " << phiMod << G4endl;
      G4cout << "----- Ring/Eta #  " << etaMod << G4endl;
      G4cout << "----- Disk #      " << LayerDisk << G4endl;
      G4cout << "----- Endcap #    " << BrlEcap << G4endl;
      G4cout << "----- Pos        # " << r<<"  "<<zpos << G4endl;
      G4cout << "----- volume  " << myTouch->GetVolume()->GetName()<< G4endl;
      G4cout << "  " << myTouch->GetVolume(1)->GetName()<<" "<<
        myTouch->GetVolume(2)->GetName()<< "  " << myTouch->GetVolume(3)->GetName()<<G4endl;
    }

  } else if(BEcopyNo == 600) { //ITk ECring

    std::string volName = myTouch->GetVolume()->GetName();

    // split the volname vs '_'
    std::replace(volName.begin(),volName.end(),'_',' ');

    std::vector<std::string> v;
    std::istringstream s(volName);
    std::string tmp;
    while (s >> tmp) {
      v.push_back(tmp);
    }

    BrlEcap = atoi(v[1].c_str());
    LayerDisk = atoi(v[2].c_str());
    phiMod = atoi(v[3].c_str());
    etaMod = atoi(v[4].c_str());

    if (verboseLevel>5){
      double xpos = coord1.x();
      double ypos = coord1.y();
      double zpos = coord1.z();
      double r = sqrt(xpos*xpos+ypos*ypos);
      G4cout << "Volume name " << volName <<G4endl;
      G4cout << "In the ITk EC ring " << G4endl;
      G4cout << "----- PhiModule # " << phiMod << G4endl;
      G4cout << "----- Ring/Eta #  " << etaMod << G4endl;
      G4cout << "----- Disk #      " << LayerDisk << G4endl;
      G4cout << "----- Endcap #    " << BrlEcap << G4endl;
      G4cout << "----- Pos        # " << r<<"  "<<zpos << G4endl;
      G4cout << "----- volume  " << myTouch->GetVolume()->GetName()<< G4endl;
      G4cout << "  " << myTouch->GetVolume(1)->GetName()<<" "<<
        myTouch->GetVolume(2)->GetName()<< "  " << myTouch->GetVolume(3)->GetName()<<G4endl;
    }
  }
  else {
    // Do not expect other numbers. Need to fix PixelGeoModel if this occurs.
    G4ExceptionDescription description;
    description << "ProcessHits: Unrecognized geometry in Pixel sensitive detector. Please contact the maintainer of the Pixel Detector Description.";
    G4Exception("PixelSensorSD", "UnrecognizedPixelGeometry", FatalException, description);
    abort();
  }

  // get the HepMcParticleLink from the TrackHelper
  TrackHelper trHelp(aStep->GetTrack());
  m_HitColl->Emplace(lP1,
                     lP2,
                     edep,
                     aStep->GetPreStepPoint()->GetGlobalTime(),
                     trHelp.GetParticleLink(),
                     0,BrlEcap,LayerDisk,etaMod,phiMod,side);
  return true;
}
