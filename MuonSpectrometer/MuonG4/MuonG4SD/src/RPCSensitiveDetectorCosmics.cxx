/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "RPCSensitiveDetectorCosmics.h"
#include "G4ThreeVector.hh"
#include "G4Trd.hh"
#include "MuonSimEvent/RpcHitIdHelper.h"
#include <string>
#include "G4Geantino.hh"
#include "G4ChargedGeantino.hh"

//#include "SimHelpers/DetectorGeometryHelper.h"
#include "MCTruth/TrackHelper.h"

#include "GeoPrimitives/CLHEPtoEigenConverter.h"

// construction/destruction
RPCSensitiveDetectorCosmics::RPCSensitiveDetectorCosmics(const std::string& name, const std::string& hitCollectionName, unsigned int nGasGaps)
  : G4VSensitiveDetector( name )
  , m_myRPCHitColl( hitCollectionName )
  , m_globalTime(0.)
  , m_isGeoModel(true)
  , m_momMag(0.)
{
  m_muonHelper = RpcHitIdHelper::GetHelper(nGasGaps);
}

void RPCSensitiveDetectorCosmics::Initialize(G4HCofThisEvent*)
{
  if (!m_myRPCHitColl.isValid()) m_myRPCHitColl = std::make_unique<RPCSimHitCollection>();
  if (verboseLevel>1) G4cout << "Initializing SD" << G4endl;
  // FIXME this next bit probebly only needs to be done once pre job
  // rather than once per G4Event?
/*  DetectorGeometryHelper DGHelp;
  if(  DGHelp.GeometryType("Muon") == GeoModel ){
    m_isGeoModel = true;
    if (verboseLevel>1) G4cout << "Muon Geometry is from GeoModel" << G4endl;
  } else {
    m_isGeoModel = false;
    if (verboseLevel>1) G4cout << "Muon Geometry is from pure G4" << G4endl;
  }*/
  m_mom = Amg::Vector3D(0.,0.,0.);
  m_globH = Amg::Vector3D(0.,0.,0.);
}

