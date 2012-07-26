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

//#define SCTSD_CTB_DEBUG

// G4SCT
#include "SCT_G4_SD/SctSensor_CTB.h"

// Geant4 Stuff
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4Geantino.hh"
#include "G4ChargedGeantino.hh"


// CLHEP transform
#include "CLHEP/Geometry/Transform3D.h"    

// to permit access to StoreGate
//#include "GaudiKernel/ISvcLocator.h"
//#include "GaudiKernel/Bootstrap.h"

#include "SimHelpers/DetectorGeometryHelper.h"
#include "MCTruth/TrackHelper.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//////////////////////////////////////////////////////////////////////////////////
// Initialize static data 
///////////////////////////////////////////////////////////////////////////////

static FADS::SensitiveDetectorEntryT<SctSensor_CTB> mdtsd("SctSensor_CTB");



SctSensor_CTB::SctSensor_CTB(G4String name) :
  FADS::FadsSensitiveDetector(name), 
  myHitColl(0),
  m_isGeoModel(true),  
  m_msg("SctSensor_CTB")
{
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SctSensor_CTB::~SctSensor_CTB(){ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SctSensor_CTB::Initialize(G4HCofThisEvent* /*HCE*/)
{
  msg(MSG::DEBUG) << "Initializing SD" << endreq;
  //
  // Create a fresh map to store the hits
  myHitColl = new SiHitCollection("SCT_Hits");

  DetectorGeometryHelper DGHelp;
  if(  DGHelp.GeometryType("SCT") == GeoModel ) 
  {
	 m_isGeoModel = true;
	 msg(MSG::INFO) << "SCT Geometry is from GeoModel" << endreq;
  } else {
	 m_isGeoModel = false;
         msg(MSG::INFO) << "SCT Geometry is from pure G4" << endreq;
  }

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
    for (int i=0;i<myTouch->GetHistoryDepth();i++)
    {
        std::string detname=myTouch->GetVolume(i)->GetLogicalVolume()->GetName();
        int copyno=myTouch->GetVolume(i)->GetCopyNo();
        msg(MSG::DEBUG) << "Volume "<<detname <<" Copy Nr. "<<copyno<<endreq;
    }
#endif


  if(m_isGeoModel) {


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
      msg(MSG::DEBUG) << "In the SCT TestBeam" << endreq;
      msg(MSG::DEBUG) << " * Module Number: " << LayerDisk << "," << phiMod 
		      << " Side: " << side << endreq;
      msg(MSG::VERBOSE) << " Identifier will be [2.2."
	                << BrlEcap << "."
	                << LayerDisk << "."
	                << phiMod << "."
	                << etaMod << "."
	                << side << "]" << endreq;
#endif
    } else {
	    msg(MSG::ERROR) << "Unexpected copy number for sensor ... " << BEcopyNo << endreq;
    } 
  } else {
	  msg(MSG::ERROR) << "Error: GeoModel flag set to false!" << endreq;
  }
  TrackHelper trHelp(aStep->GetTrack());
  int barcode = trHelp.GetBarcode();

  SiHit newHit(lP1,
	       lP2,
	       edep,
	       aStep->GetPreStepPoint()->GetGlobalTime(),
	       barcode,
	       1,BrlEcap,LayerDisk,etaMod,phiMod,side);
  myHitColl->Insert(newHit);
  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SctSensor_CTB::EndOfEvent(G4HCofThisEvent* /*HCE*/)
{
  //  myHitColl->Export();
  m_hitCollHelp.ExportCollection<SiHitCollection>(myHitColl);
  // SiHitIterator i=myHitColl->begin();
  // for ( ;i<myHitColl->end();i++)    
  // {
  //   (*i)->print();
  //   (*i)->Draw();
  // }
  msg(MSG::DEBUG) << "End of event " << endreq;
}


