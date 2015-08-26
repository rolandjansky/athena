/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __MuonObjectsInRoI_h__
#define __MuonObjectsInRoI_h__
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

  const Muon_ROI* getMuonRoI() const { return mMuonRoI; }
  const MuonFeature* getMuonFeature() const { return mMuonFeature; }
  const MuonFeatureDetails* getMuonFeatureDetails() const { return mMuonFeatureDetails; }
  const TrigVertexCollection* getVertexCollection() const {
    return mVertexCollection;
  }
  const TrigInDetTrackCollection* getInDetTrackCollection() const {
    return mInDetTrackCollection;
  }
  const CombinedMuonFeature* getCombinedMuonFeature() const {
    return mCombinedMuon;
  }
  const IsoMuonFeature* getIsoMuonFeature() const { return mIsoMuonFeature; }
  const TrigMuonEFContainer* getEFMuon() const { return mEFMuon; }
  const TrigMuonEFInfoContainer* getEFMuonInfo() const { return mEFMuonInfo; }

  inline void setMuonFeature(const MuonFeature* x);
  inline void setCombinedMuonFeature(const CombinedMuonFeature* x);
  inline void setEFMuon(const TrigMuonEFContainer* x);

protected:
  const Muon_ROI* mMuonRoI;
  const MuonFeature* mMuonFeature;
  const MuonFeatureDetails* mMuonFeatureDetails;
  const TrigVertexCollection* mVertexCollection;
  const TrigInDetTrackCollection* mInDetTrackCollection;
  const CombinedMuonFeature* mCombinedMuon;
  const IsoMuonFeature* mIsoMuonFeature;

  const TrigMuonEFContainer* mEFMuon;
  const TrigMuonEFInfoContainer* mEFMuonInfo;
};


void MuonObjectsInRoI::setMuonFeature(const MuonFeature* x) {
  mMuonFeature = x;
}

void MuonObjectsInRoI::setCombinedMuonFeature(const CombinedMuonFeature* x) {
  mCombinedMuon = x;
}

void MuonObjectsInRoI::setEFMuon(const TrigMuonEFContainer* x) {
  mEFMuon = x;
}

#endif // __MuonObjectsInRoI_h__
