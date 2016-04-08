/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MSVERTEXUTILS_TRACKLET_H
#define MSVERTEXUTILS_TRACKLET_H

#include <vector>
#include "MSVertexUtils/TrackletSegment.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "EventPrimitives/EventPrimitives.h"


//HV Track Class
class Tracklet {
  TrackletSegment m_ml1seg,m_ml2seg;
  Amg::Vector3D m_momentum;
  Amg::Vector3D m_pos;   
  std::vector<Muon::MdtPrepData*> m_mdts;
  AmgSymMatrix(5) m_ErrorMatrix;
  float m_charge;
  
 public:
  Tracklet() { }
  Tracklet(TrackletSegment ML1seg, TrackletSegment ML2seg, const Amg::Vector3D &p, const AmgSymMatrix(5) &ErrorMatrix, float charge);
  Tracklet(TrackletSegment ML1seg, const Amg::Vector3D &p, const AmgSymMatrix(5) &ErrorMatrix, float charge);

  ~Tracklet();

 
  void momentum(Amg::Vector3D p);
  void charge(float charge);

  int mdtChamber();
  int mdtChEta();
  int mdtChPhi();
  TrackletSegment getML1seg();
  TrackletSegment getML2seg();
  Amg::Vector3D globalPosition();
  Amg::Vector3D momentum();
  float alpha();
  float charge();
  AmgSymMatrix(5) errorMatrix();
  float deltaAlpha();
  std::vector<Muon::MdtPrepData*> mdtHitsOnTrack();

};
#endif
