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

class TrigTauTrackRoiUpdaterMT : public AthReentrantAlgorithm {

 public:
  TrigTauTrackRoiUpdaterMT(const std::string&, ISvcLocator*);
  ~TrigTauTrackRoiUpdaterMT();
  
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext&) const override;

 private:

  Gaudi::Property< float > m_z0HalfWidth {this,"z0HalfWidth",0.4,"z0 Half width for track"};
  Gaudi::Property< int > m_nHitPix {this,"nHitPix",2,"at least n hits in pixels on lead track"};
  Gaudi::Property< int > m_nSiHoles {this,"nSiHoles",2,"maximum number of Si holes on lead track"};
  Gaudi::Property< bool > m_updateEta {this,"updateEta",true,"flag to determine whether Roi eta should be updated using the track direction (true by default)"};
  Gaudi::Property< bool > m_updatePhi {this,"updatePhi",false,"flag to determine whether Roi phi should be updated using the track direction (false by default)"};

  SG::ReadHandleKey< TrigRoiDescriptorCollection > m_roIInputKey {this,"RoIInputKey","InputRoI","Input RoI key name"};
  SG::ReadHandleKey< TrackCollection > m_tracksKey { this, "fastTracksKey", "fasttracks", "fast tracks in view" };
  SG::WriteHandleKey< TrigRoiDescriptorCollection > m_roIOutputKey {this,"RoIOutputKey","InViewRoI","Output RoI Collection Key"};

};
#endif
