/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

#include "G4PhysicalVolumeStore.hh"
#include "G4VPhysicalVolume.hh"
#include "G4TransportationManager.hh"

namespace Units = Athena::Units;


#undef DEBUG_HEC

#undef DEBUG_HEC_OLD_DIAGNOSTIC

#undef DEBUG_DEAD

#define NUM_ETABIN 15


using HepGeom::Point3D;


namespace LArG4 {

  namespace HEC {

    HECGeometry::HECGeometry(const std::string& name, ISvcLocator * pSvcLocator)
      : base_class(name, pSvcLocator)
    {

    }

    //============================================================================================

    StatusCode HECGeometry::initialize()
    {

      if (!m_hecManager)
        {
          StoreGateSvc *detStore = StoreGate::pointer("DetectorStore");
          ATH_CHECK(detStore->retrieve(m_hecManager));
        }
      // This is VERY clumsy, but at the moment the only way to get the eta boundaries
      // for a given block from the ReadoutGeometry and at the same time have the
      // eta identifier ready for any given depth.

      for (int iblock=0; iblock<7; iblock++)  {
        m_depthSum[iblock] = 0.;
        m_depthSize[iblock] = 0.;
        for (int ireg=0; ireg<2; ireg++)     {
          for (int etaseg=0; etaseg<10; etaseg++)  {
            m_minval[iblock][ireg][etaseg] = 0.;
            m_maxval[iblock][ireg][etaseg] = 0.;
          }
        }
      }

      double depthsum_tally = 0.;
      for (int depthIndex=0; depthIndex<7; depthIndex++)
        {
          m_depthSize[depthIndex] = m_hecManager->getBlock(depthIndex)->getDepth();
          depthsum_tally += m_depthSize[depthIndex];
          if (depthIndex==3) depthsum_tally += 40.5; // FIXME YIKES! hard-coded number...!
          m_depthSum[depthIndex] = depthsum_tally  ;

          const HECLongBlock *block = m_hecManager->getBlock(depthIndex);

          int isegInner = 0;
          int isegOuter = block->getNumRadialSegments();
          const int numBlk  = block->getBlockNumber();

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
              m_minval[depthIndex][iregion][ieta]= hecRad->getMinVal();
              m_maxval[depthIndex][iregion][ieta]= hecRad->getMaxVal();
              // This is a most terrible hack to correct numbers in the database...:
              const double rInner = m_hecManager->getBlock(depthIndex)->getInnerRadius();
              const double rOuter = m_hecManager->getBlock(depthIndex)->getOuterRadius();
              if (hecRad->getMaxVal()==2027.)                    m_maxval[depthIndex][iregion][ieta]=rOuter;
              if (hecRad->getMinVal()==375. && depthIndex==0)    m_minval[depthIndex][iregion][ieta]=rInner;
              else if (hecRad->getMinVal()==478 && depthIndex>0) m_minval[depthIndex][iregion][ieta]=rInner;
            }
        }


      // And some essential numbers we'll need over and over again:
      m_firstAbsThickness[0] = m_hecManager->getBlock(0)->getFrontPlateThickness() ;
      m_firstAbsThickness[1] = m_hecManager->getBlock(3)->getFrontPlateThickness() ;
      m_wheel1 = m_depthSize[0]+m_depthSize[1]+m_depthSize[2];              // 816.5;
      m_wheel2 = m_depthSize[3]+m_depthSize[4]+m_depthSize[5]+m_depthSize[6]; // 961.0;
      m_startHec2 = m_wheel1 + m_betweenWheels + m_firstAbsThickness[1];
      m_hecLength = m_wheel1 + m_betweenWheels + m_wheel2;
      m_rOuter  = m_hecManager->getBlock(0)->getOuterRadius();
      m_rInner1 = m_hecManager->getBlock(0)->getInnerRadius();
      m_rInner2 = m_hecManager->getBlock(1)->getInnerRadius();

