/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTAUHYPO_TrigTauTrackRoiUpdaterMT_H
#define TRIGTAUHYPO_TrigTauTrackRoiUpdaterMT_H

#include <iostream>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrkTrack/TrackCollection.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "MVAUtils/BDT.h"
#include "xAODTau/TauJetContainer.h"
#include "tauRecTools/BDTHelper.h"

class TrigTauTrackRoiUpdaterMT : public AthReentrantAlgorithm {

 public:
  TrigTauTrackRoiUpdaterMT(const std::string&, ISvcLocator*);
  ~TrigTauTrackRoiUpdaterMT();
  
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext&) const override;

 private:

  Gaudi::Property< float > m_z0HalfWidth {this,"z0HalfWidth",7.0,"z0 Half width for track"};
  Gaudi::Property< int > m_nHitPix {this,"nHitPix",2,"at least n hits in pixels on lead track"};
  Gaudi::Property< int > m_nSiHoles {this,"nSiHoles",2,"maximum number of Si holes on lead track"};
  Gaudi::Property< bool > m_updateEta {this,"updateEta",true,"flag to determine whether Roi eta should be updated using the track direction (true by default)"};
  Gaudi::Property< bool > m_updatePhi {this,"updatePhi",false,"flag to determine whether Roi phi should be updated using the track direction (false by default)"};
  Gaudi::Property< bool > m_useBDT {this,"useBDT",false,"flag to determine whether the 1st stage of the FTF chooses the track with the highest BDT score"};
  Gaudi::Property< std::string > m_BDTweights {this,"BDTweights","TrigTauRec/00-11-02/FTF_tauCore_BDT_v0.root","String with the path to the BDT file"};

  //std::unique_ptr<MVAUtils::BDT> m_reader; //!
  std::unique_ptr<tauRecTools::BDTHelper> m_reader;

  struct BDTInputVariables
  {
    float logtrk_pt{0.0}; //!
    float abstrck_z0{0.0}; //!
    float abstrk_d0{0.0}; //!
    float trk_nPiHits{0.0}; //!
    float trk_nSiHoles{0.0}; //!
    float logtrk_ratiopt{0.0}; //!
    float trk_dR{0.0}; //!
    float trk_dRtoleadtrk{0.0}; //!
    float CaloHad_pt{0.0}; //!
    float CaloEM_pt{0.0}; //!

  };

  SG::ReadHandleKey< TrigRoiDescriptorCollection > m_roIInputKey {this,"RoIInputKey","InputRoI","Input RoI key name"};
  SG::ReadHandleKey< TrackCollection > m_tracksKey { this, "fastTracksKey", "fasttracks", "fast tracks in view" };
  SG::WriteHandleKey< TrigRoiDescriptorCollection > m_roIOutputKey {this,"RoIOutputKey","InViewRoI","Output RoI Collection Key"};

  SG::ReadHandleKey< xAOD::TauJetContainer> m_tauJetKey      { this, "Key_trigTauJetInputContainer", "HLT_taujet", "input taujet container" };
  double getBDTscore(const xAOD::TauJet* tau, const Trk::Track* track, const Trk::Track* leadtrack ) const;

};
#endif
