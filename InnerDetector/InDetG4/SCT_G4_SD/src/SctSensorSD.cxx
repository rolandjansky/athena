/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// SCT Sensitive Detector.
// The Hits are processed here. For every hit I get the position and
// an information on the sensor in which the interaction happened
//
#include <fstream>
#include "FadsSensitiveDetector/SensitiveDetectorEntryT.h"
#include "FadsSensitiveDetector/SensitiveDetectorCatalog.h"

// G4SCT
#include "SCT_G4_SD/SctSensorSD.h"

// Geant4 Stuff
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4Geantino.hh"
#include "G4ChargedGeantino.hh"


// CLHEP transform
#include "CLHEP/Geometry/Transform3D.h"    

#include "SimHelpers/DetectorGeometryHelper.h"
#include "MCTruth/TrackHelper.h"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//////////////////////////////////////////////////////////////////////////////////
// Initialize static data 
///////////////////////////////////////////////////////////////////////////////

static FADS::SensitiveDetectorEntryT<SctSensorSD> mdtsd("SctSensorSD");



SctSensorSD::SctSensorSD(G4String name) :  
  FADS::FadsSensitiveDetector(name), 
  myHitColl(0),
  m_isGeoModel(true),   
  m_msg("SctSensorSD")
{
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SctSensorSD::~SctSensorSD(){ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SctSensorSD::Initialize(G4HCofThisEvent* /*HCE*/)
{
  if(msgLvl(MSG::DEBUG)) { msg(MSG::DEBUG) << "Initializing SD" << endreq; }
  //
  // Create a fresh map to store the hits
  //myHitColl = new SiHitCollection("SCT_Hits");
  myHitColl = m_hitCollHelp.RetrieveNonconstCollection<SiHitCollection>("SCT_Hits");

  DetectorGeometryHelper DGHelp;
  if(  DGHelp.GeometryType("SCT") == GeoModel ) 
    {
      m_isGeoModel = true;
      if(msgLvl(MSG::DEBUG)) { msg(MSG::DEBUG) << "SCT Geometry is from GeoModel" << endreq; }
    } else {
    m_isGeoModel = false;
    msg(MSG::ERROR) << "SCT Geometry is from pure G4. NOT SUPPORTED!" << endreq;
  }

}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool SctSensorSD::ProcessHits(G4Step* aStep, G4TouchableHistory* /*ROhist*/)
{
  double edep = aStep->GetTotalEnergyDeposit();
  if(edep==0.) {
    if(aStep->GetTrack()->GetDefinition()!=G4Geantino::GeantinoDefinition() &&
       aStep->GetTrack()->GetDefinition()!=G4ChargedGeantino::ChargedGeantinoDefinition()) 
      return false;
  }
  edep *= CLHEP::MeV;
  //use the global time. i.e. the time from the beginning of the event
  // 
  // Get the Touchable History:
  //
  G4TouchableHistory*  myTouch = (G4TouchableHistory*)(aStep->GetPreStepPoint()->GetTouchable());
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
  //G4int History;
  //
  // Is it TB, barrel or endcap?
  //
  int brlEcap = 0;
  int layerDisk = 0;
  int etaMod = 0;
  int phiMod = 0;
  int side = 0;
  //
  // In the case of the TB the positioning integers won't be initialized
  // and the identifying integer will be zero. There is no need to do 
  // anything else

  int sensorCopyNo =  myTouch->GetVolume()->GetCopyNo();
 
  if (msgLvl(MSG::VERBOSE)) {
    for (int i=0;i<myTouch->GetHistoryDepth();i++)
      {
	std::string detname=myTouch->GetVolume(i)->GetLogicalVolume()->GetName();
	int copyno=myTouch->GetVolume(i)->GetCopyNo();
	msg(MSG::VERBOSE) << "Volume "<<detname << " Copy Nr. " << copyno << endreq;
      }
  }

  // Barrel geometry from GeoModel. Flag it with a 1000 in the IdTag
  //

  if(m_isGeoModel) {
    if(sensorCopyNo/1000) {
      // Barrel
      if(sensorCopyNo == 1000) {
	side = myTouch->GetVolume(1)->GetCopyNo();
	etaMod = myTouch->GetVolume(2)->GetCopyNo();
	phiMod = myTouch->GetVolume(3)->GetCopyNo();
	layerDisk = myTouch->GetVolume(5)->GetCopyNo();
      } else if(sensorCopyNo == 1100) {
	// SLHC stave layout
	int etaModTmp = myTouch->GetVolume(1)->GetCopyNo();
	int sign = (etaModTmp>=0)? +1 : -1;
	side = std::abs(etaModTmp)%2;
	etaMod = sign * std::abs(etaModTmp)/2;
	phiMod = myTouch->GetVolume(2)->GetCopyNo();
	layerDisk = myTouch->GetVolume(4)->GetCopyNo();
      } else if(sensorCopyNo/100 == 12) {
	// Segmented sensor (SLHC only)
	int iSegment = sensorCopyNo%100;
	int sensorEnvCopyNo = myTouch->GetVolume(1)->GetCopyNo();
	if (sensorEnvCopyNo == 1000) {
	  side = myTouch->GetVolume(2)->GetCopyNo();
	  etaMod = myTouch->GetVolume(3)->GetCopyNo() + iSegment;
	  phiMod = myTouch->GetVolume(4)->GetCopyNo();
	  layerDisk = myTouch->GetVolume(6)->GetCopyNo();
	} else if  (sensorEnvCopyNo == 1100) {
	  // Stave layout
	  int etaModTmp = myTouch->GetVolume(2)->GetCopyNo();
	  int sign = (etaModTmp>=0)? +1 : -1;
	  side = std::abs(etaModTmp)%2;
	  etaMod = sign * std::abs(etaModTmp)/2 + iSegment;
	  phiMod = myTouch->GetVolume(3)->GetCopyNo();
	  layerDisk = myTouch->GetVolume(5)->GetCopyNo();
	} else {
	  msg(MSG::ERROR) << "Unrecognized geometry in SCT sensistive detector. Please contact maintainer of SCT Detector Description." << endreq;
	}
      } else {
	msg(MSG::ERROR) << "Unrecognized geometry in SCT sensistive detector. Please contact maintainer of SCT Detector Description." << endreq;
      }
      if (msgLvl(MSG::VERBOSE)) {
	msg(MSG::VERBOSE) << "In the SCT Barrel" << endreq;
	msg(MSG::VERBOSE) << "----- side       : " << side << endreq;
	msg(MSG::VERBOSE) << "----- eta_module : " << etaMod << endreq;
	msg(MSG::VERBOSE) << "----- phi_module : " << phiMod << endreq;
	msg(MSG::VERBOSE) << "----- layer      : " << layerDisk << endreq;
	msg(MSG::VERBOSE) << "----- barrel_ec  : " << brlEcap<< endreq;
      }
    } else {
      // Endcap geometry from GeoModel. Flag it with a 500 or 600 in the IdTag
      // The level in the hierarchy is different for different geometries
      // 500 For pre DC3
      // 600 For DC3 and later (including SLHC)
      int sensorCopyNoTest =  sensorCopyNo/100;
      if(sensorCopyNoTest == 5 || sensorCopyNoTest == 6) {
	int signZ = (coord1.z() > 0) ? 1 : -1;
	brlEcap = 2 * signZ;
	side      = myTouch->GetVolume(0)->GetCopyNo() % 2;
	if (sensorCopyNoTest == 5) { // DC2 and Rome
	  etaMod    = myTouch->GetVolume(3)->GetCopyNo();
	  layerDisk = myTouch->GetVolume(4)->GetCopyNo();
	} else { // DC3 and later and SLHC
	  etaMod    = myTouch->GetVolume(2)->GetCopyNo();
	  layerDisk = myTouch->GetVolume(3)->GetCopyNo();
	}		
	int phiNumber = myTouch->GetVolume(1)->GetCopyNo();
	// Number of modules in ring and module which becomes 0 is encoded in the copy number.
	// This is in order to recreate the correct id in the negative endcap.
	int maxModules = (phiNumber & 0x00ff0000) >> 16;
	int moduleZero = (phiNumber & 0xff000000) >> 24; // When phiMod = moduleZero -> 0 after inverting
	phiMod = phiNumber & 0x0000ffff;
	// If -ve endcap then invert numbering 
	// maxModules is non-zero, but check for maxModules != 0  safeguards
	// against divide by zero (in case we create a geometry without the encoding).
	if (maxModules && signZ < 0) phiMod = (maxModules + moduleZero - phiMod) % maxModules;
	
	// Some printout
	if (msgLvl(MSG::VERBOSE)) {
	  msg(MSG::VERBOSE) << "In the SCT EndCap" << endreq;
	  msg(MSG::VERBOSE) << "----- side       : " << side  << endreq;
	  msg(MSG::VERBOSE) << "----- phi_module : " << phiMod <<endreq;
	  msg(MSG::VERBOSE) << "----- eta_module : " << etaMod << endreq;
	  msg(MSG::VERBOSE) << "----- layerDisk  : " << layerDisk << endreq;
	  msg(MSG::VERBOSE) << "----- barrel_ec  : " << brlEcap  << endreq;
	}
      } else {
	// Do not expect other numbers. Need to fix SCT_GeoModel or SCT_SLHC_GeoModel if this occurs. 
	msg(MSG::ERROR) << "Unrecognized geometry in SCT sensistive detector. Please contact maintainer of SCT Detector Description." << endreq;
      }
    }
  } else {
    // This was for pure G4 which is no longer supported.
    msg(MSG::ERROR) << "Unexpected condition in SCT sensistive detector. Please contact maintainer of SCT simulation." << endreq;
  }
      
  // get the barcode from the trackhelper
  TrackHelper trHelp(aStep->GetTrack());
  int barcode = trHelp.GetBarcode();
  SiHit newHit(lP1,
	       lP2,
	       edep,
	       aStep->GetPreStepPoint()->GetGlobalTime(),
	       barcode,
	       1,brlEcap,layerDisk,etaMod,phiMod,side);
  myHitColl->Insert(newHit);
  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SctSensorSD::EndOfEvent(G4HCofThisEvent* /*HCE*/)
{
  //  myHitColl->Export();
  if (!m_allowMods)
    m_hitCollHelp.SetConstCollection<SiHitCollection>(myHitColl);

  //m_hitCollHelp.ExportCollection<SiHitCollection>(myHitColl);
  if(msgLvl(MSG::DEBUG)) { msg(MSG::DEBUG) << "End of event " << endreq; }
}


