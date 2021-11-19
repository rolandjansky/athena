/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// HGTD Sensitive Detector.
// The Hits are processed here. For every hit, the position and information
// on the sensor in which the interaction happened are obtained.

// Class header
#include "HGTDSensorSD.h"

// Athena headers
#include "MCTruth/TrackHelper.h"

// Geant4 headers
#include "G4ChargedGeantino.hh"
#include "G4Geantino.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4VisExtent.hh" // Added to get extent of sensors
#include "G4VSolid.hh" // Added to get extent of sensors

// CLHEP headers
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include <stdint.h>
#include <string.h>

HGTDSensorSD::HGTDSensorSD(const std::string& name, const std::string& hitCollectionName)
    : G4VSensitiveDetector( name ), 
      m_HitColl( hitCollectionName )
{

}

// Initialize from G4
void HGTDSensorSD::Initialize(G4HCofThisEvent *)
{
    if (!m_HitColl.isValid()) m_HitColl = std::make_unique<SiHitCollection>();
}

G4bool HGTDSensorSD::ProcessHits(G4Step* aStep, G4TouchableHistory* /*ROhist*/)
{
    if (verboseLevel>5) G4cout << "Process Hit" << G4endl;

    G4double edep = aStep->GetTotalEnergyDeposit();
    edep *= CLHEP::MeV;

    if (edep==0.) {
        if (aStep->GetTrack()->GetDefinition() != G4Geantino::GeantinoDefinition() &&
            aStep->GetTrack()->GetDefinition() != G4ChargedGeantino::ChargedGeantinoDefinition())
            return false;
    }

    //
    // Get the Touchable History:
    //
    const G4TouchableHistory*  myTouch = dynamic_cast<const G4TouchableHistory*>(aStep->GetPreStepPoint()->GetTouchable());

    if(verboseLevel>5){
        for (int i=0;i<myTouch->GetHistoryDepth();i++){
            std::string detname = myTouch->GetVolume(i)->GetLogicalVolume()->GetName();
            int copyno = myTouch->GetVolume(i)->GetCopyNo();
            G4cout << "Volume " << detname << " Copy Nr. " << copyno << G4endl;
        }
    }

    //
    // Get the hit coordinates. Start and End Point
    //
    G4ThreeVector startCoord = aStep->GetPreStepPoint()->GetPosition();
    G4ThreeVector endCoord   = aStep->GetPostStepPoint()->GetPosition();
    // get the average position for writing
    G4ThreeVector globalPosition = (startCoord + endCoord)/2;

    // now deal with the Identifier
    int posNegEndcap = ( globalPosition.z() > 0. ? 1 : -1 );

    // make sure that it's a Sensor:
    std::string detname_layer=myTouch->GetVolume(0)->GetLogicalVolume()->GetName();
    if ( detname_layer.find("Sensor") == std::string::npos ) {
        // Do not expect other names. Need to fix HGTDGeoModel if this occurs.
        G4ExceptionDescription description;
        description << "ProcessHits: No HGTD sensitive detector with substring Sensor found. Check HGTD Detector Description.";
        G4Exception("HGTDSensorSD", "UnrecognizedHGTDGeometry", FatalException, description);
        abort();
    }

    if(verboseLevel>5){
        // The following code has been used to check the transformations between the detector elements
        // TRANSFORM from local to global / step by step
        for ( int i = myTouch->GetHistory()->GetDepth(); i >= 0 ; i-- ) {
            std::string detname = myTouch->GetHistory()->GetVolume(i)->GetLogicalVolume()->GetName();
            G4VisExtent extent = myTouch->GetHistory()->GetVolume(i)->GetLogicalVolume()->GetSolid()->GetExtent();
            int copyno=myTouch->GetHistory()->GetVolume(i)->GetCopyNo();
            const G4AffineTransform transformation1 = myTouch->GetHistory()->GetTransform( i ); // Transformation from global to current
            const G4AffineTransform transformationInverse = transformation1.Inverse();          // Transformation from current to global
            G4ThreeVector pos_center_local(0.0, 0.0, 0.0);
            G4ThreeVector pos_center_global = transformationInverse.TransformPoint( pos_center_local );
            G4ThreeVector pos_current = transformation1.TransformPoint( globalPosition );
            G4cout << "DEBUG HGTDG4SD : "
                   << "VOLUME: " << i << " detname: " << detname
                   << ", center of element : " << pos_center_global.x()*CLHEP::mm << ", y: " << pos_center_global.y()*CLHEP::mm << ", z: " << pos_center_global.z()*CLHEP::mm
                   << ", extent: x: " << extent.GetXmax() - extent.GetXmin() << ", y: " << extent.GetYmax() - extent.GetYmin() << ", z: " << extent.GetZmax() - extent.GetZmin()
                   << ", copyno: " << copyno << G4endl;
            G4cout << "DEBUG HGTDG4SD : LOCAL: x: " << pos_current.x()*CLHEP::mm << ", y: " << pos_current.y()*CLHEP::mm << ", z: " << pos_current.z()*CLHEP::mm << G4endl;

            if ( i >= 1) {
                const G4AffineTransform transformation2 = myTouch->GetHistory()->GetTransform( i-1 ); // Transformation from global to 1 up       
                G4AffineTransform transformation_up; // Transformation from current to 1 up
                transformation_up.Product( transformationInverse, transformation2 );
                G4ThreeVector pos_up = transformation_up.TransformPoint( pos_current );
                G4RotationMatrix rotmat = transformation_up.NetRotation(); // https://www-zeuthen.desy.de/geant4/clhep-2.0.4.3/classCLHEP_1_1HepRotation.html
                G4ThreeVector translation = transformation_up.NetTranslation(); // https://www-zeuthen.desy.de/ILC/geant4/clhep-2.0.4.3/classCLHEP_1_1Hep3Vector.html        
                G4cout << "DEBUG HGTDG4SD : Rotation:"
                       << "| xx:" << rotmat.xx() << ", xy: " << rotmat.xy() << ", xz: " << rotmat.xz() 
                       << "| yx:" << rotmat.yx() << ", yy: " << rotmat.yy() << ", yz: " << rotmat.yz() 
                       << "| zx:" << rotmat.zx() << ", zy: " << rotmat.zy() << ", zz: " << rotmat.zz() << " | " << G4endl;
                G4cout << "DEBUG HGTDG4SD : Translation: x: " << translation.x() << ", y:" << translation.y() << ", z:" << translation.z() << G4endl;
                G4cout << "DEBUG HGTDG4SD : TRANSFORMED: x:" << pos_up.x()*CLHEP::mm << ", y:" << pos_up.y()*CLHEP::mm << ", z:" << pos_up.z()*CLHEP::mm << G4endl;
            }
        } // end loop stackdepth
    }

    // Create the SiHits

    const G4AffineTransform transformation = myTouch->GetHistory()->GetTopTransform();

    G4ThreeVector localPosition1 = transformation.TransformPoint(startCoord);
    G4ThreeVector localPosition2 = transformation.TransformPoint(endCoord);

    if(verboseLevel>5){
        G4cout << " PreStepPoint " <<  G4endl;
        G4cout << " x (global/local) " << startCoord.x()*CLHEP::mm << " " << localPosition1[0]*CLHEP::mm << G4endl;
        G4cout << " y (global/local) " << startCoord.y()*CLHEP::mm << " " << localPosition1[1]*CLHEP::mm << G4endl;
        G4cout << " z (global/local) " << startCoord.z()*CLHEP::mm << " " << localPosition1[2]*CLHEP::mm << G4endl;
        
        G4cout << " PostStepPoint: " <<  G4endl;
        G4cout << " x (global/local) " << endCoord.x()*CLHEP::mm << " " << localPosition2[0]*CLHEP::mm << G4endl;
        G4cout << " y (global/local) " << endCoord.y()*CLHEP::mm << " " << localPosition2[1]*CLHEP::mm << G4endl;
        G4cout << " z (global/local) " << endCoord.z()*CLHEP::mm << " " << localPosition2[2]*CLHEP::mm << G4endl;
    }

    HepGeom::Point3D<double> lP1,lP2;
    lP1[SiHit::xEta] = localPosition1[1]*CLHEP::mm; //long edge of the module
    lP1[SiHit::xPhi] = localPosition1[0]*CLHEP::mm; //short edge of the module
    lP1[SiHit::xDep] = localPosition1[2]*CLHEP::mm; //depth (z)

    lP2[SiHit::xEta] = localPosition2[1]*CLHEP::mm;
    lP2[SiHit::xPhi] = localPosition2[0]*CLHEP::mm;
    lP2[SiHit::xDep] = localPosition2[2]*CLHEP::mm;

    std::string module_indices = myTouch->GetVolume(1)->GetLogicalVolume()->GetName();
    std::size_t found = module_indices.find_last_of("_");
  
    // get indices from the volume name
    // nomenclature is expected to be e.g. "HGTDModule0_layer_0_1_2"
    // for layer=0, phi=1, eta=2 (defined from HGTD_DetectorFactory)
    int eta   = atoi((module_indices.substr(found+1)).c_str());
    module_indices.erase(found);  
    found = module_indices.find_last_of("_");
    int phi   = atoi((module_indices.substr(found+1)).c_str());
    module_indices.erase(found);  
    found = module_indices.find_last_of("_");
    int layer   = atoi((module_indices.substr(found+1)).c_str());

    int endcap_side = 2*posNegEndcap;

    TrackHelper trHelp(aStep->GetTrack());
    m_HitColl->Emplace(lP1,
                       lP2,
                       edep,
                       aStep->GetPreStepPoint()->GetGlobalTime(),
                       trHelp.GetParticleLink(),
                       // this is hgtd, endcap_barrel, layer_disk, eta_module, phi_module, side
                       2,endcap_side,layer,eta,phi,0);

    return true;
}
