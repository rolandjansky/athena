/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class TrackHandle_TrkSegment              //
//                                                            //
//  Description: TrackHandle for Trk::Segment's               //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: February 2008                            //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef TRACKHANDLE_TRKSEGMENT_H
#define TRACKHANDLE_TRKSEGMENT_H

#include "VP1TrackSystems/TrackHandleBase.h"

#include "GeoPrimitives/GeoPrimitives.h"

namespace Trk {
  class Segment;
}

class TrackHandle_TrkSegment : public TrackHandleBase {
public:

  TrackHandle_TrkSegment(TrackCollHandleBase*,const Trk::Segment*);
  virtual ~TrackHandle_TrkSegment();

  QStringList clicked() const;
  virtual bool hasSubSystemHitInfo() const { ensureInitSubSysHitInfo(); return m_hitInfoStatus; }
  virtual unsigned getNPixelHits() const { ensureInitSubSysHitInfo(); return m_nhits_pixel; }
  virtual unsigned getNSCTHits() const { ensureInitSubSysHitInfo(); return m_nhits_sct; }
  virtual unsigned getNTRTHits() const { ensureInitSubSysHitInfo(); return m_nhits_trt; }
  virtual unsigned getNMuonPhiHits() const {  ensureInitSubSysHitInfo(); return m_nhits_muon_phi; } 
  virtual unsigned getNRPCHits() const { ensureInitSubSysHitInfo(); return m_nhits_rpc; }
  virtual unsigned getNMDTHits() const { ensureInitSubSysHitInfo(); return m_nhits_mdt; }
  virtual unsigned getNTGCHits() const { ensureInitSubSysHitInfo(); return m_nhits_tgc; }
  virtual unsigned getNCSCHits() const { ensureInitSubSysHitInfo(); return m_nhits_csc; }
  void visibleStateChanged();
  const std::vector< Amg::Vector3D > * provide_pathInfoPoints() const;

  void ensureTouchedMuonChambersInitialised() const;
  const Trk::Segment* trkSegmentPointer() const { return m_segment; }

  virtual QString type() const { return QString("Segment"); } //!< return very short word with type (maybe link with collection type?)

private:
  // void visibleStateChanged();
  void currentMaterialChanged();
  mutable unsigned m_nhits_pixel;
  mutable unsigned m_nhits_sct;
  mutable unsigned m_nhits_trt;
  mutable unsigned m_nhits_muon_phi;
  mutable unsigned m_nhits_rpc;
  mutable unsigned m_nhits_mdt;
  mutable unsigned m_nhits_tgc;
  mutable unsigned m_nhits_csc;
  mutable int m_hitInfoStatus;//!< -1: uninitialized, 0: not present, 1: present.
  void ensureInitSubSysHitInfo() const;
  const Trk::Segment* m_segment;
  mutable std::vector< Amg::Vector3D > * m_points;
};

#endif
