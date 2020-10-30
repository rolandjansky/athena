/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// LArG4::MiniFCAL::MiniFCALAssignIdentifier
#include "LArG4Code/LArG4Identifier.h"
#include "MiniFCALAssignIdentifier.h"
#include "Identifier/Identifier.h"

#include "CLHEP/Geometry/Point3D.h"

#include "G4RunManager.hh"
#include "G4EventManager.hh"
#include "G4SteppingManager.hh"
// For special escaped energy processing.
#include "G4Navigator.hh"
#include "G4TransportationManager.hh"
#include "G4TouchableHandle.hh"

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
#include "GaudiKernel/IService.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/Bootstrap.h"
//#include "AthenaKernel/getMessageSvc.h"

#include "G4ios.hh"
#include <string>
#include <cmath>
#include <stdexcept>
#include <map>


namespace LArG4 {

  namespace MiniFCAL {

    MiniFCALAssignIdentifier* MiniFCALAssignIdentifier::m_instance = 0;


    MiniFCALAssignIdentifier* MiniFCALAssignIdentifier::GetInstance()
    {
      if (m_instance == nullptr) m_instance = new MiniFCALAssignIdentifier();
      return m_instance;
    }


    MiniFCALAssignIdentifier::MiniFCALAssignIdentifier()
      : m_halfLength(0),
        m_absThick(0),
        m_layThick(0),
        m_nRings(0)
    {
      StatusCode sc;
      ISvcLocator *svcLocator = Gaudi::svcLocator();

      sc = svcLocator->service("MessageSvc", m_msgsvc, true);
      if(sc!=StatusCode::SUCCESS)
        throw std::runtime_error("Error in MiniFcalAssingIdentifier, cannot access MessageSvc");

      MsgStream log(m_msgsvc, "LArG4::MiniFcalAssignIdentifier");
      log << MSG::DEBUG << "In MiniFcalAssignIdentifier - constructor; access database" << endmsg;

      IRDBAccessSvc* pAccessSvc(0);
      sc=svcLocator->service("RDBAccessSvc",pAccessSvc);
      if(sc != StatusCode::SUCCESS) {
        log << MSG::ERROR <<"Cannot locate RDBAccessSvc!!" << endmsg;
      }

      IGeoModelSvc* geoModelSvc(0);
      sc = svcLocator->service ("GeoModelSvc",geoModelSvc);
      if (sc != StatusCode::SUCCESS) {
        log << MSG::ERROR <<"Cannot locate GeoModelSvc!!" << endmsg;
      }


      std::string AtlasVersion = geoModelSvc->atlasVersion();
      std::string LArVersion = geoModelSvc->LAr_VersionOverride();

      std::string detectorKey  = LArVersion.empty() ? AtlasVersion : LArVersion;
      std::string detectorNode = LArVersion.empty() ? "ATLAS" : "LAr";

      std::string miniFcalVersion = pAccessSvc->getChildTag("MiniFcal",detectorKey,detectorNode);
      if(miniFcalVersion.empty()) {
        m_initialized = false;
        return;
      }

      //--- Get geometry information from database:
      IRDBRecordset_ptr recEnvelope = pAccessSvc->getRecordsetPtr("MiniFcalEnvelope",detectorKey,detectorNode);
      if(recEnvelope->size()==0) {
        log << MSG::ERROR << "Unable to get envelope parameters from the database" << endmsg;
      }

      IRDBRecordset_ptr recCommon = pAccessSvc->getRecordsetPtr("MiniFcalCommon",detectorKey,detectorNode);
      if(recCommon->size()==0) {
        log << MSG::ERROR << "Unable to get MiniFcalCommon from the database" << endmsg;
      }

      IRDBRecordset_ptr recRings = pAccessSvc->getRecordsetPtr("MiniFcalRings",detectorKey,detectorNode);
      if(recRings->size()==0) {
        log << MSG::ERROR << "Unable to get MiniFcalRings from the database" << endmsg;
      }

      m_halfLength = ((*recEnvelope)[0]->getDouble("DZ"))*CLHEP::mm;    // half-length of MiniFcal module
      m_absThick = ((*recCommon)[0]->getDouble("ABSORBERTHICKNESS"))*CLHEP::mm;    // half-length of MiniFcal module
      m_layThick = ((*recCommon)[0]->getDouble("LAYERTHICKNESS"))*CLHEP::mm;    // half-length of MiniFcal module

      m_nRings=recRings->size();

      for(unsigned i=0; i<recRings->size(); ++i) {
        m_ringIndexes[(*recRings)[i]->getInt("RINGNUM")] = i;
        m_ringRouter[(*recRings)[i]->getInt("RINGNUM")] = ((*recRings)[i]->getDouble("ROUTER"))*CLHEP::mm;
        m_ringRinner[(*recRings)[i]->getInt("RINGNUM")] = ((*recRings)[i]->getDouble("RINNER"))*CLHEP::mm;
        m_nWafers[(*recRings)[i]->getInt("RINGNUM")] =(*recRings)[i]->getInt("NWAFERS") ;
      }

      m_initialized = true;
    }




