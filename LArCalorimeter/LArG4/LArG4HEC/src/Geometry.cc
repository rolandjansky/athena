/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArG4::HEC::Geometry

// This class contains the geometry calculations needed to calculate
// an identifier for a given G4Step.

// 13-Jan-2004: Karapetian Gaiane wrote the geometry code, Bill
// Seligman structured into the LArG4 processing.

#include "LArG4HEC/Geometry.h"

#include "LArG4Code/LArG4Identifier.h"

#include "G4ThreeVector.hh"
#include "G4StepPoint.hh"
#include "G4Step.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4TouchableHistory.hh"


#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "StoreGate/StoreGateSvc.h"
#include "globals.hh"
// 03-Jan-2002 WGS: For 'copysign'.
#include <cmath>
#include <stdexcept>
static G4double depthSize[7];
static G4double firstAbsorber[7];
static G4int gapNumber[7];
static G4double moduleNumber;
static G4double zStart ;
static G4double betweenWheel;
static G4double moduleDeltaPhi;
static G4double samplingStartZ[4];
static G4double samplingEndZ[4];
static G4double moduleRinner1;
static G4double moduleRinner2;

static const G4double deadZone = 4.*CLHEP::mm;


#undef DEBUG_HEC

namespace LArG4 {

  namespace HEC {

    // Standard implementation of a singleton pattern.

    Geometry* Geometry::GetInstance()
    {
      static Geometry instance;
      return &instance;
    }


    Geometry::Geometry()
    {

      std::cout<<"LArG4::HEC::Geometry  ERROR, should not be called !!!!"<<std::endl;

      StatusCode status;
      // Access the GeoModelSvc:
      ISvcLocator *svcLocator = Gaudi::svcLocator();
      IGeoModelSvc *geoModel;
      status = svcLocator->service ("GeoModelSvc",geoModel);
      if (status != StatusCode::SUCCESS) {
	throw std::runtime_error ("Cannot locate GeoModelSvc!!");
      }
      
      // Access the geometry database:
      IRDBAccessSvc *pAccessSvc;
      status=svcLocator->service("RDBAccessSvc",pAccessSvc);
      if (status != StatusCode::SUCCESS) {
	throw std::runtime_error ("Cannot locate RDBAccessSvc!!");
      }
      
      // Obtain the geometry version information:
      
      std::string AtlasVersion = geoModel->atlasVersion();
      std::string LArVersion = geoModel->LAr_VersionOverride();
      
      std::string detectorKey  = LArVersion.empty() ? AtlasVersion : LArVersion;
      std::string detectorNode = LArVersion.empty() ? "ATLAS" : "LAr";
      
      
      const IRDBRecordset *hecLongitudinalBlock = pAccessSvc->getRecordset("HecLongitudinalBlock",detectorKey,detectorNode);
      if (hecLongitudinalBlock->size()==0) {
	throw std::runtime_error("Cannot find the HecLongitinalBlock Table");
      }
      
      const IRDBRecordset *hadronicEndcap = pAccessSvc->getRecordset("HadronicEndcap",detectorKey,detectorNode);
      if (hadronicEndcap->size()==0) {
	throw std::runtime_error("Cannot find the HadronicEndcap Table");
      }

      const IRDBRecordset *emecGeometry = pAccessSvc->getRecordset("EmecGeometry",detectorKey,detectorNode);
      if (emecGeometry->size()==0) {
	throw std::runtime_error("Cannot find the EmecGeometry Table");
      }
      

      // Constructor initializes the geometry.


      // Get the endcap shift from the detector parameters.
      m_zShift = (*emecGeometry)[0]->getDouble("ZSHIFT")*CLHEP::cm;

      // Get HEC parameters from the detector parameters.
      zStart = (*hadronicEndcap)[0]->getDouble("ZSTART")*CLHEP::cm; // 427.7.*cm 

      betweenWheel  = (*hadronicEndcap)[0]->getDouble("GAPWHL")*CLHEP::cm-0.001*CLHEP::cm; //40.5*CLHEP::mm
      moduleNumber  = (*hadronicEndcap)[0]->getInt("NSCT"); // 32 Modules
      for (G4int indexloop=0; indexloop < 7; ++indexloop){

	depthSize[indexloop]    = (*hecLongitudinalBlock)[indexloop]->getDouble("BLDPTH")*CLHEP::cm;
	firstAbsorber[indexloop]= (*hecLongitudinalBlock)[indexloop]->getDouble("PLATE0")*CLHEP::cm;
	gapNumber[indexloop]    = (int) (*hecLongitudinalBlock)[indexloop]->getDouble("BLMOD");
      }
      samplingStartZ[0] = zStart;
      samplingEndZ[0]   = zStart+depthSize[0];
      samplingStartZ[1] = samplingEndZ[0];
      samplingEndZ[1]   = samplingEndZ[0] +depthSize[1]+depthSize[2];
      samplingStartZ[2] = samplingEndZ[1] + betweenWheel;
      samplingEndZ[2]   = samplingEndZ[1] +depthSize[3]+depthSize[4]+ betweenWheel;
      samplingStartZ[3] = samplingEndZ[2];
      samplingEndZ[3]   = samplingEndZ[2] +depthSize[5]+depthSize[6];

      moduleDeltaPhi   = 2*M_PI/moduleNumber; //11.25*deg;  
      
      moduleRinner1  = (*hecLongitudinalBlock)[0]->getDouble("BLRMN")*CLHEP::cm;
      moduleRinner2  = (*hecLongitudinalBlock)[0]->getDouble("BLRMN")*CLHEP::cm;
    }


