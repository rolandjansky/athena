/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MSVERTEX_TRACKLETSEGMENT_H
#define MSVERTEX_TRACKLETSEGMENT_H

#include <vector>
#include "GeoPrimitives/GeoPrimitives.h"
#include "TMath.h"

namespace Muon{
  class MdtPrepData;
}

///New segment class for single ML segments
class TrackletSegment {
  int m_chamber;
  int m_cham_eta;
  int m_cham_phi;
  float m_alpha, m_dalpha;//angle of the segment
  float m_rErr, m_zErr;//error on the r & z coordinates
  Amg::Vector3D m_globalPosition;//GlobalPosition of the segment
  float m_chmid;//radius of the middle of the chamber
  std::vector<const Muon::MdtPrepData*> m_mdts;//vector of hits on track
  int m_pattern;
  bool m_isCombined;

 public:
  TrackletSegment();
  TrackletSegment(int ch, int cheta, int chphi, float chmid, float alpha,float dalpha, const Amg::Vector3D& gpos, float rErr,
		  float zErr,const std::vector<const Muon::MdtPrepData*>& mdts, int pattern);
  ~TrackletSegment();

  //set functions
  void clearMdt();
  void isCombined(bool iscomb);

  //get functions
  int mdtChamber() const;
  int mdtChEta() const;
  int mdtChPhi() const;
  float alpha() const;
  float alphaError() const;
  float zError() const;
  float rError() const;
  float getChMidPoint() const;
  bool isCombined() const;
  const Amg::Vector3D& globalPosition() const;
  const std::vector<const Muon::MdtPrepData*>& mdtHitsOnTrack() const;
  int getHitPattern() const;

};

#endif
