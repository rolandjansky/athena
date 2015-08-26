/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TileMuObjectsInRoI_h__
#define __TileMuObjectsInRoI_h__
/*
  TileMuObjectsInRoI.h
*/
#include "TriggerMenuAnalysis/HLTObjectsInRoI.h"
#include "TileEvent/TileL2Container.h"

class TileMuFeature;
class TileTrackMuFeature;
class TrigInDetTrack;
class TrigInDetTrackCollection;
class TrigVertexCollection;

class TrigMuonEFContainer;
class TrigMuonEFInfoContainer;

class Muon_ROI;

namespace HLT {
  class TriggerElement;
  class Navigation;
}

class TileMuObjectsInRoI : public HLTObjectsInRoI {
public:
  static HLTObjectsInRoI* create();

public:
  TileMuObjectsInRoI();
  ~TileMuObjectsInRoI();

  const Muon_ROI* getMuonRoI() const { return mMuonRoI; }
  const TileMuFeature* getTileMuFeature() const { return mTileMuFeature; }
  const TileTrackMuFeature* getTileTrackMuFeature() const { return mTileTrackMuFeature; }
  const TrigVertexCollection* getVertexCollection() const {
    return mVertexCollection;
  }
  const TrigInDetTrackCollection* getInDetTrackCollection() const {
    return mInDetTrackCollection;
  }
  const TrigMuonEFContainer* getEFMuon() const { return mEFMuon; }
  const TrigMuonEFInfoContainer* getEFMuonInfo() const { return mEFMuonInfo; }

  inline void setTileMuFeature(const TileMuFeature* x);
  inline void setTileTrackMuFeature(const TileTrackMuFeature* x);
  inline void setEFMuon(const TrigMuonEFContainer* x);
  inline void setEFMuonInfo(const TrigMuonEFInfoContainer* x);

protected:
  const Muon_ROI* mMuonRoI;
  const TileMuFeature* mTileMuFeature;
  const TileTrackMuFeature* mTileTrackMuFeature;
  const TrigVertexCollection* mVertexCollection;
  const TrigInDetTrackCollection* mInDetTrackCollection;

  const TrigMuonEFContainer* mEFMuon;
  const TrigMuonEFInfoContainer* mEFMuonInfo;
};


void TileMuObjectsInRoI::setTileMuFeature(const TileMuFeature* x) {
  mTileMuFeature = x;
}

void TileMuObjectsInRoI::setTileTrackMuFeature(const TileTrackMuFeature* x) {
  mTileTrackMuFeature = x;
}

void TileMuObjectsInRoI::setEFMuon(const TrigMuonEFContainer* x) {
  mEFMuon = x;
}

void TileMuObjectsInRoI::setEFMuonInfo(const TrigMuonEFInfoContainer* x) {
  mEFMuonInfo = x;
}

#endif // __TileMuObjectsInRoI_h__
