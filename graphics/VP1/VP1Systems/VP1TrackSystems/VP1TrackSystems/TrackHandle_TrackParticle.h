/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class TrackHandle_xAODTrackParticle           //
//                                                            //
//  Description: Handles for track particles.                 //
//                                                            //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef TRACKHANDLE_TRACKPARTICLE_H
#define TRACKHANDLE_TRACKPARTICLE_H

#include "VP1TrackSystems/TrackHandleBase.h"
#include "GeoPrimitives/GeoPrimitives.h"

namespace Rec { class TrackParticle; }

class TrackHandle_TrackParticle : public TrackHandleBase {
public:

  TrackHandle_TrackParticle(TrackCollHandleBase*, const Rec::TrackParticle * );
  virtual ~TrackHandle_TrackParticle();

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
