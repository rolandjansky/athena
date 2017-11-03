/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class TrackHandle_TrackParticle           //
//                                                            //
//  Description: Handles for track particles.                 //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: May 2008                                 //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef TRACKHANDLE_XAODTRACKPARTICLE_H
#define TRACKHANDLE_XAODTRACKPARTICLE_H

#include "VP1TrackSystems/TrackHandleBase.h"

#include "xAODTracking/TrackParticle.h"
#include "GeoPrimitives/GeoPrimitives.h"


// namespace xAOD { class TrackParticle; }

class TrackHandle_xAODTrackParticle : public TrackHandleBase {
public:

  TrackHandle_xAODTrackParticle(TrackCollHandleBase*, const xAOD::TrackParticle * );
  virtual ~TrackHandle_xAODTrackParticle();

  virtual QStringList clicked() const;
  virtual Amg::Vector3D momentum() const;

  virtual unsigned getNPixelHits() const;
  virtual unsigned getNSCTHits() const;
  virtual unsigned getNTRTHits() const;
  virtual unsigned getNMuonPhiHits() const;
  virtual unsigned getNMDTHits() const;
  virtual unsigned getNRPCHits() const;
  virtual unsigned getNTGCHits() const;
  virtual unsigned getNCSCHits() const;

  virtual QString type() const { return QString("TrackParticle"); } //!< return very short word with type (maybe link with collection type?)


protected:
  const Trk::Track * provide_pathInfoTrkTrack() const;
  void ensureTouchedMuonChambersInitialised() const;
  double calculateCharge() const;
  virtual bool hasSubSystemHitInfo() const { return true; } //TODO - maybe actually check content? But a TP MUST have some hit info.
  

private:

  class Imp;
  Imp * m_d;
  void ensureInitSubSysHitInfo() const;

};

#endif
