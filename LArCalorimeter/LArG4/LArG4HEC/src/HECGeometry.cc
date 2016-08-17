/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArG4::HEC::HECGeometry
#include "LArG4Code/LArG4Identifier.h"
#include "LArG4HEC/HECGeometry.h"

#include "G4ThreeVector.hh"
#include "G4StepPoint.hh"
#include "G4Step.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4TouchableHistory.hh"
#include "G4Tubs.hh"

#include "LArReadoutGeometry/HECLongBlock.h"
#include "LArReadoutGeometry/HECDetectorManager.h"
#include "LArReadoutGeometry/HECDetectorRegion.h"
 
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/StoreGate.h"
#include "AthenaKernel/getMessageSvc.h"
#include "AthenaKernel/Units.h"

#include "globals.hh"
#include <string>
#include <cmath>
#include <stdexcept>


namespace Units = Athena::Units;


#undef DEBUG_HEC

#undef DEBUG_HEC_OLD_DIAGNOSTIC

#undef DEBUG_DEAD

#define NUM_ETABIN 15


using HepGeom::Point3D;


namespace LArG4 {

  namespace HEC {

bool   HECGeometry::m_depthHist = false;
bool   HECGeometry::m_withMother = false;
bool   HECGeometry::m_inMother = false;
int    HECGeometry::m_g4historyDepth = 0;
double HECGeometry::s_depthSize[7];
double HECGeometry::s_depthSum[7];
double HECGeometry::s_minval[7][2][10];
double HECGeometry::s_maxval[7][2][10];
double HECGeometry::s_firstAbsThickness[2];
double HECGeometry::s_wheel1;
double HECGeometry::s_wheel2;
double HECGeometry::s_betweenWheels;
double HECGeometry::s_hecLength;
double HECGeometry::s_startHec2;
double HECGeometry::s_rOuter;
double HECGeometry::s_rInner1;
double HECGeometry::s_rInner2;


HECGeometry* HECGeometry::m_instance = 0;


HECGeometry* HECGeometry::GetInstance()
{
  if (m_instance == 0) m_instance = new HECGeometry();
  return m_instance;
}


    HECGeometry::HECGeometry() {
  
  
  MsgStream log(Athena::getMessageSvc(),"LArG4HEC::HECGeometry" );
  log << MSG::DEBUG << "HECGeometry - Constructor" << endmsg;
    
  m_hecManager=NULL;
  
  if (!m_hecManager){
    StoreGateSvc* detStore = StoreGate::pointer("DetectorStore");
    if (detStore->retrieve(m_hecManager)!=StatusCode::SUCCESS){
      throw std::runtime_error("HEC::HECGeometry Can't find HECDetectorManager!");
    }
  }


  // This is VERY clumsy, but at the moment the only way to get the eta boundaries
  // for a given block from the ReadoutGeometry and at the same time have the 
  // eta identifier ready for any given depth. 

  for (int iblock=0; iblock<7; iblock++)  {
    s_depthSum[iblock] = 0.;
    s_depthSize[iblock] = 0.;
    for (int ireg=0; ireg<2; ireg++)     {
      for (int etaseg=0; etaseg<10; etaseg++)  {
	s_minval[iblock][ireg][etaseg] = 0.; 
	s_maxval[iblock][ireg][etaseg] = 0.;
      }
    }
  }

  double depthsum_tally = 0.;
  for (int depthIndex=0; depthIndex<7; depthIndex++)
    {
      s_depthSize[depthIndex] = m_hecManager->getBlock(depthIndex)->getDepth();
      depthsum_tally += s_depthSize[depthIndex];
      if (depthIndex==3) depthsum_tally += 40.5; // YIKES! hard-coded number...!
      s_depthSum[depthIndex] = depthsum_tally  ;

      const HECLongBlock *block = m_hecManager->getBlock(depthIndex);
      
      int isegInner = 0;
      int isegOuter = block->getNumRadialSegments();
      int numBlk  = block->getBlockNumber();
      
      int nInReg = 4;
      if (numBlk>1 && numBlk<6) { isegInner = 1; nInReg=3; }
      if (numBlk>6)isegOuter=11; 
      else if (numBlk>4)isegOuter=12; 
      else if (numBlk>2)isegOuter=13;
      
      int ieta = nInReg;
      int iregion = 1;
      
      for (int iseg=isegInner; iseg<isegOuter; iseg++)
	{
	  ieta--;
	  if (ieta < 0) { iregion=0; ieta=9; } 
	  
	  const HECRadialSegment *hecRad = block->getRadialSegment(iseg);
	  s_minval[depthIndex][iregion][ieta]= hecRad->getMinVal();
	  s_maxval[depthIndex][iregion][ieta]= hecRad->getMaxVal();
	  // This is a most terrible hack to correct numbers in the database...:
	  double rInner = m_hecManager->getBlock(depthIndex)->getInnerRadius();
	  double s_rOuter = m_hecManager->getBlock(depthIndex)->getOuterRadius();
	  if (hecRad->getMaxVal()==2027.)                    s_maxval[depthIndex][iregion][ieta]=s_rOuter;
	  if (hecRad->getMinVal()==375. && depthIndex==0)    s_minval[depthIndex][iregion][ieta]=rInner;
	  else if (hecRad->getMinVal()==478 && depthIndex>0) s_minval[depthIndex][iregion][ieta]=rInner;
	}
    }


  // And some essential numbers we'll need over and over again:
  s_firstAbsThickness[0] = m_hecManager->getBlock(0)->getFrontPlateThickness() ; 
  s_firstAbsThickness[1] = m_hecManager->getBlock(3)->getFrontPlateThickness() ; 
  s_wheel1 = s_depthSize[0]+s_depthSize[1]+s_depthSize[2];              // 816.5;
  s_wheel2 = s_depthSize[3]+s_depthSize[4]+s_depthSize[5]+s_depthSize[6]; // 961.0;
  s_betweenWheels = 40.5; // HACK!! -- can't find this through LArReadoutGeometry...
  s_startHec2 = s_wheel1 + s_betweenWheels + s_firstAbsThickness[1];
  s_hecLength = s_wheel1 + s_betweenWheels + s_wheel2;  
  s_rOuter  = m_hecManager->getBlock(0)->getOuterRadius();
  s_rInner1 = m_hecManager->getBlock(0)->getInnerRadius();
  s_rInner2 = m_hecManager->getBlock(1)->getInnerRadius();

}


//============================================================================================

