/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGERMENUANALYSIS_TILEMUOBJECTSINROI_H
#define TRIGGERMENUANALYSIS_TILEMUOBJECTSINROI_H
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

  const Muon_ROI* getMuonRoI() const { return m_muonRoI; }
  const TileMuFeature* getTileMuFeature() const { return m_tileMuFeature; }
  const TileTrackMuFeature* getTileTrackMuFeature() const { return m_tileTrackMuFeature; }
  const TrigVertexCollection* getVertexCollection() const {
    return m_vertexCollection;
  }
  const TrigInDetTrackCollection* getInDetTrackCollection() const {
    return m_inDetTrackCollection;
  }
  const TrigMuonEFContainer* getEFMuon() const { return m_EFMuon; }
  const TrigMuonEFInfoContainer* getEFMuonInfo() const { return m_EFMuonInfo; }

  inline void setTileMuFeature(const TileMuFeature* x);
  inline void setTileTrackMuFeature(const TileTrackMuFeature* x);
  inline void setEFMuon(const TrigMuonEFContainer* x);
  inline void setEFMuonInfo(const TrigMuonEFInfoContainer* x);

protected:
  const Muon_ROI* m_muonRoI;
  const TileMuFeature* m_tileMuFeature;
  const TileTrackMuFeature* m_tileTrackMuFeature;
  const TrigVertexCollection* m_vertexCollection;
  const TrigInDetTrackCollection* m_inDetTrackCollection;

  const TrigMuonEFContainer* m_EFMuon;
  const TrigMuonEFInfoContainer* m_EFMuonInfo;
};


void TileMuObjectsInRoI::setTileMuFeature(const TileMuFeature* x) {
  m_tileMuFeature = x;
}

void TileMuObjectsInRoI::setTileTrackMuFeature(const TileTrackMuFeature* x) {
  m_tileTrackMuFeature = x;
}

void TileMuObjectsInRoI::setEFMuon(const TrigMuonEFContainer* x) {
  m_EFMuon = x;
}

void TileMuObjectsInRoI::setEFMuonInfo(const TrigMuonEFInfoContainer* x) {
  m_EFMuonInfo = x;
}

#endif // TRIGGERMENUANALYSIS_TILEMUOBJECTSINROI_H