G4bool RPCSensitiveDetectorCosmics::ProcessHits(G4Step* aStep,G4TouchableHistory*) {
  G4Track* currentTrack = aStep->GetTrack();

  if (currentTrack->GetDefinition()->GetPDGCharge() == 0.0) {
    if (currentTrack->GetDefinition()!=G4Geantino::GeantinoDefinition()) return true;
    // else if (currentTrack->GetDefinition()==G4ChargedGeantino::ChargedGeantinoDefinition()) return true;
  }

  G4TouchableHistory*     touchHist = (G4TouchableHistory*)aStep->GetPreStepPoint()->GetTouchable();
  G4ThreeVector           position  = aStep->GetPreStepPoint()->GetPosition();
  G4ThreeVector       postPosition  = aStep->GetPostStepPoint()->GetPosition();
  const G4AffineTransform trans     = currentTrack->GetTouchable()->GetHistory()->GetTopTransform(); // from global to local

  // necessary to assign correct identifiers
  int rpcIsRotated      = 0;
  // int layer             = 0;

  std::string tech;
  // fields for the RPC identifier construction
  std::string stationName;
  int         stationEta= 0;
  int         stationPhi= 0;
  int         doubletR= 0;
  int         doubletZ= 0;
  int         doubletPhi= 0;
  int         gasGap= 0;
  // int         eta_inversed=0;
  // int         phi_inversed=0;
  int         station_rotated=0; // tells us if the station was rotated before being positioned.
                                 // if =1 we have to correct some IDs
  int         zNeg_original=0; // tells if the station at z<0 was obtained duplicating a station at z>0

  // RPC hit information
  double     globalTime    = aStep->GetPreStepPoint()->GetGlobalTime();
  Amg::Vector3D localPosition = Amg::Hep3VectorToEigen( trans.TransformPoint(position) );
  Amg::Vector3D localPostPosition = Amg::Hep3VectorToEigen( trans.TransformPoint(postPosition) );

  // global coordinates
  G4ThreeVector globVrtx = aStep->GetPreStepPoint()->GetPosition();

  // distance of the hit from (0,0,0) vertex - calculated from the PreStepPoint (approximation)
  // double dist = globVrtx.mag();
  double lightspeed = 299.792458; /* in vacuo speed of light [mm/ns] */
  // double tOrigin = dist / lightspeed;

  G4int trackid = aStep->GetTrack()->GetTrackID();
  m_currVertex = Amg::Hep3VectorToEigen( aStep->GetTrack()->GetVertexPosition() );

  // for cosmics: only primary muon tracks - track momentum when first entering the spectrometer (one muon per event)
  if ((m_currVertex != m_vertex) && (trackid == 1)) {
    // after calculationg the momentum magnidude, normalize it
    m_mom = Amg::Hep3VectorToEigen( currentTrack->GetMomentum() );
    m_momMag = m_mom.mag();
    m_mom.normalize();
    // the direction of the primary mu is used to calculate the t0, the position ot the t0, m_globH, is ONE for a track
    Amg::Vector3D globVrtxFix = Amg::Hep3VectorToEigen( globVrtx );
    double AlphaGlobal = -1*(globVrtxFix[0]*m_mom[0] + globVrtxFix[1]*m_mom[1] + globVrtxFix[2]*m_mom[2])/(m_mom[0]*m_mom[0] + m_mom[1]*m_mom[1] + m_mom[2]*m_mom[2]);
    m_globH = globVrtxFix + AlphaGlobal*m_mom;
  }
  double globalDist = sqrt((m_globH[0] - globVrtx[0])*(m_globH[0] - globVrtx[0]) +
                           (m_globH[1] - globVrtx[1])*(m_globH[1] - globVrtx[1]) +
                           (m_globH[2] - globVrtx[2])*(m_globH[2] - globVrtx[2]));
  double tof = globalDist / lightspeed;

  int mydbZ=0;
  int mydbPMod=0;
  int mydbP=0;
  bool isAssembly = false;
  // scan geometry tree to identify the hit channel
  for (int i=touchHist->GetHistoryDepth();i>=0;i--) {

    std::string::size_type npos;
    std::string volName = touchHist->GetVolume(i)->GetName();
    std::string num=volName.substr(3,2);
    if(num[0]==' ') num[0]=0;

    // check if this station is an assembly
    if ((npos = volName.find("av_")) != std::string::npos &&
        (npos = volName.find("impr_")) != std::string::npos)  isAssembly = true;

    // stationName, stationEta, stationPhi
    if ((npos = volName.find("station")) != std::string::npos  && (!isAssembly)) {

      stationName = volName.substr(0,npos-1);

      int volCopyNo = touchHist->GetVolume(i)->GetCopyNo();

      if(abs(volCopyNo/1000)==1){
        zNeg_original=1;
        volCopyNo=volCopyNo%1000;
      }

      stationEta = volCopyNo/100;
      stationPhi = abs(volCopyNo%100);

      if(stationEta<0&&!zNeg_original) station_rotated=1;

      // stationName, stationEta, stationPhi
    } else if ((npos = volName.find("RPC")) != std::string::npos && isAssembly) {
      // vol name for Assembly components are
      // av_WWW_impr_XXX_Muon::BMSxMDTxx_pv_ZZZ_NAME
      //         where WWW is ass. istance nr.
      //               XXX is comp. imprint nr.
      //               BMSxMDTxx is the name of the comp. log.Vol.
      //                  x station sub-type; xx technology subtype
      //               ZZZ is the comp. number of order
      //               NAME is the comp. tag (geoIdentifierTag)
      //                    for RPCs it is SwapdbPdbRdbZ[aaa]
      //                    with aaa = doubletZ + doubletR*100 + dbphi*1000;
      //                         aaa = -aaa if iswap == -1
      //                         dbphi = 1 always but for S1 or S2 at doubletPhi = 2
      // copy numbers for Ass.components are =
      //                CopyNoBase(= geoIdentifierTag of the assembly) + geoIdentifierTag of the component
      //                geoIdentifierTag of the component = Job
      //                geoIdentifierTag of the assembly = (sideC*10000 +
      //                             mirsign*1000 + abs(zi)*100 + fi+1)*100000;
      //                             mirsign = 1 if zi<0 and !mirrored; otherwise 0
      std::string::size_type loc1,loc2;
      if ((loc1 = volName.find("Muon::")) != std::string::npos) {
        stationName = volName.substr(loc1+6,4); //type and subtype
      }

      int volCopyNo = touchHist->GetVolume(i)->GetCopyNo();
      int copyNrBase = int(volCopyNo/100000);
      int sideC  = int(copyNrBase/10000);
      int zi     = int((copyNrBase%1000)/100);
      int mirfl  = int((copyNrBase%10000)/1000);
      int fi     = int(copyNrBase%100);
      if (sideC == 1) zi = -zi;
      stationEta    = zi;
      stationPhi    = fi;
      zNeg_original = mirfl;

      if(stationEta<0&&!zNeg_original) station_rotated=1;
      tech=volName.substr(npos,5);

      // now get the geoIdentifierTag of the rpc components
      int gmID = 0;
      if ((loc1 = volName.find('[')) != std::string::npos) {
        if ((loc2 = volName.find(']', loc1+1)) != std::string::npos) {
          std::istringstream istrvar(volName.substr(loc1+1,loc2-loc1-1));
          istrvar>>gmID;
        }
      }
      int kk=gmID;

      if (kk < 0) rpcIsRotated=1;

      doubletR =(abs(kk)%1000)/100;
      mydbZ    = abs(int(kk%10));
      mydbPMod = abs(int(kk/1000));
      // doubleR, doubletZ
    } else if ((npos = volName.find("rpccomponent")) != std::string::npos && (!isAssembly)) {

      tech=volName.substr(npos-5,5);

      std::string::size_type loc1,loc2;
      int gmID = 0;
      if ((loc1 = volName.find('[')) != std::string::npos) {
        if ((loc2 = volName.find(']', loc1+1)) != std::string::npos) {
          std::istringstream istrvar(volName.substr(loc1+1,loc2-loc1-1));
          istrvar>>gmID;
        }
      }
      mydbZ    = abs(int(gmID%10));
      mydbPMod = abs(int(gmID/1000));

      int kk=touchHist->GetVolume(i)->GetCopyNo();

      if (kk < 0) rpcIsRotated=1;

      doubletR=(abs(kk)%1000)/100;

      // doubleR
    } else if ((npos = volName.find("rpccomponent")) != std::string::npos && (!isAssembly)) {

      std::string::size_type loc1,loc2;
      int gmID = 0;
      if ((loc1 = volName.find('[')) != std::string::npos) {
        if ((loc2 = volName.find(']', loc1+1)) != std::string::npos) {
          std::istringstream istrvar(volName.substr(loc1+1,loc2-loc1-1));
          istrvar>>gmID;
        }
      }
      mydbZ    = abs(int(gmID%10));
      mydbPMod = abs(int(gmID/1000));

      int kk=touchHist->GetVolume(i)->GetCopyNo();

      if (kk < 0) rpcIsRotated=1;

      doubletR=(abs(kk)%1000)/100;

    // gasGap
    } else if ((npos = volName.find("layer")) != std::string::npos) {

      int copyNo = touchHist->GetVolume(i)->GetCopyNo();

      if (copyNo == 1) {
        rpcIsRotated ? gasGap = 2 : gasGap = 1;
      } else if (copyNo ==2) {
        rpcIsRotated ? gasGap = 1 : gasGap = 2;
      }

      // doubletPhi
    } else if((npos = volName.find("gas volume")) != std::string::npos) {

      int copyNo = touchHist->GetVolume(i)->GetCopyNo();
      if (copyNo == 0) {
        doubletPhi = 1;
      } else if (copyNo == 10) {
        doubletPhi = 2;
      }
      mydbP = doubletPhi;
      int ngap_in_s=0;
      int nstrippanel_in_s=0;
      std::string::size_type loc1;
      if ((loc1 = volName.find("gg_in_s")) != std::string::npos) {
        std::istringstream istrvar(volName.substr(loc1-1,1));
        istrvar>>ngap_in_s;
      }
      if ((loc1 = volName.find("sp_in_s")) != std::string::npos) {
        std::istringstream istrvar(volName.substr(loc1-1,1));
        istrvar>>nstrippanel_in_s;
      }
      if (ngap_in_s == 1 && nstrippanel_in_s == 2){
        if(localPosition.y()>0) mydbP=2;
        else mydbP=1;
      } else if (ngap_in_s == 1 && nstrippanel_in_s == 1){
        mydbP = mydbPMod;
      }
    }
  }

  ////////////////////////////////////////////////
  // correct wrong IDs due to station rotation. only doubletZ and doubletPhi are affected at this point
  ////////////////////////////////////////////////

  if(station_rotated){
    // doubletPhi correction
    // note that this is correct also for ribs chambers
    mydbP++;
    if (mydbP>2) mydbP=1;
    // doubletZ correction: specialStations have already been correted
    // the others do not need any correction, because doubletZ increases with |Z|, thus not
    // changing in the rotation

    // strip numbering: if the station is rotated both eta and phi directions get inversed.
    // commented out for geomodel!

    if(!m_isGeoModel){
      localPosition.z() = -localPosition.z();
      localPosition.y() = -localPosition.y();
      localPostPosition.z() = -localPostPosition.z();
      localPostPosition.y() = -localPostPosition.y();
    }
  }

  // further correction on the eta direction due to rpc component rotation
  // commented for geomodel!
  if(!rpcIsRotated&&!m_isGeoModel) {
    localPosition.z() = -localPosition.z();
    localPostPosition.z() = -localPostPosition.z();
  }

  /////////////////////////////////////

  // now we have the position in the gas gap, with correct axis orientation, and the offlineIDs *of the strip panel*
  // nstrip is supposed to be calculated by RPC_Digitizer.

  // construct two (one in eta and one in phi) new RPC hits and store them in hit collection

  //construct the hit identifiers

  if (verboseLevel>1) {
    G4cout << "hit in station "<<stationName<< " on technology "<<tech << G4endl;
    G4cout << "constructing ids (stName, stEta, stPhi, dr, dZ, dPhi)= "<<stationName<< " "<< stationEta<<" " << stationPhi<< " "<<doubletR<< " "<< doubletZ<< " "<<doubletPhi << G4endl;
  }

  HitID RPCid_eta = m_muonHelper->BuildRpcHitId(stationName, stationPhi, stationEta,
                                              mydbZ, doubletR, gasGap, mydbP,0);

  HitID RPCid_phi = m_muonHelper->BuildRpcHitId(stationName, stationPhi, stationEta,
                                              mydbZ, doubletR, gasGap, mydbP,1);

  // retrieve track barcode
  TrackHelper trHelp(aStep->GetTrack());

  //construct new rpc hit
  m_vertex = Amg::Hep3VectorToEigen( aStep->GetTrack()->GetVertexPosition() );

  // if the track m_vertex is far from (0,0,0), takes the tof, otherwise take the "usual" g4 globalTime
  (((m_vertex.mag()) < 100) ? (m_globalTime  = globalTime) : (m_globalTime = tof));

  m_myRPCHitColl->Emplace(RPCid_eta, m_globalTime,
                          localPosition, trHelp.GetParticleLink(), localPostPosition,
                          aStep->GetTotalEnergyDeposit(),
                          aStep->GetStepLength(),
                          currentTrack->GetDefinition()->GetPDGEncoding(),
                          aStep->GetPreStepPoint()->GetKineticEnergy());
  m_myRPCHitColl->Emplace(RPCid_phi, m_globalTime,
                        localPosition, trHelp.GetParticleLink(), localPostPosition,
                        aStep->GetTotalEnergyDeposit(),
                        aStep->GetStepLength(),
                        currentTrack->GetDefinition()->GetPDGEncoding(),
                        aStep->GetPreStepPoint()->GetKineticEnergy());

  return true;
}
