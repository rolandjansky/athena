/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGERMENUANALYSIS_MUONOBJECTSINROI_H
#define TRIGGERMENUANALYSIS_MUONOBJECTSINROI_H
/*
  MuonObjectsInRoI.h
*/
#include "TriggerMenuAnalysis/HLTObjectsInRoI.h"

class MuonFeature;
class MuonFeatureDetails;
class TrigInDetTrack;
class CombinedMuonFeature;
class TrigInDetTrackCollection;
class TrigVertexCollection;
class IsoMuonFeature;

class TrigMuonEFContainer;
class TrigMuonEFInfoContainer;


namespace HLT {
  class TriggerElement;
  class Navigation;
}

class MuonObjectsInRoI : public HLTObjectsInRoI {
public:
  static HLTObjectsInRoI* create();

public:
  MuonObjectsInRoI();
  ~MuonObjectsInRoI();

  const Muon_ROI* getMuonRoI() const { return m_muonRoI; }
  const MuonFeature* getMuonFeature() const { return m_muonFeature; }
  const MuonFeatureDetails* getMuonFeatureDetails() const { return m_muonFeatureDetails; }
  const TrigVertexCollection* getVertexCollection() const {
    return m_vertexCollection;
  }
  const TrigInDetTrackCollection* getInDetTrackCollection() const {
    return m_inDetTrackCollection;
  }
  const CombinedMuonFeature* getCombinedMuonFeature() const {
    return m_combinedMuon;
  }
  const IsoMuonFeature* getIsoMuonFeature() const { return m_isoMuonFeature; }
  const TrigMuonEFContainer* getEFMuon() const { return m_EFMuon; }
  const TrigMuonEFInfoContainer* getEFMuonInfo() const { return m_EFMuonInfo; }

  inline void setMuonFeature(const MuonFeature* x);
  inline void setCombinedMuonFeature(const CombinedMuonFeature* x);
  inline void setEFMuon(const TrigMuonEFContainer* x);

protected:
  const Muon_ROI* m_muonRoI;
  const MuonFeature* m_muonFeature;
  const MuonFeatureDetails* m_muonFeatureDetails;
  const TrigVertexCollection* m_vertexCollection;
  const TrigInDetTrackCollection* m_inDetTrackCollection;
  const CombinedMuonFeature* m_combinedMuon;
  const IsoMuonFeature* m_isoMuonFeature;

  const TrigMuonEFContainer* m_EFMuon;
  const TrigMuonEFInfoContainer* m_EFMuonInfo;
};


void MuonObjectsInRoI::setMuonFeature(const MuonFeature* x) {
  m_muonFeature = x;
}

void MuonObjectsInRoI::setCombinedMuonFeature(const CombinedMuonFeature* x) {
  m_combinedMuon = x;
}

void MuonObjectsInRoI::setEFMuon(const TrigMuonEFContainer* x) {
  m_EFMuon = x;
}

#endif // TRIGGERMENUANALYSIS_MUONOBJECTSINROI_H
