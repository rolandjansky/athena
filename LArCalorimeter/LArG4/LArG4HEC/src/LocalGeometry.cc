/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArG4::HEC::LocalGeometry
#include "LArG4HEC/LocalGeometry.h"

#include "LArG4Code/LArG4Identifier.h"
#include "LArG4HEC/Geometry.h"

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
#include "AthenaKernel/Units.h"
#include "globals.hh"
#include <cmath>
#include <stdexcept>


namespace Units = Athena::Units;


#undef DEBUG_HEC
#undef DEBUG_HEC_OLD_DIAGNOSTIC

#define NUM_ETABIN 15

namespace LArG4 {

  namespace HEC {

static double pads[7][NUM_ETABIN];

static G4double depthSize[7];
static G4double firstAbsorber[7];
static G4double betweenWheel;

LocalGeometry* LocalGeometry::m_instance = 0;
bool LocalGeometry::m_isX = false;


G4int binSearch(G4double ly, G4int depth, G4int reg)
{
   static G4int xl,xh;
   static G4int ires;
   
   if(reg == 1) { xl = 0; xh = 4;
   } else { xl = 4; xh = NUM_ETABIN-1; }
   
//   std::cout<<"binSearch::ly "<<ly<<" depth: "<<depth<<" reg: "<<reg<<std::endl;
//   if(ly < pads[depth][xl] || ly > pads[depth][xh] ) return -1;
   if(ly < pads[depth][xl]) return xl;
   if(ly > pads[depth][xh] ) return xh-1;
   while( xl < xh - 1 ) {
     ires = xl + (xh - xl)/2;
//     G4cout<<"xl: "<<xl<<" xh: "<<xh<<" ires: "<<ires<<" pads[depth][ires]: "<< pads[depth][ires]<<endl;
     if (ly == pads[depth][ires]) return ires - 1;
     if (ly < pads[depth][ires])
       xh = ires;
     else
       xl = ires;
   }
//   std::cout<<xl<<std::endl;

   return xl;
}

G4int binSearchAll(G4double ly, G4int depth, bool regular=false)
{
   static G4int xl,xh;
   static G4int ires;
   
   xl = 0; xh = NUM_ETABIN-1;
   
//   std::cout<<"binSearchAll::ly "<<ly<<" depth: "<<depth<<" regular: "<<regular<<std::endl;
//   if(ly < pads[depth][xl] || ly > pads[depth][xh] ) return -1;
   if(ly < pads[depth][xl]) return xl;
   if(ly > pads[depth][xh] ) return xh-1;
   while( xl < xh - 1 ) {
     ires = xl + (xh - xl)/2;
//     G4cout<<"xl: "<<xl<<" xh: "<<xh<<" ires: "<<ires<<" pads[depth][ires]: "<< pads[depth][ires]<<endl;
     if (ly == pads[depth][ires]) return ires - 1;
     if (ly < pads[depth][ires])
       xh = ires;
     else
       xl = ires;
   }
//   std::cout<<xl<<" "<<pads[depth][xl]<<" "<<pads[depth][xl+1]<<std::endl;
   if(regular) { // we should recompute to 0.1 eta binning overall numbering
      if(xl < 4) {
	 ires = xl;
	 xl *= 2;
	 if(ly > pads[depth][ires] + (pads[depth][ires+1]-pads[depth][ires])/2.) ++xl;
      } else {
	 xl += 4;
      }
   }
//   std::cout<<"After recomput.: "<<xl<<std::endl;
   return xl;
}
double deadZone(double locx, double locy, double /*rot*/=0)
{
         double r = sqrt(locx*locx + locy*locy);
         double alpha = atan(locx/locy);
         double dalpha = 2*M_PI/64 - alpha;
         return r*sin(dalpha);
}


    // Standard implementation of a singleton pattern.

    LocalGeometry* LocalGeometry::GetInstance()
    {
      static LocalGeometry instance;
      return &instance;
    }