    Geometry::~Geometry() {;}


    LArG4Identifier Geometry::CalculateIdentifier(const G4Step* a_step, const eGeometryType g_type)
    {
      // Given a G4Step, find the sampling, region, eta bin, and phi bin
      // in the LAr HEC associated with that point.

      // Note that we don't do any range checking; it is assumed that p
      // lies within the LAr endcap.  But we do have debug checks via asserts.

      // The default result is a blank identifier.
      LArG4Identifier result = LArG4Identifier();

      // Find out how long it took the energy to get here.
      G4StepPoint* pre_step_point = a_step->GetPreStepPoint();
      G4StepPoint* post_step_point = a_step->GetPostStepPoint();
      G4ThreeVector startPoint = pre_step_point->GetPosition();
      G4ThreeVector endPoint   = post_step_point->GetPosition();
      G4ThreeVector p = (startPoint + endPoint) * 0.5;

      // Adjust the input point for any endcap offset (the endcap
      // geometry was designed to point at (0,0,0) in the detector; if
      // we've shifted the endcap, we have to project the geometry to a
      // different point).
      G4double pointZ = p.z();

      G4double pointZshift = pointZ - copysign(m_zShift,pointZ);

      G4ThreeVector pointShift(p.x(),
			       p.y(),
			       pointZshift);

      // Determine eta, phi, and z w.r.t. the shifted point.
      G4double eta = fabs( pointShift.pseudoRapidity() );
      G4double phi = pointShift.phi();
      if (phi < 0) phi += 2*M_PI;
      G4double absZ = fabs( pointZshift );

#ifdef DEBUG_HEC
      G4cout<<" ***NEW   STEP****   "<<G4endl;
      G4cout<<"middle point="<< pointZ<<" , eta = "<<eta <<"  , phi= "<<phi<<G4endl;
      G4cout<<"start point hit Z="<<startPoint.z()<<" , eta = "<<startPoint.pseudoRapidity()<<"  ,phi()= "<<startPoint.phi()<<G4endl;
					  
      assert (eta > 1.375);
#endif

      // Determine the "type" of the LArHecSolid in which the hit took place.

      // Positive/negative wheel of HEC

      G4int zSide;
      G4int sampling;
      G4int sector;
      G4int region;
      G4int etaBin; 
      G4int phiBin; 
      G4int phiSector;
 
      zSide = 2;
      if ( pointZ < 0. ) zSide = -2; 

#ifdef DEBUG_HEC_OLD_DIAGNOSTIC

      // The hitted volume's identier is possible to get by the name of his "mother" volume.
      // G4cout <<" hitLogiVolume->GetNoDaughters=   " <<hitLogiVolume->GetNoDaughters()<<G4endl;
      // G4cout <<"+++++"<<hitLogiVolume->GetDaughter(1)->GetLogicalVolume()->GetMaterial()->GetName()<<G4endl;
      // G4cout <<"+++++"<<hitLogiVolume->GetName()<<G4endl;

      G4String hitVolume=a_step->GetPreStepPoint()->GetPhysicalVolume()->GetName();

      G4int copyN=a_step->GetPreStepPoint()->GetPhysicalVolume()->GetCopyNo();

      // 16-Oct-2003 WGS: Due to a revision in Geant4,
      // G4VPhysicalVolume::GetMother() no longer exists.  If we want to
      // find a mother volume, we have to do so in a more complex way:

      G4TouchableHistory* touchHist = 
	(G4TouchableHistory*) a_step->GetPreStepPoint()->GetTouchable();
      G4int volDepth = touchHist->GetHistoryDepth();

      // If the volume depth is 0, then this is the mother volume of the
      // simulation.  (I know that's not likely, but let's be sure.)
      // Otherwise, we want to go up one level in depth to get the mother
      // volume.

      //if ( volDepth != 0 ) volDepth--;
      //G4VPhysicalVolume *motherPhysical = touchHist->GetVolume(volDepth);
      G4VPhysicalVolume *motherPhysical = touchHist->GetVolume(1);

      // G4String motherVolume=a_step->GetPreStepPoint()->GetPhysicalVolume()->GetMother()->GetName();
      G4String motherVolume=motherPhysical->GetName();

      G4String material=a_step->GetPreStepPoint()->GetPhysicalVolume()->GetLogicalVolume()->GetMaterial()->GetName();

      // G4int copyN1=a_step->GetPreStepPoint()->GetPhysicalVolume()->GetMother()->GetCopyNo();
      G4int copyN1=motherPhysical->GetCopyNo();

      G4cout <<"+++++hitVolume="<<hitVolume<<" , his material="<<material<<", copyNumber="<<copyN<<", motherVolume="<<motherVolume <<", copy1="<< copyN1<< G4endl;

#endif 

      if(g_type != kDead) { // Calculate the active and inactive ID
         // Sampling Identifier
         if (absZ<samplingEndZ[0]) sampling = 0; 
         else if ((absZ>samplingStartZ[1]) && (absZ<samplingEndZ[1])) sampling = 1; 
         else if ((absZ>samplingStartZ[2]) && (absZ<samplingEndZ[2])) sampling = 2; 
         else if ((absZ>samplingStartZ[3]) && (absZ<samplingEndZ[3])) sampling = 3; 
         else { // Something wrong, it is a dead hit ?
	    // sampling = -1;
	    return CalculateIdentifier(a_step, kDead);
         }

         // Region Identifier
         if (eta<2.5) region = 0;  
         else  region = 1;

         // Check if not in crack between modules
         G4double alph = atan(pointShift.y()/pointShift.x()); 
         G4double dalpha = alph - int(alph/moduleDeltaPhi + copysign(0.5,alph))*moduleDeltaPhi;
         G4double distance = sqrt(pointShift.x()*pointShift.x()+pointShift.y()*pointShift.y())*sin(dalpha);
         if(fabs(distance) < deadZone) {
	    return CalculateIdentifier(a_step, kDead);
         }

         // Sector Identifier and Phi_sector Identifier will be replaced
         // by Phi Identifier
         // Sector Identifier
         sector = G4int(phi/moduleDeltaPhi);   //  phi/11.25*deg;

         // Phi_sector Identifier
         phiSector=0;
         if ( region == 0 ) {
	    if ((phi-float(sector)*moduleDeltaPhi) > (moduleDeltaPhi/2.)) phiSector=1;
         }
 
         // Sector Identifier and Phi_sector Identifier will be replaced
         // by Phi Identifier  
         if ( region == 1 ) phiBin=sector;
         else phiBin=2*sector+phiSector;


         // eta_Bin Identifier
         etaBin = -1;
         if ( region == 0 ) {
	   etaBin =  G4int((eta-1.5)/0.1); 
         } else {
	   etaBin = G4int((eta-2.5)/0.2);
	   if ((sampling == 2) && (etaBin >2)) etaBin = 2;
         } 
  
#ifdef DEBUG_HEC
         std::cout<<"+++++ Active:"<<std::endl;
	 std::cout <<"zSide = "<<zSide<<" , sector = "<<sector<<
	    " , sampling = "<<sampling<<"  ,  region="<<region <<
	    " , phiBin="<<phiBin<< " ,  etaBin="<<etaBin <<std::endl;
#endif

         result << 4          // LArCalorimeter
	        << 2          // LArHEC
	        << zSide
	        << sampling
	        << region
	        << etaBin
	        << phiBin;	       

      } else {  // Calculate dead ID

	 G4int type = 0;
	 G4int subdet;
     
	 if ( pointZshift < 0. ) subdet = -4; else subdet = 4;
	 
         phiBin = G4int(phi/moduleDeltaPhi); 
         double r = sqrt(pointShift.x()*pointShift.x()+pointShift.y()*pointShift.y());
         if(absZ<samplingEndZ[0]) sampling = 0;
         else if(absZ<samplingEndZ[1]) sampling = 1;
           else if(absZ<samplingEndZ[2]) sampling = 2;
	         else sampling = 3;


         if (absZ<samplingStartZ[0]+firstAbsorber[0]) { // in front of HEC
	    type = 1; 
	    sampling = 2;
	    region = 3;
	    etaBin =  G4int((eta-1.5)/0.1); 
	 } else if((absZ < samplingEndZ[0] && r < moduleRinner1) || 
	           (absZ < samplingEndZ[3] && r < moduleRinner2) )  { // HEC-FCAL crack
	       region = 4;
	       etaBin = 0; 
	 } else if (
	       ((absZ>samplingStartZ[0]+firstAbsorber[0]) && (absZ<samplingEndZ[1])) ||
	       ((absZ>samplingStartZ[2]+firstAbsorber[3]) && (absZ<samplingEndZ[3]))
	       ) { // intermodule cracks
            G4double alph = atan(pointShift.y()/pointShift.x()); 
            G4double dalpha = alph - int(alph/moduleDeltaPhi + copysign(0.5,alph))*moduleDeltaPhi;
            G4double distance = sqrt(pointShift.x()*pointShift.x()+pointShift.y()*pointShift.y())*sin(dalpha);
            if(fabs(distance) < deadZone) {
	       type = 2;
                  if (eta<2.5) {
	             region = 2;
	             etaBin =  G4int((eta-1.5)/0.1);
	          }  else {
                     region = 3; 
	             etaBin =  G4int((eta-2.5)/0.2);
		  }
	    } else { // something wrong - where it comes from? - default calculator hit
	       type = 1;
	       region = sampling = 0;
	       etaBin =  G4int((eta-1.5)/0.1); 
	    }
	 } else if ((absZ>samplingEndZ[1]) && (absZ<samplingStartZ[2]+firstAbsorber[3])) { // interwheel crack
	    type = 1;
	    region = 4;
	    sampling = 2;
	    etaBin =  G4int((eta-1.5)/0.1);
	 } else { // HEC-Ext.B crack
	    type = 1;
	    if(eta < 1.7) {
	       sampling = 2;
	       region = 5;
	       etaBin =  G4int((eta-1.0)/0.1);
	    } else { // Back of the HEC
	       sampling = 3;
	       region = 0;
	       etaBin =  G4int((eta-1.7)/0.1);
	    }
	 }

#ifdef DEBUG_HEC
      std::cout<<"+++++ Dead:"<<std::endl;
      G4cout <<"subdet = "<<subdet<<" , type = "<<type<<
	" , sampling = "<<sampling<<"  ,  region="<<region <<
	" , phi="<<phiBin<< " ,  etaBin="<<etaBin <<G4endl;
#endif

	 result << 10      // Calorimeter
	     << subdet     // +-4 - LAr dead material
	     << type
	     << sampling
	     << region
	     << etaBin
	     << phiBin;	       

      }

      return result;
    }

  } // namespace HEC

} // namespace LArG4
