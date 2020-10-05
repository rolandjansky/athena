/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CSCSensitiveDetector.h"
#include "G4Trd.hh"
#include <string>
#include <sstream>
#include "MuonSimEvent/CscHitIdHelper.h"
#include "MCTruth/TrackHelper.h"
#include "G4Geantino.hh"
#include "G4ChargedGeantino.hh"

#include "GeoPrimitives/CLHEPtoEigenConverter.h"

// construction/destruction
CSCSensitiveDetector::CSCSensitiveDetector(const std::string& name,
                                           const std::string& hitCollectionName)
  : G4VSensitiveDetector( name )
  , m_myCSCHitColl( hitCollectionName )
{
  m_muonHelper = CscHitIdHelper::GetHelper();
}

// Implemenation of memebr functions
void CSCSensitiveDetector::Initialize(G4HCofThisEvent*)
{
  if (!m_myCSCHitColl.isValid()) m_myCSCHitColl = std::make_unique<CSCSimHitCollection>();
}

G4bool CSCSensitiveDetector::ProcessHits(G4Step* aStep,G4TouchableHistory* /*ROHist*/) {

  G4Track* currentTrack = aStep->GetTrack();

  /** CSCs sensitive to charged particle only; plus energy threshold */
  auto trackDef = currentTrack->GetDefinition();
  if (trackDef->GetPDGCharge() == 0.0) {
    if (trackDef != G4Geantino::GeantinoDefinition()) return true;
    else if (trackDef == G4ChargedGeantino::ChargedGeantinoDefinition()) return true;
  }

  G4TouchableHistory* touchHist = (G4TouchableHistory*)aStep->GetPreStepPoint()->GetTouchable();
  G4ThreeVector startPos=aStep->GetPreStepPoint()->GetPosition();
  G4ThreeVector endPos=aStep->GetPostStepPoint()->GetPosition();
  double kinEnergy = aStep->GetPreStepPoint()->GetKineticEnergy();

  /** attributes of the CSC identifier construction */

  std::string stationName="";
  int stationEta=1;
  int stationPhi=1;
  int multiLayer=1;
  int wireLayer=0;

  /** hit information to be recorded */

  Amg::Vector3D HitStart = Amg::Vector3D(-1000,-1000,-1000);
  Amg::Vector3D HitEnd   = Amg::Vector3D(-1000,-1000,-1000);
  double globalTime   = -1;
  double energyDeposit= -1;
  G4int lundcode      = -1;
  // int trackid         = -1;

  /** scan geometry tree to identify the gas layer */

  bool isAssembly = false;
  for (int i = touchHist->GetHistoryDepth(); i>=0; --i) {
    std::string::size_type npos;
    std::string volName = touchHist->GetVolume(i)->GetName();

    //G4cout << "Csc SD: swimming through the tree: level "<<i-touchHist->GetHistoryDepth() << G4endl;
    //G4cout << "Csc SD: name "<<volName << G4endl;

    /** station: name, eta and phi (-> chamber!) */
    if ((npos = volName.find("av_")) != std::string::npos &&
        (npos = volName.find("impr_")) != std::string::npos)  isAssembly = true;

    if ((npos = volName.find("station")) != std::string::npos && (!isAssembly)) {

      /** station name, station eta and station phi */
      volName       = volName.substr(0,npos-2);
      int volCopyNo = touchHist->GetVolume(i)->GetCopyNo();
      stationName   = volName;
      //stationEta    = volCopyNo/100;
      // bug fix for 14.2.0 to cope with non-mirrored chambers as proposed by
      // S.Spagnolo
      stationEta    = (volCopyNo%1000)/100;
      stationPhi    = abs(volCopyNo%100);

    } else if ((npos = volName.find("CSC")) != std::string::npos && isAssembly ) {
      // vol name for Assembly components are
      // av_WWW_impr_XXX_Muon::BMSxMDTxx_pv_ZZZ_NAME
      //         where WWW is ass. istance nr.
      //               XXX is comp. imprint nr.
      //               BMSxMDTxx is the name of the comp. log.Vol.
      //                  x station sub-type; xx technology subtype
      //               ZZZ is No of order of this daugther
      //               NAME is the comp. tag (geoIdentifierTag)
      //               for CSCs it is cl[1] or cl[2]
      // copy numbers for Ass.components are =
      //               CopyNoBase(= geoIdentifierTag of the assembly) + geoIdentifierTag of the component
      //               geoIdentifierTag of the component = Job
      //               geoIdentifierTag of the assembly = (sideC*10000 +
      //                             mirsign*1000 + abs(zi)*100 + fi+1)*100000;
      //                             mirsign*1000 + abs(zi)*100 + fi+1)*100000;

      /** station name */
      std::string::size_type loc1,loc2;
      if ((loc1 = volName.find("Muon::")) != std::string::npos) {
        stationName = volName.substr(loc1+6,3); //type only
      }

      /** station eta and station phi */
      int volCopyNo = touchHist->GetVolume(i)->GetCopyNo();
      int copyNrBase = int(volCopyNo/100000);
      int sideC  = int(copyNrBase/10000);
      int zi     = int((copyNrBase%1000)/100);
      // int mirfl  = int((copyNrBase%10000)/1000);
      int fi     = int(copyNrBase%100);
      if (sideC == 1) zi = -zi;
      stationEta    = zi;
      stationPhi    = fi;

      // now get the geoIdentifierTag of the rpc components
      int gmID = 0;
      if ((loc1 = volName.find("[")) != std::string::npos) {
        if ((loc2 = volName.find("]", loc1+1)) != std::string::npos) {
          std::istringstream istrvar(volName.substr(loc1+1,loc2-loc1-1));
          istrvar>>gmID;
        }
      }
      /** chamber layer */
      multiLayer = gmID;
    } else if ((npos = volName.find("component")) != std::string::npos && (!isAssembly)) {

      /** chamber layer */
      multiLayer = touchHist->GetVolume(i)->GetCopyNo();
      if(multiLayer==3) multiLayer=2; //multilayer index
    } else if ((npos = volName.find("CscArCO2")) != std::string::npos) {

      /** the wire layer number */
      wireLayer=touchHist->GetVolume(i)->GetCopyNo();
      wireLayer+=1;
      if(wireLayer==4) wireLayer=1;
      else if(wireLayer==3) wireLayer=2;
      else if(wireLayer==2) wireLayer=3;
      else if(wireLayer==1) wireLayer=4;
      // G4cout << "CSC:::::: wireLayer  "<<wireLayer << G4endl;

      /** get the particle ID */
      G4String particle=aStep->GetTrack()->GetDefinition()->GetParticleName();
      // http://www.slac.stanford.edu/BFROOT/www/Computing/Environment/NewUser/htmlbug/node51.html #GEANT code
      if (particle=="gamma") lundcode=1;
      else if (particle=="e+")     lundcode=2;
      else if (particle=="e-")     lundcode=3;
      else if (particle=="mu+")    lundcode=5;
      else if (particle=="mu-")    lundcode=6;
      else if (particle=="pi+")    lundcode=8;
      else if (particle=="pi-")    lundcode=9;
      else if (particle=="kaon+")  lundcode=11;
      else if (particle=="kaon-")  lundcode=12;
      else if (particle=="proton") lundcode=14;
      else if (particle=="anti_proton") lundcode=15;
      else if (particle=="sigma+") lundcode=19;
      else if (particle=="sigma-") lundcode=21;
      else if (particle=="anti_sigma-") lundcode=27;
      else if (particle=="anti_sigma+") lundcode=29;
      else if (particle=="deuteron") lundcode=45;
      //else if (particle=="geantino") lundcode=999;
      //else lundcode=999;

      if (lundcode < 0)
        G4cout << "WP CSCSensitiveDetector.cxx have exceptional particlue out of selection::  "
               << particle<< G4endl;

      /** the track id */
      // trackid=aStep->GetTrack()->GetTrackID();

      /** record global time */
      globalTime = aStep->GetPreStepPoint()->GetGlobalTime();

      /** energy deposit */
      energyDeposit = aStep->GetTotalEnergyDeposit();

      /** get the transformation */
      const G4AffineTransform transform = touchHist->GetHistory()->GetTopTransform();

      /** local position of HitStart and HitEnd */
      HitStart = Amg::Hep3VectorToEigen( transform.TransformPoint(startPos) );

      /** get the end of the step as return by G4 - could be at the beginning of
          the next volume */
      HitEnd = Amg::Hep3VectorToEigen( transform.TransformPoint(endPos) );

    }
  }

  TrackHelper trHelp(aStep->GetTrack());

  /** construct the hit identifier */
  HitID CSCid = m_muonHelper->BuildCscHitId(stationName, stationPhi,
                                          stationEta, multiLayer, wireLayer);

  /** insert hit in collection */
  m_myCSCHitColl->Emplace(CSCid, globalTime, energyDeposit,
                          HitStart, HitEnd, lundcode,
                          trHelp.GetParticleLink(), kinEnergy);

  return true;
}