    LocalGeometry::LocalGeometry() {

//       std::cout<<"LArG4::HEC::LocalGeometry called !"<<std::endl;
      StatusCode status;
       // Access the GeoModelSvc:
      ISvcLocator *svcLocator = Gaudi::svcLocator();

      IMessageSvc *msgSvc;
      status = svcLocator->service("MessageSvc", msgSvc);
      if(!status.isFailure()) m_log = new MsgStream(msgSvc,"HEC::LocalGeometry");;

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
      pAccessSvc->connect();

      std::string AtlasVersion = geoModel->atlasVersion();
      std::string LArVersion = geoModel->LAr_VersionOverride();

      std::string detectorKey  = LArVersion.empty() ? AtlasVersion : LArVersion;
      std::string detectorNode = LArVersion.empty() ? "ATLAS" : "LAr";

      const IRDBRecordset *hecPad    = pAccessSvc->getRecordset("HecPad",detectorKey, detectorNode);
      if (hecPad->size()==0) {
	hecPad    = pAccessSvc->getRecordset("HecPad","HecPad-00", "HecPad");
      }

      if(m_log) {
         *m_log << MSG::INFO << "Constructing local HEC geometry helper " << endmsg;
         *m_log << MSG::DEBUG<< " detectorKey: "<<detectorKey<<" detectorNode: "<<detectorNode<<std::endl;
      }


      if (hecPad->size()==0) { throw std::runtime_error("Cannot find the HECPAD Table"); }
      
      const IRDBRecord *rowPad;
      for(unsigned int i=0; i<hecPad->size(); ++i) {
         rowPad = (*hecPad)[i];
         for(int j=0; j<NUM_ETABIN; ++j) {
	    pads[i][j] = rowPad->getDouble("ETA",j);
         }
      }

      const IRDBRecordset *hecLongitudinalBlock = pAccessSvc->getRecordset("HecLongitudinalBlock",detectorKey,detectorNode);
      if (hecLongitudinalBlock->size()==0) {
	throw std::runtime_error("Cannot find the HecLongitinalBlock Table");
      }
      
      const IRDBRecordset *hadronicEndcap = pAccessSvc->getRecordset("HadronicEndcap",detectorKey,detectorNode);
      if (hadronicEndcap->size()==0) {
	throw std::runtime_error("Cannot find the HadronicEndcap Table");
      }
      for (int indexloop=0; indexloop < 7; ++indexloop){

	depthSize[indexloop]    = (*hecLongitudinalBlock)[indexloop]->getDouble("BLDPTH")*CLHEP::cm;
	firstAbsorber[indexloop]= (*hecLongitudinalBlock)[indexloop]->getDouble("PLATE0")*CLHEP::cm;
      }
      betweenWheel  = (*hadronicEndcap)[0]->getDouble("GAPWHL")*CLHEP::cm-0.001*CLHEP::cm; //40.5*mm


      pAccessSvc->shutdown();

    }


