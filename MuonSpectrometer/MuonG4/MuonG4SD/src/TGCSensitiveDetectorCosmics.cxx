/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TGCSensitiveDetectorCosmics.h"
#include <string>
#include "CxxUtils/make_unique.h" // For make unique
#include "MuonSimEvent/TgcHitIdHelper.h"
#include "MCTruth/TrackHelper.h"
#include "G4Geantino.hh"
#include "G4ChargedGeantino.hh"

#include "GeoPrimitives/CLHEPtoEigenConverter.h"

// construction/destruction
TGCSensitiveDetectorCosmics::TGCSensitiveDetectorCosmics(const std::string& name, const std::string& hitCollectionName)
  : G4VSensitiveDetector( name )
  , m_momMag(0)
  , m_globalTime(0)
  , m_myTGCHitColl( hitCollectionName )
{
  m_muonHelper = TgcHitIdHelper::GetHelper();
}

// Implemenation of member functions
void TGCSensitiveDetectorCosmics::Initialize(G4HCofThisEvent*)
{
  if (!m_myTGCHitColl.isValid()) m_myTGCHitColl = CxxUtils::make_unique<TGCSimHitCollection>();
  // START OF COSMICS-SPECIFIC CODE
  m_mom = Amg::Vector3D(0.,0.,0.);
  m_globH = Amg::Vector3D(0.,0.,0.);
  // END OF COSMICS-SPECIFIC CODE
}

G4bool TGCSensitiveDetectorCosmics::ProcessHits(G4Step* aStep,G4TouchableHistory*) {

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

  // START OF COSMICS-SPECIFIC CODE
  // global coordinates
  G4ThreeVector globVrtx = aStep->GetPreStepPoint()->GetPosition();

  // distance of the hit from (0,0,0) vertex - calculated from the PreStepPoint (approximation)
  double dist = globVrtx.mag();
  double inv_lightspeed = 1. / CLHEP::c_light;
  double tOrigin = dist * inv_lightspeed;

  G4int trackid = aStep->GetTrack()->GetTrackID();
  m_currVertex = Amg::Hep3VectorToEigen(aStep->GetTrack()->GetVertexPosition());

  // for cosmics: only primary muon tracks - track momentum when first entering the spectrometer (one muon per event)
  if ((m_currVertex != m_vertex) && (trackid == 1)) {
    // after calculationg the momentum magnidude, normalize it
    m_mom = Amg::Hep3VectorToEigen(track->GetMomentum());
    m_momMag = m_mom.mag();
    m_mom.normalize();
    // the direction of the primary mu is used to calculate the t0, the position ot the t0, m_globH, is ONE for a track
    Amg::Vector3D globVrtxFix = Amg::Hep3VectorToEigen( globVrtx );
    double AlphaGlobal = -1*(globVrtxFix[0]*m_mom[0] + globVrtxFix[1]*m_mom[1] + globVrtxFix[2]*m_mom[2])/(m_mom[0]*m_mom[0] + m_mom[1]*m_mom[1] + m_mom[2]*m_mom[2]);
    m_globH = globVrtxFix + AlphaGlobal*m_mom;
    G4cout << "COSMICS MAIN TRACK IN THE MDT!" << G4endl;
  }
  double globalDist = sqrt((m_globH[0] - globVrtx[0])*(m_globH[0] - globVrtx[0]) +
                           (m_globH[1] - globVrtx[1])*(m_globH[1] - globVrtx[1]) +
                           (m_globH[2] - globVrtx[2])*(m_globH[2] - globVrtx[2]));
  double tof = globalDist * inv_lightspeed;
  // END OF COSMICS-SPECIFIC CODE

  // scan geometry tree to identify hit channel
  int zside(0);
  for (int i=touchHist->GetHistoryDepth();i>=0;i--) {

    std::string::size_type npos;
    std::string volName = touchHist->GetVolume(i)->GetName();

    // stationName and stationPhi
    if ((npos = volName.find("station")) != std::string::npos) {

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

    // stationEta
    } else if ((npos = volName.find("tgccomponent")) != std::string::npos) {
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
  // START OF COSMICS-SPECIFIC CODE
  m_vertex = Amg::Hep3VectorToEigen(aStep->GetTrack()->GetVertexPosition());
  // if the track m_vertex is far from (0,0,0), takes the tof, otherwise take the "usual" g4 globalTime
  ((((m_vertex.mag()) < 100) || ((fabs(globalTime - tOrigin)) < 0.1) ) ? (m_globalTime  = globalTime) : (m_globalTime = tof));
  // if m_globalTime  != globalTime and m_globalTime != tof in the output, this is due to multiple hits
  // before founding the good one (small approximation)
  // END OF COSMICS-SPECIFIC CODE

  // construct new mdt hit
  TrackHelper trHelp(aStep->GetTrack());
  m_myTGCHitColl->Emplace(TGCid,
                        m_globalTime,
                        localPosition,
                        localDireCos,
                        trHelp.GetParticleLink(),
                        aStep->GetTotalEnergyDeposit(),
                        aStep->GetStepLength());
  return true;
}