    LArG4Identifier HECGeometry::CalculateIdentifier(const G4Step* a_step, 
						     const eHECGeometryType g_type,
						     int  *subgap) 

    {
   
      if (!m_hecManager){
	StoreGateSvc* detStore = StoreGate::pointer("DetectorStore");
	if (detStore->retrieve(m_hecManager)!=StatusCode::SUCCESS){
	  throw std::runtime_error("HEC::HECGeometry Can't find HECDetectorManager!");
	}
      }

      if (!m_hecManager){
	throw std::runtime_error("HEC::HECGeometry Can't find HECDetectorManager!");
      }
         
      // Get the pre-step point; Get global (atlas) and local (HEC mother) Positions
      G4StepPoint* pre_step_point = a_step->GetPreStepPoint();
      Point3D<double>  globalPosition = pre_step_point->GetPosition();  

      G4TouchableHistory* theTouchable = (G4TouchableHistory*) (pre_step_point->GetTouchable());
      G4String volumeName = theTouchable->GetVolume()->GetName() ;

      // Do the following only once - to determine the G4 history-depth of the wheel coordinates	
      // We also want to know whether the Wheels are imbedded in a HEC Mother or whether
      // the Front and Rear Wheels have been placed (independently) directly into the
      // Cryostat Liquid Argon     

      int g4Depth = theTouchable->GetHistoryDepth();
      if (!m_depthHist) 
	{
	  MsgStream log(Athena::getMessageSvc(),"LArG4HEC::HECGeometry" );
	  log << MSG::INFO << " This should be done once only:  " ;
	  for (int myVol=g4Depth; myVol>0; myVol--)
	    {
	      if ((theTouchable->GetHistory()->GetVolume(myVol)->GetName())== "LArMgr::LAr::HEC::LiquidArgon")
		{ m_g4historyDepth=myVol; 
		  m_depthHist=true;  
		  log << MSG::INFO << " G4Depth of HEC::LiquidArgon " << m_g4historyDepth << endmsg;
		  if ((theTouchable->GetHistory()->GetVolume(m_g4historyDepth-1)->GetName())== "LArMgr::LAr::HEC::Mother")
		    {m_withMother=true;    log << MSG::INFO << "The HEC Wheel is embedded in a HEC Mother" << endmsg; }
		  else {m_withMother=false;    log << MSG::INFO << "The HEC Wheel is independent (no Mother)" << endmsg; }
		  break;}	
	      else if ((theTouchable->GetHistory()->GetVolume(myVol)->GetName())== "LArMgr::LAr::HEC::Mother")
		{ m_g4historyDepth=myVol+1;  
		  m_depthHist=true;  
		  m_withMother=true;
		  log << MSG::INFO << " G4Depth of HEC::LiquidArgon " << m_g4historyDepth  
		      << "  The HEC Wheel is embedded in a HEC Mother" << endmsg; 
		  break;}	
	    }
	}

      // Now we know that no matter how the HEC is nested (ie with or without overall Mother);
      // m_g4historyDepth is the G4 history depth of the HEC Wheel coordinates (ie: LAr::HEC::LiquidArgon)

      // Next we need to be able to handle things regardless whether the Wheels are
      // imbedded in a HEC Mother or whether they are placed independently into the cryostat:

      m_inMother = false;
      if (volumeName == "LArMgr::LAr::HEC::Mother") m_inMother = true;

      // This will give us the hit-position in the Wheel coordinates (ie in LAr::HEC::LiquidArgon):
      // But we have to be careful: We COULD be in a mother, in which case the wheel coord. is meaningless.
      G4AffineTransform wheelTransform;
      G4ThreeVector wheelPosition ;
      if (!m_inMother){
	wheelTransform = pre_step_point->GetTouchable()->GetHistory()->GetTransform(m_g4historyDepth);
	wheelPosition = wheelTransform.TransformPoint(pre_step_point->GetPosition());

      }

      // In case there IS a mother and the hit is actually IN this mother, we need those coordinates as well:
      G4AffineTransform motherTransform;
      G4ThreeVector motherPosition ;
      if (m_inMother){
	motherTransform = pre_step_point->GetTouchable()->GetHistory()->GetTransform(m_g4historyDepth-1);
	motherPosition = motherTransform.TransformPoint(pre_step_point->GetPosition());
      }

  
      // z-side sign
      int zSide = globalPosition.z()<0.  ? -1:1;



      // ------- ACTIVE and INACTIVE volumes -----------------------
      // (They are all inside a Depth (and some DM, too)) 

      if ( volumeName.contains("LArMgr::LAr::HEC::Module::Depth") )
	
	{	  	  

	  int modVol  = m_g4historyDepth+1 ; // geant depth of the HEC::Module 
	  int depthVol= m_g4historyDepth+2 ; // geant depth of the HEC::Module::Depth

	  int depthIndex   = theTouchable->GetHistory()->GetVolume(depthVol)->GetCopyNo();
	  int moduleIndex  = theTouchable->GetHistory()->GetVolume(modVol)->GetCopyNo();
	  
	  const G4AffineTransform moduleTransform= theTouchable->GetHistory()->GetTransform(modVol);
	  
	  G4ThreeVector modulePosition = moduleTransform.TransformPoint(pre_step_point->GetPosition());	  
          double moduleY               = (sqrt)(modulePosition.y()/Units::mm*modulePosition.y()/Units::mm) ;

	  if (subgap) {

	    int sliceVol= m_g4historyDepth+3 ; // geant depth of the slice
	    //int sliceIndex   = theTouchable->GetHistory()->GetVolume(sliceVol)->GetCopyNo();

	    const G4AffineTransform sliceTransform=theTouchable->GetHistory()->GetTransform(sliceVol);
	    G4ThreeVector slicePosition = sliceTransform.TransformPoint(pre_step_point->GetPosition());	  
	    G4VSolid      *sliceSolid   = theTouchable->GetHistory()->GetVolume(sliceVol)->GetLogicalVolume()->GetSolid();
	    double sliceZ               = slicePosition.z()/Units::mm ;
	    
	    G4Tubs *tubs = dynamic_cast<G4Tubs *> (sliceSolid);
	    if (tubs) {
	      double dz = tubs->GetDz()/Units::mm;
	      if (sliceZ > 0) {
		if (fabs(sliceZ)>dz/2.0) {
		  (*subgap) = 0;
		}
		else {
		  dz=1;
		  (*subgap) = 1;
		}
	      }
	      else {
		if (fabs(sliceZ)>dz/2.0) {
		  (*subgap) = 3;
		  dz=3;
		}
		else {
		  (*subgap) = 2;
		}
	      }
	    }
	  }



	  // Get: sampling, region, eta and phi:

	  //--- SAMPLING ---
	  // initialize value to something invalid, so we won't accidentally mis-assign something:
	  int sampling=-1;
	  sampling = (depthIndex==0 ? 0 : (depthIndex<3 ? 1 : (depthIndex<5 ? 2 : 3))); 
	  

	  //---REGION, ETA ---
	  int ieta = -1;
	  int iregion = ( moduleY>= s_maxval[depthIndex][1][0] ) ? 0 : 1 ;
	  for (int ie=0; ie<10; ie++)	{
	    if ( moduleY > s_minval[depthIndex][iregion][ie] &&
		 moduleY<= s_maxval[depthIndex][iregion][ie] )
	      {
		ieta = ie;  
		break; 
	      }
	  }


	  // Take care of very outside eta identifiers of hits that can
	  // sometimes be a wee bit outside the boundaries 
	  if (ieta<0){ 
	    int ietaMin[7] = {3,2,2,2,2,3,3};
	    int ietaMax[7] = {0,0,1,1,2,2,3};
	    if      (moduleY>=s_rOuter)                        ieta = ietaMax[depthIndex]; 
	    else if ((depthIndex==0 && moduleY<s_rInner1 ) ||
		     (depthIndex>0  && moduleY<s_rInner2 ))    ieta = ietaMin[depthIndex];
	  }



	  //--- PHI ---
	  int iphi=-1;
	  if(iregion == 1) iphi = moduleIndex;
	  else           
	    {
	      iphi = 2*moduleIndex;
	      if(zSide * modulePosition.x() > 0) iphi++;
	    }	  
	  
	  
	  //--- Assign identifier according the the volumes we are in:

	  if ( g_type==kWheelActive || g_type==kWheelInactive  )
	    {
	      LArG4Identifier result = LArG4Identifier();
	      
	      result       << 4           // LAr
			   << 2           // HEC
			   << 2*zSide     // z-side
			   << sampling    // sampling
			   << iregion     // region 
			   << ieta        // ieta 
			   << iphi     ;  // iphi 
		
		return result;
	    }
	  
	  else   // (has to be dead)		    
	    { 
               int itype = 2;
               if ( volumeName.contains("LArMgr::LAr::HEC::Module::Depth::FirstAbsorber") )   { 
                  itype = 1;
                  sampling = 2;
                  iphi = moduleIndex;
                  if(iregion==1) {
                     ieta += 10;
                  }
	          iphi = 2*moduleIndex;
	          if(zSide * modulePosition.x() > 0) iphi++;
                  if(ieta<0) ieta = 0;
                  if(ieta>17) ieta = 17;
                  if(depthIndex < 3) iregion = 3; else iregion = 4;
               } else {   
                  if(iregion==0) {
                       iregion=2; iphi /= 2;
                  } else { iregion=3; }// diference in regions and phi for DM
               }
#ifdef DEBUG_DEAD
	       std::cout<<"Dead phi: "<<globalPosition.getPhi() <<" index: "
			<<iphi<<" eta: "<<globalPosition.getEta() <<" ieta: "
			<<ieta<<" itype: "<<itype<<" iregion: "<<iregion<<std::endl;
	       std::cout<<"Name: "<<volumeName<<std::endl;
#endif

	      LArG4Identifier result = LArG4Identifier();
	      result       << 10          // Calo
			   << 4*zSide     // LAr
			   << itype       // DM type 
			   << sampling    // sampling
			   << iregion     // region 
			   << ieta        // ieta 
			   << iphi     ;  // iphi 
		
		return result;
	    }
	  
	}  // End of if we are in a Depth
      
      //========================================================

      //---------- All that's left are DEAD Volumes -------------------------------

      else
	{

	  // If we are not in a Depth, we have to make use of either
	  // Wheel Coordinates (in case the Wheels are independent without a Mother)
	  // or Mother Coordinates (only in case there is a Mother and the hit is actually in it)


	  // For z, r and PHI , use either mother or wheel coordinates:
	  // For ETA:
	  // Get the ideal HEC eta for the inter-moduel crack:
	  // m_hecManager->getFocalToRef() = 4270 ...that's the start of the ideal HEC envelope
	  // (no database access to this:  4277 ...is the start of the actual ideal HEC
	  // in any case; this number is only used to get some ideal eta assignment.
	  // it is NOT geometry- or alignmnet-dependent!
	  double zed    =0. ;
	  double radius =0. ;
	  double phi    =0. ;
	  int hecCopy   = -1;
	  double idealEta = -1.;
	  double idealZ   = 0.; 	  

	  if (m_inMother)  {
	    zed    = motherPosition.z();    
	    radius = motherPosition.perp(); 
	    phi    = (  (zSide<0.)    ? 
			((motherPosition.getPhi() > 0.0) ? (M_PI-motherPosition.getPhi()) : (-M_PI-motherPosition.getPhi()))
			: motherPosition.getPhi());
	    idealZ   = zed + 4277.; 
	    idealEta = -log ( tan( atan(radius/idealZ) / 2.));
	  }
	  else  {
	    hecCopy= theTouchable->GetHistory()->GetVolume(m_g4historyDepth)->GetCopyNo();
	    zed    = wheelPosition.z();    
	    radius = wheelPosition.perp(); 
	    phi    = (  (zSide<0.)    ? 
			((wheelPosition.getPhi() > 0.0) ? (M_PI-wheelPosition.getPhi()) : (-M_PI-wheelPosition.getPhi()))
			: wheelPosition.getPhi());
	    // See whether we are in Front or Rear wheel:
	    zed      = (hecCopy<1) ? (zed    ) : (zed+s_wheel1+s_betweenWheels); 
	    idealZ   = zed + 4277.; 
	    idealEta = -log ( tan( atan(radius/idealZ) / 2.));

	  }

	  // --- PHI ---
	  // For phi need to consider opposite sense for the negative z-side (that's the same for mother/wheel):
	  phi           = (phi<0) ? (phi+2.*M_PI) : phi;          
          int iphi      = int(phi*(32/M_PI));

#ifdef DEBUG_DEAD
	  std::cout<<"Dead global phi: "<<globalPosition.getPhi()
		   <<" mother phi: " << motherPosition.getPhi()
		   <<" wheel phi: "  << wheelPosition.getPhi()
		   <<" phi: "<<phi<<" index: "<<iphi<<std::endl;
#endif
	  
	  
	  //===========================================================================

	  // Now we have to be careful again, because zed is diffrent, depending
	  // on if we are inside a mother or not!

	  int type    = -1;
	  int sampling= -1;
	  int region  = -1;
          int ieta = -1;

	  // --- TYPE, SAMPLING, REGION ---

	  
          //--- outside or behind HEC:
	  if ( radius >= s_rOuter ) {
            if(idealEta < 1.7) { 
	      ieta = int((idealEta - 1.0)/0.1);
	      type=1  ; sampling=2;  region=5; 
	    } 
            else { 
	      ieta = int((idealEta - 1.7)/0.1);
	      type=1  ; sampling=3; region=0; 
	    }
          }
	  //--- behind HEC:
	  else if ( zed >= s_hecLength ) {  
	    ieta = int((idealEta - 1.7)/0.1); 
	    type=1  ; sampling=3; region=0;   
	  }
          //--- in front of HEC:
	  else if ( zed <= s_firstAbsThickness[0] && radius >s_rInner1  ) {
	    ieta = int((idealEta - 1.5)/0.1);
	    type=1  ; sampling=2;  region=3;    
	  }
          //--- in bewteen the two HECs:
	  else if ( zed >= s_wheel1 &&  zed <=s_startHec2 && radius<s_rOuter && radius >s_rInner2 ) {
	    ieta = int((idealEta - 1.5)/0.1); 
	    type=1  ; sampling=2;  region=4; 
	  }
          //--- inside the HEC or between HEC and FCAL:
	  else 
	    {  
	      type=2; 
	      
	      int iDepth =-1;
	      for (int depthIndex=0; depthIndex<7; depthIndex++) {
		if (zed <= s_depthSum[depthIndex]) { iDepth = depthIndex; break; } 
	      }
              if(iDepth < 0) { // this should not happen (previous tests on zed should guarantee, but to make Coverity happy
                 iDepth = 6;
              }

	      // for inner HEC region or HEC/FCAL region:
	      sampling = (iDepth==0 ? 0 : (iDepth<3 ? 1 : (iDepth<5 ? 2 : 3))); 
	      iphi = int(32*phi/(2.*M_PI));  
	      region = (( radius>= s_maxval[iDepth][1][0] ) ? 2 : 3 );
	      if      (region==3) ieta = int((idealEta-2.5)/0.2) ; 
	      else                ieta = int((idealEta-1.5)/0.2) ; 
	      if      (iDepth>0  && radius <= s_rInner2 ) { region = 4 ; ieta=0; iphi = int(phi*(32/M_PI));} // between HEC and FCAL
	      else if (iDepth==0 && radius <= s_rInner1 ) { region = 4 ; ieta=0; iphi = int(phi*(32/M_PI));} // between HEC and FCAL

	    }

	  
          if(ieta<0)  ieta = 0; // stupid hack, until we solve everything
          if(iphi>63) iphi = 63;

	  
	  LArG4Identifier result = LArG4Identifier();
	  result       << 10          // Calo
		       << 4*zSide     // LAr
		       << type        // type
		       << sampling    // sampling
		       << region      // region 
		       << ieta        // ieta 
		       << iphi     ;  // iphi 
	  
	  return result;
      
	}
	

    } // end calculateIdentifier

  } // namespace HEC

} // namespace LArG4
 