    LArG4Identifier LocalGeometry::CalculateIdentifier(const G4Step* a_step, const eLocalGeometryType g_type, int depthadd, double deadzone, double locyadd) {

      // Find out how long it took the energy to get here.
      G4StepPoint* pre_step_point = a_step->GetPreStepPoint();
      G4StepPoint* post_step_point = a_step->GetPostStepPoint();
      G4ThreeVector startPoint = pre_step_point->GetPosition();
      G4ThreeVector endPoint   = post_step_point->GetPosition();
      G4ThreeVector p = (startPoint + endPoint) * 0.5;

 
      // We can extract our position from the copy number of depth and module
      G4TouchableHistory* theTouchable = (G4TouchableHistory*) (pre_step_point->GetTouchable());

      //  Get local coordinates of the step, independently of how it was positioned  in World
      G4AffineTransform transformation;
      if(depthadd >=0) {
	 transformation = pre_step_point->GetTouchable()->GetHistory()->GetTopTransform();
      } else {
	 transformation = pre_step_point->GetTouchable()->GetHistory()->GetTransform(pre_step_point->GetTouchable()->GetHistoryDepth() + abs(depthadd));
      }
      G4ThreeVector startPointinLocal = transformation.TransformPoint(startPoint);
      G4ThreeVector   endPointinLocal = transformation.TransformPoint  (endPoint);
      G4ThreeVector          pinLocal =(startPointinLocal+endPointinLocal)*0.5;

#ifdef DEBUG_HEC

//      if(g_type == kLocDead) { 
      if(deadzone == 0.) { 
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

      std::cout <<"HEC::LocalGeometry +++++hitVolume="<<hitVolume<<" , his material="<<material<<", copyNumber="<<copyN<<", motherVolume="<<motherVolume <<", copy1="<< copyN1<< std::endl;
      std::cout <<"Coordinates: "<<p<<" Local coordinates: "<<pinLocal<<std::endl;
         G4int copyDepth = theTouchable->GetVolume(1 + abs(depthadd))->GetCopyNo() - 100;
         // As well as module
         G4int copyModule = theTouchable->GetVolume(2 + abs(depthadd))->GetCopyNo();
	 std::cout <<"g_type: "<<g_type<<" depthadd: "<<depthadd<<std::endl;
	 std::cout <<"copyModule = "<<copyModule<<" copyDepth = "<<copyDepth<<std::endl;
      }

#endif 

      G4int zSide;
      G4int sampling = 0;
      G4int region;
      G4int etaBin = 0; 
      G4int phiBin; 

      G4int copyModule;
 
      zSide = 2;
      if( p.z()<0.) zSide =-2; 

      if(g_type != kLocDead) { // Calculate the active or inactive ID

         // Volume name 
         // G4String hitVolume = theTouchable->GetVolume(0)->GetName();
         // We need also copy number of depth
         G4int copyDepth = theTouchable->GetVolume(1 + abs(depthadd))->GetCopyNo() - 100;
         // As well as module
         copyModule = theTouchable->GetVolume(2 + abs(depthadd))->GetCopyNo() - 1;
	 if(zSide<0) {
	    if(copyModule<16) copyModule = abs(copyModule - 15); else copyModule = 47 - copyModule;
	 }

         // Sampling Identifier
         switch(copyDepth) {
             case 0: { sampling = 0; break; }
             case 1: case 2: { sampling = 1; break; }
             case 3: case 4: { sampling = 2; break; }
             case 5: case 6: { sampling = 3; break; }
             default: { // Something wrong,  should not be
//			return CalculateIdentifier(a_step, kLocDead, depthadd); 
			assert(0<1);
			break; }
         }
  
         double locy, locx;
         if(m_isX) {
	    locy = fabs(pinLocal.x()/Units::mm);
	    locx = pinLocal.y()/Units::mm;
	 } else {
	    locy = fabs(pinLocal.y()/Units::mm);
	    locx = pinLocal.x()/Units::mm;
	 }
         // Add shift is needed (absorber)
         locy += locyadd;
         // Region Identifier
         // This will be computed from local Y coordinate
         if( locy < pads[copyDepth][4] ) region = 1;  
         else region = 0;

         // eta_Bin Identifier
         // Needs a table of coordinates of pad boundaries
         switch(region) {
             case 1: { etaBin = 3 - binSearch(locy, copyDepth, region); break; }
             case 0: { etaBin = 13 - binSearch(locy, copyDepth, region); break; }
             default: { assert(0<1); break; }
         }

         LArG4Identifier result = LArG4Identifier();

	 // Check, if we are not in dead zone
	 double distance = deadZone(fabs(locx),locy);
         if(depthadd < 0 || distance < deadzone) { // we return the dead identifier
#ifdef DEBUG_HEC 
		    std::cout<<" In active copyModule = "<< copyModule<<std::endl;
#endif
	    if(zSide * locx < 0) phiBin = copyModule;
		 else {
		       if(copyModule == 31) phiBin = 0; else phiBin=copyModule+1;
		 }
	    if(region==1 && etaBin == 3 && (sampling == 1 || sampling == 2)) etaBin = 2;
            result << 10          // DeadM
	           << 2*zSide   // LAr
	           << 2           // DM type
	           << sampling
	           << region+2
	           << etaBin
	           << phiBin;
#ifdef DEBUG_HEC 
         std::cout <<"HEC::LocalGeometry ++Dead in active  zSide = "<<zSide<<" type = 2"<<" , sampling = "<<sampling
	           <<"  ,  region="<<region+2 << " , phiBin="<<phiBin<< " ,  etaBin="<<etaBin <<std::endl;
#endif

         } else { // we return regular ID

            // Phi Identifier
            if(region == 1) { phiBin = copyModule;
            } else {
               phiBin = 2*copyModule;
	       if(zSide * locx > 0) ++phiBin;
	       /*
               if(m_isX) {
                   if(zSide * pinLocal.y() > 0) ++phiBin;
               } else {
                   if(zSide * pinLocal.x() > 0) ++phiBin;
               }
	       */
            }
	    if(region==1 && etaBin == 3 && (sampling == 1 || sampling == 2)) etaBin = 2;
	    if(region==0 && etaBin == 0 &&  sampling == 2) etaBin = 1;
	    if(region==0 && etaBin < 2 &&  sampling == 3) etaBin = 2;
            result       << 4          // LArCalorimeter
	                 << 2          // LArHEC
	                 << zSide
	                 << sampling
	                 << region
	                 << etaBin
	                 << phiBin;	       
#ifdef DEBUG_HEC 
         std::cout <<"HEC::LocalGeometry ++Active zSide = "<<zSide<<" , sampling = "<<sampling<<"  ,  region="<<region <<
                     " , phiBin="<<phiBin<< " ,  etaBin="<<etaBin <<std::endl;
#endif
	 }
	 
	return result;

      } else {  // Calculate dead ID

//        if(m_log) *m_log << MSG::DEBUG << "Local geometry DM Id " << endmsg;

         LArG4Identifier result = LArG4Identifier();

	 G4int type=1;
	 sampling=region=phiBin = 0;
	 etaBin = 25;

        // Copy number
        G4int copyN = theTouchable->GetVolume(0)->GetCopyNo();
        double locy, locx, locz, abslocz;
	locz = pinLocal.z()/Units::mm;
	abslocz = fabs(locz);
        if(m_isX) {
	    locy = fabs(pinLocal.x()/Units::mm);
	    locx = pinLocal.y()/Units::mm;
	} else {
	    locy = fabs(pinLocal.y()/Units::mm);
	    locx = pinLocal.x()/Units::mm;
	}

	if(copyN == 16969) { // Mother volume (should not be)
	   if(m_log) *m_log << MSG::WARNING << "Wrong Local geometry DM volume: HEC mother, default ID used" <<endmsg;
	   G4double phi = p.phi();
	   if (phi < 0) phi += 2*M_PI;
	   phiBin = int(phi*(32/(2*M_PI)));
	   G4double eta = fabs( p.pseudoRapidity() );
	   etaBin =  int(eta/0.1);
	} else if (copyN <= 32) { // HEC::Module
//	   double locz = fabs(pinLocal.z()/Units::mm);
//	   double locy = fabs(pinLocal.y()/Units::mm);
	   if(zSide<0) {
	      if(copyN-1<16) copyModule = abs(copyN - 1 - 15); else copyModule = 47 - (copyN - 1);
	   } else {
	      copyModule = copyN -1;
	   }
	   phiBin = 2*copyModule;
	   if(zSide * locx > 0) ++phiBin;
	   /*
           if(m_isX) {
                   if(zSide * pinLocal.y() > 0) ++phiBin;
           } else {
                   if(zSide * pinLocal.x() > 0) ++phiBin;
           }
	   */
#ifdef DEBUG_HEC_OLD_DIAGNOSTIC
	   std::cout<<"HEC::LocalGeometry Module locy: "<<locy<<" locz: "<<locz<<std::endl;
#endif
	   if(abslocz < firstAbsorber[0]) { // in front of HEC
#ifdef DEBUG_HEC_OLD_DIAGNOSTIC
	      std::cout<<"In front of HEC"<<std::endl;
#endif
	      type = 1;
              sampling = 2;
              region = 3; 
//	      etaBin = 13 - binSearchAll(locy, 0);
	      etaBin = 16 - binSearchAll(locy, 0, true);
	      if( etaBin < 0 ) etaBin = 0;
	   } else if(abslocz > depthSize[0] + depthSize[1] + depthSize[2] && abslocz < depthSize[0] + depthSize[1] + depthSize[2] + betweenWheel + firstAbsorber[3]) { // interwheel gap
#ifdef DEBUG_HEC_OLD_DIAGNOSTIC
	      std::cout<<"interwheel gap"<<std::endl;
#endif
	      type = 1;
              sampling = 2;
              region = 4;
//	      etaBin = 13 - binSearchAll(locy, 0);
	      etaBin = 16 - binSearchAll(locy, 3, true);
	   } else {
//	   sampling = localSampling(pinLocal.z()/Units::mm);
  	      double distance = deadZone(fabs(pinLocal.x()/Units::mm),locy);
	      if(distance > deadzone) { // We should return the inactive Id !!!!
#ifdef DEBUG_HEC_OLD_DIAGNOSTIC
	         std::cout<<"inactive Id"<<std::endl;
#endif
	         int depthNum;
	         if(abslocz < depthSize[0]) {
		    sampling = 0; depthNum=0;
	         } else if(abslocz < depthSize[0] + depthSize[1] + depthSize[2]) {
		    sampling = 1; 
		    if(abslocz < depthSize[0] + depthSize[1]) depthNum = 1; else depthNum = 2;
	         } else if(abslocz < depthSize[0] + depthSize[1] 
		                  + depthSize[2] + betweenWheel + depthSize[3] + depthSize[4] ) {
		    sampling = 2; 
		    if(abslocz < depthSize[0] + depthSize[1] 
			      + depthSize[2] + betweenWheel + depthSize[3]) depthNum = 3; else depthNum = 4;
	         } else {
		    sampling = 3;
		    if(abslocz < depthSize[0] + depthSize[1] 
			      + depthSize[2] + betweenWheel + depthSize[3] + depthSize[4] + depthSize[5])
		       depthNum = 5;
		    else
		       depthNum = 6;
	         }
                 if( locy < pads[depthNum][4] ) region = 1;  else region = 0;
                 switch(region) {
                     case 1: { etaBin = 3 - binSearch(locy, depthNum, region); phiBin /= 2; break; }
                     case 0: { etaBin = 13 - binSearch(locy, depthNum, region); break; }
                     default: { assert(0<1); break; }
                 }
#ifdef DEBUG_HEC 
         std::cout <<"HEC::LocalGeometry zSide = "<<zSide<<" , sampling = "<<sampling<<"  ,  region="<<region <<
                     " , phiBin="<<phiBin<< " ,  etaBin="<<etaBin <<std::endl;
#endif
	          if(region==1 && etaBin == 3 && (sampling == 1 || sampling == 2)) etaBin = 2;
	          if(region==0 && etaBin == 0 &&  sampling == 2) etaBin = 1;
	          if(region==0 && etaBin < 2 &&  sampling == 3) etaBin = 2;
		  result << 4          // LArCalorimeter
	                 << 2          // LArHEC
	                 << zSide
	                 << sampling
	                 << region
	                 << etaBin
	                 << phiBin;
		  return result;
	      } // intermodule cracks otherwise
#ifdef DEBUG_HEC_OLD_DIAGNOSTIC
	      std::cout<<"intermodule crack"<<std::endl;
#endif
	      if(zSide<0) {
	         if(copyN-1<16) copyModule = abs(copyN - 1 - 15); else copyModule = 47 - (copyN - 1);
	      } else {
		 copyModule = copyN - 1;
	      }
	      if(zSide * locx < 0) phiBin = copyModule; 
		 else {
		       if(copyModule == 31) phiBin = 0; else phiBin=copyModule+1;
		 }
	      type = 2; 
	      int depthNum;
	      if(abslocz < depthSize[0]) {
		 sampling = 0; depthNum=0;
	      } else if(abslocz < depthSize[0] + depthSize[1] + depthSize[2]) {
		 sampling = 1; 
		 if(abslocz < depthSize[0] + depthSize[1]) depthNum = 1; else depthNum = 2;
	      } else if(abslocz < depthSize[0] + depthSize[1] + depthSize[2] + betweenWheel + depthSize[3] + depthSize[4] ) {
		 sampling = 2; 
		 if(abslocz < depthSize[0] + depthSize[1] + depthSize[2] + betweenWheel + depthSize[3]) depthNum = 3; else depthNum = 4;
	      } else {
		 sampling = 3;
		 if(abslocz < depthSize[0] + depthSize[1] + depthSize[2] + betweenWheel + depthSize[3] + depthSize[4] + depthSize[5])
		    depthNum = 5;
		 else
		    depthNum = 6;
	      }
              if( locy < pads[depthNum][4] ) region = 3;  else region = 2;
#ifdef DEBUG_HEC_OLD_DIAGNOSTIC
	      std::cout<<locy<<" "<<pads[depthNum][4]<<" "<<region<<std::endl;
#endif
              switch(region) {
                  case 3: { etaBin = 3 - binSearch(locy, depthNum, region-2); break; }
                  case 2: { etaBin = 13 - binSearch(locy, depthNum, region-2); break; }
                  default: { assert(0<1); break; }
              }
	   }
	} else if (copyN==50 ) { // First Absorber - in front of HEC 
	   copyModule = theTouchable->GetVolume(2)->GetCopyNo() - 1;
	   if(zSide<0) {
	     if(copyModule<16) copyModule = abs(copyModule - 15); else copyModule = 47 - copyModule;
	   }
	   phiBin = 2*copyModule;
	   if(zSide * locx > 0) ++phiBin;
	   /*
           if(m_isX) {
                   if(zSide * pinLocal.y() > 0) ++phiBin;
           } else {
                   if(zSide * pinLocal.x() > 0) ++phiBin;
           }
	   */
	   type = 1;
           sampling = 2;
           region = 3;
//	   etaBin = 13 - binSearchAll(fabs(pinLocal.y()/Units::mm), 0);
//	   etaBin = 16 - binSearchAll(fabs(pinLocal.y()/Units::mm), 0, true);
	   etaBin = 16 - binSearchAll(locy, 0, true);
	   if( etaBin < 0 ) etaBin = 0;
	} else if (copyN==51 ) { // First Absorber  - interwheel gap
	   copyModule = theTouchable->GetVolume(2)->GetCopyNo() - 1;
	   if(zSide<0) {
	     if(copyModule<16) copyModule = abs(copyModule - 15); else copyModule = 47 - copyModule;
	   }
	   phiBin = 2*copyModule;
	   if(zSide * locx > 0) ++phiBin;
	   /*
           if(m_isX) {
                   if(zSide * pinLocal.y() > 0) ++phiBin;
           } else {
                   if(zSide * pinLocal.x() > 0) ++phiBin;
           }
	   */
	   type = 1;
           sampling = 2;
           region = 4;
//	   etaBin = 13 - binSearchAll(fabs(pinLocal.y()/Units::mm), 0);
//	   etaBin = 16 - binSearchAll(fabs(pinLocal.y()/Units::mm), 3, true);
	   etaBin = 16 - binSearchAll(locy, 3, true);
	} else if (copyN>=100  && copyN<= 106 ) { //Depth
	   copyModule = theTouchable->GetVolume(1)->GetCopyNo() - 1;
	   if(zSide<0) {
	     if(copyModule<16) copyModule = abs(copyModule -  15); else copyModule = 47 - copyModule;
	   }
	   phiBin = 2*copyModule;
	   if(zSide * locx > 0) ++phiBin;
	   /*
           if(m_isX) {
                   if(zSide * pinLocal.y() > 0) ++phiBin;
           } else {
                   if(zSide * pinLocal.x() > 0) ++phiBin;
           }
	   double locy = fabs(pinLocal.y()/Units::mm);
	   */
	   if(copyN==100 && locz < -depthSize[0]/2. + firstAbsorber[0]) { // in front of HEC
	      type = 1;
              sampling = 2;
              region = 3;
//	      etaBin = 13 - binSearchAll(locy, 0);
	      etaBin = 16 - binSearchAll(locy, 0, true);
	      if( etaBin < 0) etaBin = 0;
	   } else if(copyN==103 && locz < -depthSize[3]/2. + firstAbsorber[3]) { // interwheel gap
	      type = 1;
              region = 4;
              sampling = 2;
//	      etaBin = 13 - binSearchAll(locy, 0);
	      etaBin = 16 - binSearchAll(locy, 3, true);
	   } else { // intermodule cracks
	      double distance = deadZone(fabs(locx),locy);
	      if(distance > deadzone) { // We should return the inactive Id !!!!
//		 std::cout<<"Calling kLocInactive"<<std::endl;
	         return CalculateIdentifier(a_step, kLocInactive, -1, deadzone);
	      }
	      copyModule  = theTouchable->GetVolume(1)->GetCopyNo() - 1;
	      if(zSide<0) {
	        if(copyModule<16) copyModule = abs(copyModule - 15); else copyModule = 47 - copyModule;
	      }
	      if(zSide * locx < 0) phiBin = copyModule; 
		 else {
		       if(copyModule == 31) phiBin = 0; else phiBin=copyModule+1;
		 }
	      type =2; 
	      switch(copyN) {
	         case 100: {sampling = 0; break;}
	         case 101: case 102: {sampling=1; break;}	
	         case 103: case 104: {sampling=2; break;}	
	         case 105: case 106: {sampling=3; break;}	
	      }

              if( locy < pads[copyN-100][4] ) {
                region = 3;
                etaBin = 3 - binSearch(locy, copyN-100, region-2);
              }
              else {
                region = 2;
                etaBin = 13 - binSearch(locy, copyN-100, region-2);
              }
	   }
	}
	
#ifdef DEBUG_HEC 
         std::cout <<"HEC::LocalGeometry ++Dead   zSide = "<<zSide<<" type = "<<type<<" , sampling = "<<sampling
	           <<"  ,  region="<<region << " , phiBin="<<phiBin<< " ,  etaBin="<<etaBin <<std::endl;
#endif

	result << 10          // DeadM
	       << 2*zSide     // LAr
	       << type        // DM type
	       << sampling
	       << region
	       << etaBin
	       << phiBin;
	return result;

//	 return Geometry::GetInstance()->CalculateIdentifier(a_step, kDead);
      }

    }

  } // namespace HEC

} // namespace LArG4