      if (!m_depthHist)
        {
          const G4LogicalVolume * logicalWorld = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking()->GetWorldVolume()->GetLogicalVolume();
          const G4VPhysicalVolume *thePhysicalVolume = G4PhysicalVolumeStore::GetInstance()->GetVolume("LArMgr::LAr::HEC::Mother",false);
          int level=0;
          if(this->checkDaughters(logicalWorld, thePhysicalVolume,level))
            {
              m_g4historyDepth=level+1;
              m_depthHist=true;
              m_withMother=true;
              ATH_MSG_INFO(" G4Depth of HEC::LiquidArgon " << m_g4historyDepth
                           << "  The HEC Wheel is embedded in a HEC Mother");
            }
          else
            {
              const G4VPhysicalVolume *thePhysicalVolume2 = G4PhysicalVolumeStore::GetInstance()->GetVolume("LArMgr::LAr::HEC::LiquidArgon",false);
              int level2=0;
              if(this->checkDaughters(logicalWorld, thePhysicalVolume2,level2))
                {
                  m_g4historyDepth=level2;
                  m_depthHist=true;
                  ATH_MSG_INFO(" G4Depth of HEC::LiquidArgon " << m_g4historyDepth);
                  m_withMother=false;
                  ATH_MSG_INFO("The HEC Wheel is independent (no Mother)");
                }
              else
                {
                  ATH_MSG_ERROR("Failed to find LArMgr::LAr::HEC::LiquidArgon!!");
                  return StatusCode::FAILURE;
                }
            }
        }

      return StatusCode::SUCCESS;
    }

   //============================================================================================

    bool HECGeometry::checkDaughters(const G4LogicalVolume *possibleParent, const G4VPhysicalVolume *thePhysicalVolume, int& level) const
    {
      if (possibleParent->IsDaughter(thePhysicalVolume))
        {
          ++level;
          return true;
        }
      // Otherwise Loop over the daughters
      const G4int nDaughters(possibleParent->GetNoDaughters());
      for(G4int daughter(0); daughter<nDaughters; ++daughter)
        {
          const G4VPhysicalVolume* daughterPhysVol = possibleParent->GetDaughter(daughter);
          const G4LogicalVolume* daughterLogVol = daughterPhysVol->GetLogicalVolume();
          if (this->checkDaughters(daughterLogVol, thePhysicalVolume,level))
            {
              ++level;
              return true;
            }
        }
      return false;
    }

    //============================================================================================

    LArG4Identifier HECGeometry::CalculateIdentifier(const G4Step* a_step,
                                                     const eHECGeometryType g_type,
                                                     int  *subgap) const

