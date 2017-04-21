/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MSVertexUtils/TrackletSegment.h"

#include "MuonPrepRawData/MdtPrepDataContainer.h"

TrackletSegment::TrackletSegment() {
  m_chamber=999;  m_cham_eta=999;  m_cham_phi=999;  m_alpha=-999;  m_dalpha=-999;  m_rErr=-999;
  m_zErr=-999;  m_chmid=0;  m_mdts.clear();  m_pattern=0;  m_isCombined=false;
}

TrackletSegment::TrackletSegment(int ch, int cheta, int chphi, float chmid, float alpha,float dalpha, const Amg::Vector3D& gpos, 
				 float rErr, float zErr,const std::vector<const Muon::MdtPrepData*>& mdts, int pattern) {
  m_chamber=ch; m_cham_eta=cheta; m_cham_phi=chphi; m_chmid=chmid; m_alpha=alpha; m_dalpha=dalpha; m_globalPosition=gpos;
  m_rErr=rErr; m_zErr=zErr; m_mdts=mdts; m_pattern=pattern; m_isCombined=false;
}

TrackletSegment::~TrackletSegment() { }

//set functions
void TrackletSegment::clearMdt() { m_mdts.clear(); return; }
void TrackletSegment::isCombined(bool iscomb) { m_isCombined=iscomb; return; }


//get functions
int TrackletSegment::mdtChamber() const { return m_chamber; }
int TrackletSegment::mdtChEta() const { return m_cham_eta; }
int TrackletSegment::mdtChPhi() const { return m_cham_phi; }
float TrackletSegment::alpha() const { return m_alpha; }
float TrackletSegment::alphaError() const { return m_dalpha; }
float TrackletSegment::zError() const { return m_zErr; }
float TrackletSegment::rError() const { return m_rErr; }
float TrackletSegment::getChMidPoint() const { return m_chmid; }
bool TrackletSegment::isCombined() const { return m_isCombined; }
const Amg::Vector3D& TrackletSegment::globalPosition() const { return m_globalPosition; }
const std::vector<const Muon::MdtPrepData*>& TrackletSegment::mdtHitsOnTrack() const { return m_mdts; }
int TrackletSegment::getHitPattern() const { return m_pattern; }
