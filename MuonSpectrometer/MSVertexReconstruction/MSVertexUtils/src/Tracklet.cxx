/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MSVertexUtils/Tracklet.h"
  
Tracklet::Tracklet(TrackletSegment ML1seg, TrackletSegment ML2seg, const Amg::Vector3D &p, const AmgSymMatrix(5) &ErrorMatrix, float charge) {
  m_ml1seg=ML1seg; m_ml2seg=ML2seg; m_momentum = p; m_ErrorMatrix = ErrorMatrix; m_charge = charge; 
  m_pos = ML1seg.globalPosition(); m_mdts = ML1seg.mdtHitsOnTrack();
  for(unsigned int i=0; i<m_ml2seg.mdtHitsOnTrack().size(); ++i) m_mdts.push_back(m_ml2seg.mdtHitsOnTrack().at(i));     
}

Tracklet::Tracklet(TrackletSegment ML1seg, const Amg::Vector3D &p, const AmgSymMatrix(5) &ErrorMatrix, float charge) {
  m_ml1seg=ML1seg; m_ml2seg=ML1seg; m_momentum = p; m_ErrorMatrix = ErrorMatrix; m_charge = charge;
  m_pos = ML1seg.globalPosition(); m_mdts = ML1seg.mdtHitsOnTrack();
}


Tracklet::~Tracklet() { }


void Tracklet::momentum(Amg::Vector3D p) { m_momentum = p; return; }
void Tracklet::charge(float charge) { m_charge=charge; return; }

int Tracklet::mdtChamber() { return m_ml1seg.mdtChamber(); }
int Tracklet::mdtChEta() { return m_ml1seg.mdtChEta(); }
int Tracklet::mdtChPhi() { return m_ml1seg.mdtChPhi(); }
TrackletSegment Tracklet::getML1seg() { return m_ml1seg; }
TrackletSegment Tracklet::getML2seg() { return m_ml2seg; }
Amg::Vector3D Tracklet::globalPosition() { return m_pos; }
Amg::Vector3D Tracklet::momentum() { return m_momentum; }
float Tracklet::alpha() { return TMath::ATan2(m_momentum.perp(),m_momentum.z()); }
float Tracklet::charge() { return m_charge; }
AmgSymMatrix(5) Tracklet::errorMatrix() { return m_ErrorMatrix; }
float Tracklet::deltaAlpha() { return (m_ml1seg.alpha()-m_ml2seg.alpha()); }
std::vector<Muon::MdtPrepData*> Tracklet::mdtHitsOnTrack() { return m_mdts; }

