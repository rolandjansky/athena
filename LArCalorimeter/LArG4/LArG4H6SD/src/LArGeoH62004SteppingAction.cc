/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArGeoH62004SteppingAction.h"
#include "LArG4Code/ILArCalculatorSvc.h"
#include "LArG4Code/LArG4SimpleSD.h"
#include "LArG4Code/LArG4Identifier.h"
#include "G4Track.hh"
#include "G4StepPoint.hh"
#include "G4TouchableHistory.hh"
#include "G4TransportationManager.hh"
#include "G4VPhysicalVolume.hh"
#include "G4SDManager.hh"
#include "G4TrackStatus.hh"
#include "G4StepPoint.hh"

#include "boost/io/ios_state.hpp"
#include <iomanip>

namespace G4UA
{
  LArGeoH62004SteppingAction::LArGeoH62004SteppingAction(const Config& config)
    : m_yTable(config.yTable)
    , m_checkprim(config.checkprim)
    , m_printstep(config.printstep)
  {

  }

  void LArGeoH62004SteppingAction::UserSteppingAction(const G4Step * theStep)
  {

    // ADS: why are these static?

    static G4int h_num,kk;
    static G4int trackid;
    static G4ThreeVector xyz;
    static G4String vname;
    static G4Track* theTrack;
    static G4String thePrePVname;
    static G4StepPoint* tacka;
    static double z;

    // Check the primary track
    if(m_checkprim) {
      //   std::cout <<"LArGeoH62004SteppingAction::LArGeoH62004SteppingAction checking primary track "<<std::endl;

      tacka=theStep->GetPostStepPoint();
      xyz = tacka->GetPosition();
      z=xyz.z();
      theTrack = theStep->GetTrack();
      trackid = theTrack->GetTrackID();
      if(trackid > 1 || z > 9460. || z < 8005. ) return; // not a scint. region
      if( z < 9325. && z > 8015 ) return; // BPC&MWPC region
      //  if(trackid > 10 ) return;
      //  std::cout<<"LArGeoH62004SteppingAction: "<<trackid<<", part:  "<<theTrack->GetDefinition()->GetPDGEncoding()<<" : "<<xyz.x()<<" "<<xyz.y()<<" "<<xyz.z()<<std::endl;
      if( z <= 8015.) { // Veto scint. 8
        if( xyz.x()*xyz.x() + (xyz.y() - m_yTable)*(xyz.y() - m_yTable) > 900. ) {
          std::cout<<"Primary track in veto scint. H !!!!"<<std::endl;
          std::cout<<"LArGeoH62004SteppingAction: "<<trackid<<", part:  "<<theTrack->GetDefinition()->GetPDGEncoding()<<" : "<<xyz.x()<<" "<<xyz.y()<<" - "<<m_yTable<<" "<<z<<std::endl;
          G4Exception("LArGeoH62004SteppingAction","CheckPrim",EventMustBeAborted,"Primary track in veto scintillator");
        }
      }
      if( z >= 9450. || ( z >= 9325. && z <= 9335.)) {  // Scint. 6,7
        if(fabs(xyz.x()) > 35. || fabs(xyz.y() - m_yTable) > 35.) {
          std::cout<<"Primary track outside S2,3 !!!!"<<std::endl;
          std::cout<<"LArGeoH62004SteppingAction: "<<trackid<<", part:  "<<theTrack->GetDefinition()->GetPDGEncoding()<<" : "<<xyz.x()<<" "<<xyz.y()<<" - "<<m_yTable<<" "<<z<<std::endl;
          G4Exception("LArGeoH62004SteppingAction","CheckPrim",EventMustBeAborted,"Primary track outside beam scintillators");
        }
      }

    }



    // Printing for geantino scan

    if(m_printstep) {
      boost::io::ios_base_all_saver coutsave (std::cout);
      std::cout <<"LArGeoH62004SteppingAction::LArGeoH62004SteppingAction printing "<<std::endl;

      tacka=theStep->GetPreStepPoint();
      G4StepPoint* tackab=theStep->GetPostStepPoint();
      G4TouchableHistory* theTouchable =
        (G4TouchableHistory*) (tacka->GetTouchable());
      xyz = tacka->GetPosition();

      std::cout<<"----------------------------------------------------"<<std::endl;
      std::cout<<std::fixed<<std::setprecision(3)<<"Step: X= "<<std::setw(10)<<xyz.x()<<" Y= "<<std::setw(10)<<xyz.y()<<" Z= "<<std::setw(10)<<xyz.z()<<std::endl;
      G4ThreeVector localPoint = theStep->GetPreStepPoint()->GetTouchable()->GetHistory()->GetTopTransform().TransformPoint(xyz);

      G4cout<<"Local: X= "<<localPoint.x()<<" Y= "<<localPoint.y()<<
        " Z= "<<localPoint.z()<<G4endl;
      vname = tacka->GetPhysicalVolume()->GetName();
      G4cout<<"Name: "<<vname<< "  Copy n.: "<<tacka->GetPhysicalVolume()->GetCopyNo()<<G4endl;
      G4cout<<"Material: "<<tacka->GetPhysicalVolume()->GetLogicalVolume()->GetMaterial()->GetName()<<G4endl;
      std::cout<<"-------------------"<<std::endl;
      G4ThreeVector xyzb = tackab->GetPosition();

      std::cout<<"Post Step: X= "<<xyzb.x()<<" Y= "<<xyzb.y()<<" Z= "<<xyzb.z()<<std::endl;
      G4ThreeVector localPointb = theStep->GetPreStepPoint()->GetTouchable()->GetHistory()->GetTopTransform().TransformPoint(xyzb);
      std::cout<<"Post Local: X= "<<localPointb.x()<<" Y= "<<localPointb.y()<<
        " Z= "<<localPointb.z()<<std::endl;
      vname = tackab->GetPhysicalVolume()->GetName();
      std::cout<<"Post Name: "<<vname<< "  Copy n.: "<<tackab->GetPhysicalVolume()->GetCopyNo()<<std::endl;
      std::cout<<"Material: "<<tackab->GetPhysicalVolume()->GetLogicalVolume()->GetMaterial()->GetName()<<std::endl;
      std::cout<<"-------------------"<<std::endl;
      std::cout<<"Step length: "<<theStep->GetStepLength()<<std::endl;
      G4cout<<"Energy dep.: "<<theStep->GetTotalEnergyDeposit()*theStep->GetTrack()->GetWeight()<<G4endl;


      LArG4SimpleSD* lsd = 0;
      if((!strncmp(vname,"LArMgr::LAr::EMEC",17)) || (!strncmp(vname,"LArMgr::LAr::HEC::Module",24)) ) {
        lsd=(LArG4SimpleSD*)tacka->GetPhysicalVolume()->GetLogicalVolume()->GetSensitiveDetector();
      }
      if(lsd) {
        //    This is all just for verbose print out -- it doesn't seem to make a lot of sense to me...
        //     This is also the only case I can find of the requirement of the "GetCalculator" functionality...
        //     so I'm skipping it for now as part of this migration.
        //      std::cout<<"LArG4SimpleSD: "<<lsd->GetName()<<std::endl;
        //      whcalc = lsd->GetCalculator();
        //      if(whcalc){
        //        if(whcalc->Process(theStep) ){
        //          LArG4Identifier id = whcalc->identifier();
        //          if(!strncmp(vname,"LArMgr::LAr::FCAL",17))
        //            std::cout<<" Stepping sampling: "<<id[3]<<" etaBin: "<<id[4]<<" phiBin: "<<id[5]<<std::endl;
        //          else
        //            std::cout<<" Stepping sampling: "<<id[3]<<" reg.: "<<id[4]<<" etaBin: "<<id[5]<<" phiBin: "<<id[6]<<std::endl;
        //        }
        //      }
        //      std::cout<<"-------------------"<<std::endl;
      }


      G4int dubina=theTouchable->GetHistoryDepth();
      for( kk=0; kk<=dubina; kk++ )
        {
          h_num=theTouchable->GetVolume(kk)->GetCopyNo();
          vname=theTouchable->GetVolume(kk)->GetName();
          G4cout<<vname<<" "<<h_num<<G4endl;
        }

      G4cout<<"----------------------------------------------------"<<G4endl;
    }

    return;
  }
} // namespace G4UA
