/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TGCSensitiveDetector.h"
#include "MuonSimEvent/TgcHitIdHelper.h"
#include "CxxUtils/make_unique.h" // For make unique
#include "MCTruth/TrackHelper.h"
#include "G4Geantino.hh"
#include "G4ChargedGeantino.hh"

#include <string>

#include "GeoPrimitives/CLHEPtoEigenConverter.h"

// construction/destruction
TGCSensitiveDetector::TGCSensitiveDetector(const std::string& name, const std::string& hitCollectionName)
  : G4VSensitiveDetector( name )
  , m_myTGCHitColl( hitCollectionName )
{
  m_muonHelper = TgcHitIdHelper::GetHelper();
}

// Implemenation of member functions
void TGCSensitiveDetector::Initialize(G4HCofThisEvent*)
{
  if (!m_myTGCHitColl.isValid()) m_myTGCHitColl = CxxUtils::make_unique<TGCSimHitCollection>();
}

G4bool TGCSensitiveDetector::ProcessHits(G4Step* aStep,G4TouchableHistory*) {

  G4Track* track = aStep->GetTrack();

  if (track->GetDefinition()->GetPDGCharge() == 0.0) {
    if (track->GetDefinition()!=G4Geantino::GeantinoDefinition()) return true;
    else if (track->GetDefinition()==G4ChargedGeantino::ChargedGeantinoDefinition()) return true;
  }
  G4TouchableHistory*     touchHist = (G4TouchableHistory*)aStep->GetPreStepPoint()->GetTouchable();
  G4ThreeVector           position  = aStep->GetPreStepPoint()->GetPosition();
  const G4AffineTransform trans     = track->GetTouchable()->GetHistory()->GetTopTransform();

  // fields for the TGC identifier construction
  std::string stationName;
  int stationEta(0);
  int stationPhi(0);
  int gasGap = 0;

  // TGC hit information
  double globalTime        = aStep->GetPreStepPoint()->GetGlobalTime();
  Amg::Vector3D localPosition = Amg::Hep3VectorToEigen( trans.TransformPoint(position) );
  G4ThreeVector direcos    = aStep->GetPreStepPoint()->GetMomentumDirection();
  Amg::Vector3D localDireCos  = Amg::Hep3VectorToEigen( trans.TransformAxis(direcos) );

  // scan geometry tree to identify hit channel
  int zside(0);
  bool isAssembly = false;
  for (int i=touchHist->GetHistoryDepth();i>=0;i--) {

    std::string::size_type npos;
    std::string::size_type nposStat;
    std::string volName = touchHist->GetVolume(i)->GetName();

    // check if this station is an assembly
    if ((npos = volName.find("av_")) != std::string::npos &&
        (npos = volName.find("impr_")) != std::string::npos)  isAssembly = true;

    // stationName and stationPhi
    if ((npos = volName.find("station")) != std::string::npos && (!isAssembly)) {

      stationName   = volName.substr(0,npos-2);
      int volCopyNo = touchHist->GetVolume(i)->GetCopyNo();
      if (volCopyNo > 0) {
        zside = 1;
      } else {
        zside = -1;
      }
      if (stationName.substr(2,1) == "F") {

        stationPhi    = (abs(volCopyNo%100)-1)*3;
        if (abs(volCopyNo/100) > 3) {
          stationPhi += abs(volCopyNo/100)-3;
        } else {
          stationPhi += abs(volCopyNo/100);
        }

        stationPhi -= 1;
        if (stationPhi <= 0) {
          stationPhi = 24 - stationPhi;
        }

      } else if (stationName.substr(2,1) == "E") {
        if (stationName.substr(1,1) == "4") {

          stationPhi = (abs(volCopyNo%100)-1)*3+abs(volCopyNo/100);

          if (abs(volCopyNo%100) < 4) {
            stationPhi = stationPhi - 1;
            if (stationPhi <= 0) {
              stationPhi = 21 - stationPhi;
            }
          } else if(abs(volCopyNo%100) < 7) {
            stationPhi = stationPhi - 1 - 1;
          } else {
            stationPhi = stationPhi - 2 - 1;
          }

        } else {

          stationPhi = (abs(volCopyNo%100)-1)*6+abs(volCopyNo/100);

          stationPhi -= 2;
          if (stationPhi <= 0) {
            stationPhi = 48 - stationPhi;
          }
        }
      }
    } else if ((nposStat = volName.find("impr_")) != std::string::npos &&
               (npos = volName.find("TGC")) != std::string::npos  && isAssembly ) {
      // vol name for Assembly components are
      // av_WWW_impr_XXX_Muon::BMSxMDTxx_pv_ZZZ_NAME
      //         where WWW is ass. istance nr.
      //               XXX is comp. imprint nr.
      //               BMSxMDTxx is the name of the comp. log.Vol.
      //                  x station sub-type; xx technology subtype
      //               ZZZ is No of order of this daugther
      //               NAME is the comp. tag (geoIdentifierTag)
      //               for TGCs it is stPhiJob[aaa] with
      //               aaa = 1000*stationPhi+Job;
      // copy numbers for Ass.components are =
      //                CopyNoBase(= geoIdentifierTag of the assembly) + geoIdentifierTag of the component
      //                geoIdentifierTag of the component = Job
      //                geoIdentifierTag of the assembly = (sideC*10000 +
      //                             mirsign*1000 + abs(zi)*100 + fi+1)*100000;
      //                             mirsign*1000 + abs(zi)*100 + fi+1)*100000;
      //                             mirsign = 1 if zi<0 and !mirrored; otherwise 0
      std::string::size_type loc1,loc2;
      if ((loc1 = volName.find("Muon::")) != std::string::npos) {
        stationName = volName.substr(loc1+6,3); //type only
      }

      int volCopyNo = touchHist->GetVolume(i)->GetCopyNo();
      int copyNrBase = int(volCopyNo/100000);
      int sideC  = int(copyNrBase/10000);
      int zi     = int((copyNrBase%1000)/100);
      if (sideC == 1) zi = -zi;
      if (sideC) {
        zside = -1;
      } else {
        zside = 1;
      }
      if (stationName.substr(2,1) == "F") {

        stationPhi    = (abs(copyNrBase%100)-1)*3;
        if (abs(copyNrBase/100) > 3) {
          stationPhi += abs(copyNrBase/100)-3;
        } else {
          stationPhi += abs(copyNrBase/100);
        }

        stationPhi -= 1;
        if (stationPhi <= 0) {
          stationPhi = 24 - stationPhi;
        }

      } else if (stationName.substr(2,1) == "E") {
        if (stationName.substr(1,1) == "4") {

          stationPhi = (abs(copyNrBase%100)-1)*3+abs(copyNrBase/100);

          if (abs(copyNrBase%100) < 4) {
            stationPhi = stationPhi - 1;
            if (stationPhi <= 0) {
              stationPhi = 21 - stationPhi;
            }
          } else if(abs(copyNrBase%100) < 7) {
            stationPhi = stationPhi - 1 - 1;
          } else {
            stationPhi = stationPhi - 2 - 1;
          }
        } else {

          stationPhi = (abs(copyNrBase%100)-1)*6+abs(copyNrBase/100);

          stationPhi -= 2;
          if (stationPhi <= 0) {
            stationPhi = 48 - stationPhi;
          }
        }
      }

      // now get the geoIdentifierTag of the rpc components
      int gmID = 0;
      if ((loc1 = volName.find("[")) != std::string::npos) {
        if ((loc2 = volName.find("]", loc1+1)) != std::string::npos) {
          std::istringstream istrvar(volName.substr(loc1+1,loc2-loc1-1));
          istrvar>>gmID;
        }
      }
      // stationEta, stationPhi
      stationEta = zside*int(gmID%100);
      if (gmID > 999) stationPhi = gmID/1000;

    // stationEta
    } else if ((npos = volName.find("tgccomponent")) != std::string::npos && (!isAssembly)) {
      int volCopyNo = abs(touchHist->GetVolume(i)->GetCopyNo());
      stationEta = zside*volCopyNo%100;
      if (volCopyNo > 1000) { // stationPhi overridden by the number assigned by MuonGeoModel
        stationPhi = volCopyNo/1000;
      }
    // gasGap
    } else if ((npos = volName.find("Gas Volume Layer")) != std::string::npos) {
      int volCopyNo = touchHist->GetVolume(i)->GetCopyNo();
      int iStation = atoi(stationName.substr(1,1).c_str());
      if (zside < 0) {
        if (iStation == 1)
          gasGap = 3-(volCopyNo-3)/4;
        else
          gasGap = 2-(volCopyNo-3)/4;
      } else {
//        if (iStation == 1)
          gasGap = (volCopyNo-3)/4+1;
//        else
//          gasGap = (volCopyNo-3)/4+1;
      }
    } else if ((npos = volName.find("TGCGas")) != std::string::npos) {

      std::string currentTech = volName.substr(0,4);
      int volCopyNo = touchHist->GetVolume(i)->GetCopyNo();

      if (volCopyNo != 0)
        gasGap = volCopyNo;
    }

  }

  //construct the hit identifier
  HitID TGCid = m_muonHelper->BuildTgcHitId(stationName,
                                          stationPhi,
                                          stationEta,
                                          gasGap);
  //m_muonHelper->Print(TGCid);

  // construct new tgc hit
  TrackHelper trHelp(aStep->GetTrack());
  m_myTGCHitColl->Emplace(TGCid,
                        globalTime,
                        localPosition,
                        localDireCos,
                        trHelp.GetParticleLink(),
                        aStep->GetTotalEnergyDeposit(),
                        aStep->GetStepLength(),
                        track->GetDefinition()->GetPDGEncoding(),
                        aStep->GetPreStepPoint()->GetKineticEnergy());
  return true;
}