    {

      if (!m_hecManager){
        throw std::runtime_error("HEC::HECGeometry Can't find HECDetectorManager!");
      }

      // Get the pre-step point; Get global (atlas) and local (HEC mother) Positions
      G4StepPoint* pre_step_point = a_step->GetPreStepPoint();
      Point3D<double>  globalPosition = pre_step_point->GetPosition();

      const G4TouchableHistory* theTouchable = static_cast<const G4TouchableHistory*>(pre_step_point->GetTouchable());
      G4String volumeName = theTouchable->GetVolume()->GetName() ;

      // Do the following only once - to determine the G4 history-depth of the wheel coordinates
      // We also want to know whether the Wheels are imbedded in a HEC Mother or whether
      // the Front and Rear Wheels have been placed (independently) directly into the
      // Cryostat Liquid Argon

      // Now we know that no matter how the HEC is nested (ie with or without overall Mother);
      // m_g4historyDepth is the G4 history depth of the HEC Wheel coordinates (ie: LAr::HEC::LiquidArgon)

      // Next we need to be able to handle things regardless whether the Wheels are
      // imbedded in a HEC Mother or whether they are placed independently into the cryostat:

      static const G4String motherVolumeName("LArMgr::LAr::HEC::Mother");
      const bool inMother(volumeName == motherVolumeName);

      // This will give us the hit-position in the Wheel coordinates (ie in LAr::HEC::LiquidArgon):
      // But we have to be careful: We COULD be in a mother, in which case the wheel coord. is meaningless.
      G4AffineTransform wheelTransform;
      G4ThreeVector wheelPosition ;
      if (!inMother){
        wheelTransform = pre_step_point->GetTouchable()->GetHistory()->GetTransform(m_g4historyDepth);
        wheelPosition = wheelTransform.TransformPoint(pre_step_point->GetPosition());

      }

      // In case there IS a mother and the hit is actually IN this mother, we need those coordinates as well:
      G4AffineTransform motherTransform;
      G4ThreeVector motherPosition ;
      if (inMother){
        motherTransform = pre_step_point->GetTouchable()->GetHistory()->GetTransform(m_g4historyDepth-1);
        motherPosition = motherTransform.TransformPoint(pre_step_point->GetPosition());
      }


      // z-side sign
      int zSide = globalPosition.z()<0.  ? -1:1;



      // ------- ACTIVE and INACTIVE volumes -----------------------
      // (They are all inside a Depth (and some DM, too))

      if ( volumeName.contains("LArMgr::LAr::HEC::Module::Depth") )

        {

          const int depthVol= m_g4historyDepth+2 ; // geant depth of the HEC::Module::Depth
          const int depthIndex   = theTouchable->GetHistory()->GetVolume(depthVol)->GetCopyNo();
          const int modVol  = m_g4historyDepth+1 ; // geant depth of the HEC::Module
          const int moduleIndex  = theTouchable->GetHistory()->GetVolume(modVol)->GetCopyNo();

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
              double dz = tubs->GetZHalfLength()/Units::mm;
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
          int iregion = ( moduleY>= m_maxval[depthIndex][1][0] ) ? 0 : 1 ;
          for (int ie=0; ie<10; ie++) {
            if ( moduleY > m_minval[depthIndex][iregion][ie] &&
                 moduleY<= m_maxval[depthIndex][iregion][ie] )
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
            if      (moduleY>=m_rOuter)                        ieta = ietaMax[depthIndex];
            else if ((depthIndex==0 && moduleY<m_rInner1 ) ||
                     (depthIndex>0  && moduleY<m_rInner2 ))    ieta = ietaMin[depthIndex];
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

          if (inMother)  {
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
            zed      = (hecCopy<1) ? (zed    ) : (zed+m_wheel1+m_betweenWheels);
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
          if ( radius >= m_rOuter ) {
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
          else if ( zed >= m_hecLength ) {
            ieta = int((idealEta - 1.7)/0.1);
            type=1  ; sampling=3; region=0;
          }
          //--- in front of HEC:
          else if ( zed <= m_firstAbsThickness[0] && radius >m_rInner1  ) {
            ieta = int((idealEta - 1.5)/0.1);
            type=1  ; sampling=2;  region=3;
          }
          //--- in bewteen the two HECs:
          else if ( zed >= m_wheel1 &&  zed <=m_startHec2 && radius<m_rOuter && radius >m_rInner2 ) {
            ieta = int((idealEta - 1.5)/0.1);
            type=1  ; sampling=2;  region=4;
          }
          //--- inside the HEC or between HEC and FCAL:
          else
            {
              type=2;

              int iDepth =-1;
              for (int depthIndex=0; depthIndex<7; depthIndex++) {
                if (zed <= m_depthSum[depthIndex]) { iDepth = depthIndex; break; }
              }
              if(iDepth < 0) { // this should not happen (previous tests on zed should guarantee, but to make Coverity happy
                iDepth = 6;
              }

              // for inner HEC region or HEC/FCAL region:
              sampling = (iDepth==0 ? 0 : (iDepth<3 ? 1 : (iDepth<5 ? 2 : 3)));
              iphi = int(32*phi/(2.*M_PI));
              region = (( radius>= m_maxval[iDepth][1][0] ) ? 2 : 3 );
              if      (region==3) ieta = int((idealEta-2.5)/0.2) ;
              else                ieta = int((idealEta-1.5)/0.2) ;
              if      (iDepth>0  && radius <= m_rInner2 ) { region = 4 ; ieta=0; iphi = int(phi*(32/M_PI));} // between HEC and FCAL
              else if (iDepth==0 && radius <= m_rInner1 ) { region = 4 ; ieta=0; iphi = int(phi*(32/M_PI));} // between HEC and FCAL

            }


          if(ieta<0)  ieta = 0; // FIXME stupid hack, until we solve everything
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
