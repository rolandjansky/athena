/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// Pixel Sensitive Detector.
// The Hits are processed here. For every hit I get the position and
// an information on the sensor in which the interaction happened
//
#include <fstream>
#include "FadsSensitiveDetector/SensitiveDetectorEntryT.h"
#include "FadsSensitiveDetector/SensitiveDetectorCatalog.h"

#include "SimHelpers/AthenaHitsCollectionHelper.h"
#include "InDetSimEvent/SiHitCollection.h"

// G4Pixel 
#include "PixelG4_SD/PixelSensorSD.h"

// Geant4 Stuff
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4Geantino.hh"
#include "G4ChargedGeantino.hh"


// CLHEP transform
#include "CLHEP/Geometry/Transform3D.h"    
// Units
#include "CLHEP/Units/SystemOfUnits.h"    

#include "SimHelpers/DetectorGeometryHelper.h"
#include "MCTruth/TrackHelper.h"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//////////////////////////////////////////////////////////////////////////////////
// Initialize static data 
///////////////////////////////////////////////////////////////////////////////

static FADS::SensitiveDetectorEntryT<PixelSensorSD> mdtsd("PixelSensorSD");



PixelSensorSD::PixelSensorSD(G4String name) :  
  FADS::FadsSensitiveDetector(name),
  myHitColl(0),
  m_isGeoModel(true), 
  m_msg("PixelSensorSD")
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PixelSensorSD::~PixelSensorSD(){ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PixelSensorSD::Initialize(G4HCofThisEvent* /*HCE*/)
{
  if(msgLvl(MSG::DEBUG)) { msg(MSG::DEBUG) << "Initializing SD" << endreq; }
  DetectorGeometryHelper DGHelp;
  if(  DGHelp.GeometryType("Pixel") == GeoModel ) {
    m_isGeoModel = true;
    if(msgLvl(MSG::DEBUG)) { msg(MSG::DEBUG) << "Pixel Geometry is from GeoModel" << endreq; }
  } 
  else {
    m_isGeoModel = false;
    msg(MSG::ERROR) << "Pixel Geometry is from pure G4. NOT SUPPORTED!" << endreq;
  }
  //
  // Create a fresh map to store the hits
  //
  //myHitColl = new SiHitCollection("PixelHits");
  myHitColl = m_hitCollHelp.RetrieveNonconstCollection<SiHitCollection>("PixelHits");

}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool PixelSensorSD::ProcessHits(G4Step* aStep, G4TouchableHistory* /*ROhist*/)
{
  if(msgLvl(MSG::VERBOSE)) { msg(MSG::VERBOSE) << "Process Hit" << endreq; }

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

  if(msgLvl(MSG::VERBOSE)) {
    for (int i=0;i<myTouch->GetHistoryDepth();i++)
      {
	std::string detname=myTouch->GetVolume(i)->GetLogicalVolume()->GetName();
	int copyno=myTouch->GetVolume(i)->GetCopyNo();
	msg(MSG::VERBOSE)
	  << "Volume "
	  <<detname <<" Copy Nr. "<<copyno<<endreq;
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
    if(msgLvl(MSG::VERBOSE)){ 
      msg(MSG::VERBOSE) << "In the Pixel Barrel" << endreq; 
      msg(MSG::VERBOSE) << "----- Phi Module # " << phiMod << endreq; 
      msg(MSG::VERBOSE) << "----- Eta Ladder # " << etaMod << endreq; 
      msg(MSG::VERBOSE) << "----- Layer      # " << LayerDisk << endreq; 
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
    if(msgLvl(MSG::VERBOSE)){
      msg(MSG::VERBOSE) << "In the Pixel EndCap" << endreq; 
      msg(MSG::VERBOSE) << "----- PhiModule # " << phiMod << endreq; 
      msg(MSG::VERBOSE) << "----- Disk #      " << LayerDisk << endreq; 
      msg(MSG::VERBOSE) << "----- Endcap #    " << BrlEcap << endreq;     
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
    if(msgLvl(MSG::VERBOSE)){
      msg(MSG::VERBOSE) << "In the SLHC Pixel EndCap" << endreq; 
      msg(MSG::VERBOSE) << "----- PhiModule # " << phiMod << endreq; 
      msg(MSG::VERBOSE) << "----- Ring/Eta #  " << etaMod << endreq; 
      msg(MSG::VERBOSE) << "----- Disk #      " << LayerDisk << endreq; 
      msg(MSG::VERBOSE) << "----- Endcap #    " << BrlEcap << endreq;     
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
    if(msgLvl(MSG::VERBOSE)){ 
      msg(MSG::VERBOSE) << "In the DBM" << endreq; 
      msg(MSG::VERBOSE) << "----- PhiModule # " << phiMod << endreq; 
      msg(MSG::VERBOSE) << "----- Ring/Eta #  " << etaMod << endreq; 
      msg(MSG::VERBOSE) << "----- Disk #      " << LayerDisk << endreq; 
      msg(MSG::VERBOSE) << "----- Endcap #    " << BrlEcap << endreq; 
    }

  } else {
    // Do not expect other numbers. Need to fix PixelGeoModel if this occurs. 
    msg(MSG::ERROR) << "Unrecognized geometry in Pixel sensistive detector. Please contact maintainer of Pixel Detector Description." << endreq;
  }

  // get the barcode from the track helper
  TrackHelper trHelp(aStep->GetTrack());
  int barcode = trHelp.GetBarcode();
  SiHit newHit(lP1,
	       lP2,
	       edep,
	       aStep->GetPreStepPoint()->GetGlobalTime(),
	       barcode,
	       0,BrlEcap,LayerDisk,etaMod,phiMod,side);
  myHitColl->Insert(newHit);
  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PixelSensorSD::EndOfEvent(G4HCofThisEvent* /*HCE*/)
{
  if (!m_allowMods) 
    m_hitCollHelp.SetConstCollection<SiHitCollection>(myHitColl);

  //m_hitCollHelp.ExportCollection<SiHitCollection>(myHitColl,m_allowMods);
  if(msgLvl(MSG::DEBUG)) { msg(MSG::DEBUG) << "End of event " << endreq; }
}