    //============================================================================================

    LArG4Identifier MiniFCALAssignIdentifier::CalculateIdentifier(const G4Step* a_step,
                                                                  const eMiniFCALAssignIdentifierType g_type) const

    {
      if(!m_initialized) return LArG4Identifier();

      MsgStream log(m_msgsvc, "LArG4::MiniFcalAssignIdentifier::Calculator");

      //--- Get coorditnates of G4 step

      G4StepPoint* pre_step_point = a_step->GetPreStepPoint();
      G4StepPoint* post_step_point = a_step->GetPostStepPoint();
      HepGeom::Point3D<double>  globalPosition = pre_step_point->GetPosition();
      HepGeom::Point3D<double>  globalPosition2 = post_step_point->GetPosition();

      G4TouchableHistory* theTouchable = (G4TouchableHistory*) (pre_step_point->GetTouchable());
      G4String volumeName = theTouchable->GetVolume()->GetName() ;
      G4String volumeName2 = post_step_point->GetTouchable()->GetVolume()->GetName() ;


      G4ThreeVector midPositionG = (globalPosition + globalPosition2) * 0.5;   // global atlas coordinates
      int zSide = 0;
      zSide = midPositionG.z()<0.  ? -1:1;

      //--- Assign identifier according the the volumes we are in:

      if ( g_type==kActive || g_type==kInactive  )
        {
          int g4depth = theTouchable->GetHistoryDepth();

          G4int index= -1;

          if(volumeName=="LArMgr::MiniFCAL::Layer") {
            g4depth = g4depth-1;
            index = a_step->GetPreStepPoint()->GetPhysicalVolume()->GetCopyNo();
          }
          else if(volumeName=="LArMgr::MiniFCAL::Wafer") {
            g4depth = g4depth-2;
            index = a_step->GetPreStepPoint()->GetPhysicalVolume()->GetCopyNo();
          }


          //--- Get local (ie: module) coordinates

          G4AffineTransform loc1Transform;
          G4ThreeVector loc1Position ;
          loc1Transform = theTouchable->GetHistory()->GetTransform(g4depth);
          loc1Position = loc1Transform.TransformPoint(pre_step_point->GetPosition());

          G4AffineTransform loc2Transform;
          G4ThreeVector loc2Position ;
          loc2Transform = post_step_point->GetTouchable()->GetHistory()->GetTransform(g4depth);
          loc2Position = loc2Transform.TransformPoint(post_step_point->GetPosition());

          if (g4depth!=4) log << MSG::INFO << "MinFcal local Coordinates - BIG Error!: " << volumeName << " " << g4depth << " " << endmsg;


          G4ThreeVector midPosition = (loc1Position + loc2Position) * 0.5;         // local module coordinates

          double phigrad=0.;
          phigrad = midPosition.getPhi()*(180./M_PI);
          if (phigrad<0.) phigrad = 360. + phigrad;


          // initialize the identifier
          LArG4Identifier minifcalID = LArG4Identifier();
          int ieta=-1;
          int iphi=-1;
          int idepth = -1;

          //--- find idepth - regardless of which volume we are in:
          for (int ilongi = 0; ilongi<12; ilongi++) {
            if ( midPosition.z() < (-m_halfLength+m_absThick+m_layThick) + double(ilongi)*(m_absThick+m_layThick) ) {
              idepth = ilongi;
              if (idepth==11) idepth=10; // that includes the end-plate now
              break;
            }
          }


          //--- First look at everything that is not an active wafer: --------------------------
          if(volumeName!="LArMgr::MiniFCAL::Wafer") {

            //--- find ieta:
            G4double radi=sqrt(midPosition.x()*midPosition.x() + midPosition.y()*midPosition.y());

            if (radi > m_ringRinner.at(0)) {ieta=0;}                      // everything at outer r ieta=0
            else if (radi<=m_ringRinner.at(m_nRings-2)) { ieta=m_nRings-1; }  // at inner r maximum ieta
            else {                                                   // all other r in-between
              for (int j=0; j<(m_nRings-2); j++){                      // goes from 0 to (nrings-1)
                if (radi<=m_ringRinner.at(j) && radi>m_ringRinner.at(j+1) ) { ieta=j+1;  break;}
              }
            }

            //--- find iphi:
            double delphi = 360./double(m_nWafers.at(ieta));

            if (zSide<0){
              phigrad -= 180.;
              if (phigrad<0.) phigrad = -phigrad ;
              else phigrad = -phigrad + 360. ;
            }

            for (int j=0; j<m_nWafers.at(ieta); j++){
              if (phigrad>delphi*double(j) && phigrad<=delphi*double(j+1)) { iphi=j; break;}
            }


          } // done with inactive ieta/iphi assignment


          //--- this for active wafers now - go by copy number:
          else {
            ieta = int(float(index)/100.);
            iphi=index%100 ;
          }


          //--- Check if there is anything unusual:
          if (iphi<0) log << MSG::INFO << "MiniFcal iphi problem:   Z " << volumeName << " "
                          << zSide << " " << phigrad << " " << iphi << " / ieta: " << ieta
                          << " " << m_nWafers.at(ieta) << endmsg;
          if (ieta<0||ieta>(m_nRings-1)) log << MSG::INFO << " MiniFcal ieta problem - ieta="
                                             << volumeName << " " << ieta << endmsg;


          if (idepth<0 || idepth>10) log << MSG::INFO << idepth << "MiniFcal-wrong idepth! " << midPosition.z()
                                         << " | " << volumeName << " -> " << volumeName2 << endmsg;


          // adjust idepth fo 4 samplings max:
          if      (idepth<2)              idepth=0;
          else if (idepth<8 && idepth>=2) idepth=1;
          else if (idepth>=8)             idepth=2;
          else log << MSG::INFO << "Bad handling of the sampling! "  << idepth <<endmsg;


          minifcalID   << 4           // LAr
                       << 3           // Fcal
                       << 2*zSide     // +-Z
                       << 0           // module
                       << idepth     // depth
                       << ieta        // ieta
                       << iphi     ;  // iphi



          return minifcalID;

        }	// end of if in active or inactive

      else {
        // Provision for dead material - we shoud never get here!;
        // Use an ID that's close to the standard asignment for the moment with ieta/iphi=0
        log << MSG::WARNING << " MiniFcalAssignIdentifier: Found dead material. This should NOT happen! Assign DM ID: 10-4-2-0-5-0-0 "
            << volumeName << " " << volumeName2 << endmsg;
        LArG4Identifier minifcalID = LArG4Identifier();
        minifcalID   << 10          // DM
                     << 4           // +/-LAr
                     << 2           // type (DM inside)
                     << 0           // sampling (0 or 1 in front of Fcal)
                     << 5           // region
                     << 0           // ieta
                     << 0        ;  // iphi

        return minifcalID;
      }

    } // end calculateIdentifier

  } // namespace MiniFCAL

} // namespace LArG4
