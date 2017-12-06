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
  std::vector<const Muon::MdtPrepData*> m_mdts;
  AmgSymMatrix(5) m_ErrorMatrix;
  float m_charge;
  
 public:
  Tracklet() { }
  Tracklet(const TrackletSegment& ML1seg, const TrackletSegment& ML2seg, const Amg::Vector3D &p, const AmgSymMatrix(5) &ErrorMatrix, float charge);
  Tracklet(const TrackletSegment& ML1seg, const Amg::Vector3D &p, const AmgSymMatrix(5) &ErrorMatrix, float charge);

  ~Tracklet();

 
  void momentum(const Amg::Vector3D& p);
  void charge(float charge);

  int mdtChamber() const;
  int mdtChEta() const;
  int mdtChPhi() const;
  const TrackletSegment& getML1seg() const;
  const TrackletSegment& getML2seg() const;
  const Amg::Vector3D& globalPosition() const;
  const Amg::Vector3D& momentum() const;
  float alpha() const;
  float charge() const;
  const AmgSymMatrix(5)& errorMatrix() const;
  float deltaAlpha() const;
  const std::vector<const Muon::MdtPrepData*>& mdtHitsOnTrack() const;

};
#endif
