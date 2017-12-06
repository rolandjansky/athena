/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MSVertexUtils/Tracklet.h"
  
Tracklet::Tracklet(const TrackletSegment& ML1seg, const TrackletSegment& ML2seg, const Amg::Vector3D &p, const AmgSymMatrix(5) &ErrorMatrix, float charge) {
  m_ml1seg=ML1seg; m_ml2seg=ML2seg; m_momentum = p; m_ErrorMatrix = ErrorMatrix; m_charge = charge; 
  m_pos = ML1seg.globalPosition(); m_mdts = ML1seg.mdtHitsOnTrack();
  for(unsigned int i=0; i<m_ml2seg.mdtHitsOnTrack().size(); ++i) m_mdts.push_back(m_ml2seg.mdtHitsOnTrack().at(i));     
}

Tracklet::Tracklet(const TrackletSegment& ML1seg, const Amg::Vector3D &p, const AmgSymMatrix(5) &ErrorMatrix, float charge) {
  m_ml1seg=ML1seg; m_ml2seg=ML1seg; m_momentum = p; m_ErrorMatrix = ErrorMatrix; m_charge = charge;
  m_pos = ML1seg.globalPosition(); m_mdts = ML1seg.mdtHitsOnTrack();
}


Tracklet::~Tracklet() { }


void Tracklet::momentum(const Amg::Vector3D& p) { m_momentum = p; return; }
void Tracklet::charge(float charge) { m_charge=charge; return; }

int Tracklet::mdtChamber() const { return m_ml1seg.mdtChamber(); }
int Tracklet::mdtChEta() const { return m_ml1seg.mdtChEta(); }
int Tracklet::mdtChPhi() const { return m_ml1seg.mdtChPhi(); }
const TrackletSegment& Tracklet::getML1seg() const { return m_ml1seg; }
const TrackletSegment& Tracklet::getML2seg() const { return m_ml2seg; }
const Amg::Vector3D& Tracklet::globalPosition() const { return m_pos; }
const Amg::Vector3D& Tracklet::momentum() const { return m_momentum; }
float Tracklet::alpha() const { return TMath::ATan2(m_momentum.perp(),m_momentum.z()); }
float Tracklet::charge() const { return m_charge; }
const AmgSymMatrix(5)& Tracklet::errorMatrix() const { return m_ErrorMatrix; }
float Tracklet::deltaAlpha() const { return (m_ml1seg.alpha()-m_ml2seg.alpha()); }
const std::vector<const Muon::MdtPrepData*>& Tracklet::mdtHitsOnTrack() const { return m_mdts; }

