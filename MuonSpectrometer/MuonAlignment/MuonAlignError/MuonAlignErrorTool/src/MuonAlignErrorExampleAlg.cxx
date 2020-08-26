/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonAlignErrorTool/MuonAlignErrorExampleAlg.h"
#include "MuonAlignErrorBase/AlignmentDeviation.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include <iostream>

using namespace MuonAlign;

MuonAlignErrorExampleAlg::MuonAlignErrorExampleAlg (const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_debug(nullptr),
  m_cham_per_dev(nullptr),
  m_dev_per_track(nullptr) {
}

StatusCode MuonAlignErrorExampleAlg::initialize () {

  m_debug = new TFile("muonalignerrordebug.root", "RECREATE");
  m_debug->cd();
  m_cham_per_dev = new TH1F("cham_per_dev", "# of chamber layers per alignment deviation", 10, 0., 10.); 
  m_dev_per_track = new TH1F("dev_per_track", "# of chamber layers per alignment deviation", 10, 0., 10.); 

  ATH_CHECK(m_idHelperSvc.retrieve());

  return StatusCode::SUCCESS;
}

StatusCode MuonAlignErrorExampleAlg::execute () {
  StatusCode sc;
  const ::TrackCollection* tracks = 0;
  sc = sgSvc()->retrieve(tracks, "MuonSpectrometerTracks"); // for example, should be a job option
  if(sc.isFailure()) return sc;

  // LOOP ON MUON TRACKS //
  for (::TrackCollection::const_iterator it=tracks->begin(), end=tracks->end(); it!=end; it++) {
    
    const Trk::Track* track = *it;
    muonTrack(track);

  }

  return StatusCode::SUCCESS;
}

void MuonAlignErrorExampleAlg::muonTrack (const Trk::Track* track) const {
  // Compute and verbose the alignment deviations for a given track

  ATH_MSG_DEBUG("Processing track with momentum " << track->trackParameters()->front()->momentum().mag()/1000. << " GeV and transverse momentum " << track->trackParameters()->front()->momentum().perp()/1000 << " GeV");

  std::vector<Trk::AlignmentDeviation*> deviations;
  m_alignErrorTool->makeAlignmentDeviations(*track, deviations);


  // DEBUG
  m_dev_per_track->Fill(deviations.size());
  std::vector<const Trk::RIO_OnTrack*> hits;

  // DO SOME CHECKS AND THEN CLEAN UP
  for (std::vector<Trk::AlignmentDeviation*>::iterator it(deviations.begin()), end(deviations.end()); it!=end; ++it) {

    (*it)->getListOfHits(hits);

    std::set<std::string> myidset_all;
    for(std::vector<const Trk::RIO_OnTrack*>::iterator jt(hits.begin()), end(hits.end()); jt!=end; ++jt) {
        // JOCHEN WAY
        Identifier myid = ((const Trk::RIO_OnTrack*)(*jt))->identify();

        if (!(m_idHelperSvc->isMM(myid)||m_idHelperSvc->issTgc(myid))) {
          // CAMILLA WAY
          MuonCalib::MuonFixedId calibId = m_idTool->idToFixedId(myid);
          if (!calibId.isValid()) {
            continue;
          }
          std::string completename = hardwareName(calibId);
          myidset_all.insert(completename.substr(0,2));
        } else myidset_all.insert(m_idHelperSvc->toStringStation(myid));
    }
    m_cham_per_dev->Fill(myidset_all.size());

    // cleanup
    delete (*it);

  }
}

StatusCode MuonAlignErrorExampleAlg::finalize () {

  m_debug->Write();
  //m_cham_per_dev->Write();
  //m_dev_per_track->Write();
  m_debug->Close();

  return StatusCode::SUCCESS;
}


////////////////////////////
// RECOGNIZE STATION NAME //
////////////////////////////
inline std::string MuonAlignErrorExampleAlg::hardwareName(MuonCalib::MuonFixedId calibId) const {
  if (sector(calibId)==13) {
    if (calibId.eta()== 7 && calibId.stationName()==5) return "BOE1A13"; // BOE1A13 not BOL7A13
    if (calibId.eta()==-7 && calibId.stationName()==5) return "BOE1C13"; // BOE1C13 not BOL7C13
  }
  std::string etaString = "0";
  etaString[0] += std::abs(hardwareEta(calibId));
  return calibId.stationNameString() + etaString + side(calibId) + sectorString(calibId);
}

inline std::string MuonAlignErrorExampleAlg::side(MuonCalib::MuonFixedId calibId) const {
  return calibId.eta()>0 ? "A" : calibId.eta()<0 ? "C" : "B";
}

inline std::string MuonAlignErrorExampleAlg::sectorString(MuonCalib::MuonFixedId calibId) const  {
  std::string ret = "00";
  int sec = sector(calibId);
  if (sec >= 10) {
    ret[0] = '1';
    sec -= 10;
  }
  ret[1] += sec;
  return ret;
}

inline int MuonAlignErrorExampleAlg::sector (MuonCalib::MuonFixedId calibId) const {
  return isSmallSector(calibId) ? 2*calibId.phi() : 2*calibId.phi()-1;
}

inline bool MuonAlignErrorExampleAlg::isSmallSector (MuonCalib::MuonFixedId calibId) const {
  switch (calibId.stationName()) {
    case 2: return true;
    case 4: return true;
    case 6: return true;
    case 7: return true;
    case 9: return true;
    case 10: return true;
    case 11: return true;
    case 17: return true;
    case 20: return true;
    case 23: return true;
    case 24: return true;
    case 33: return true;
    default: return false;
  }
}

inline int MuonAlignErrorExampleAlg::hardwareEta(MuonCalib::MuonFixedId calibId) const {
  switch (calibId.stationName()) {
    case 3:
    {
       if (sector(calibId)==13) {
          switch (calibId.eta()) {
             case 4: return 5;
             case 5: return 6;
             case 6: return 7;
             case -4: return -5;
             case -5: return -6;
             case -6: return -7;
          }
       }
       return calibId.eta();
     }
     case 5:
     {
       if (sector(calibId)==13) {
       if (calibId.eta()== 7) return 1; // BOE1A13 not BOL7A13
       if (calibId.eta()==-7) return -1; // BOE1C13 not BOL7C13
     }
     return calibId.eta();
    }
    case 10: return calibId.eta()>0 ? calibId.eta()*2-1 : calibId.eta()*2+1;
    case 11: return calibId.eta()*2;
    case 15:
      {
        if ((sector(calibId) == 1) || (sector(calibId) == 9)) {
          switch (calibId.eta()) {
            case 4: return 5;
            case 5: return 4;
            case -4: return -5;
            case -5: return -4;
          }
        }
        return calibId.eta();
      }
    case 16:
      {
        if ((sector(calibId) == 5) && (calibId.eta() == 1)) return 2;
        if ((sector(calibId) == 5) && (calibId.eta() == -1)) return -2;
        return calibId.eta();
      }
    default: return calibId.eta();
  }
}
