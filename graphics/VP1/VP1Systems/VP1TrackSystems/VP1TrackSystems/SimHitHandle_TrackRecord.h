/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class SimHitHandle_TrackRecord            //
//                                                            //
//  Description: Handle for Track Records                     //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: May 2008                                 //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef SIMHITHANDLE_TRACKRECORD_H
#define SIMHITHANDLE_TRACKRECORD_H

#include "VP1TrackSystems/SimHitHandleBase.h"
#include "TrackRecord/TrackRecord.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"


class TrackRecord;
class HepMcParticleLink;

class SimHitHandle_TrackRecord : public SimHitHandleBase {
public:

  SimHitHandle_TrackRecord(const TrackRecord*);
  virtual ~SimHitHandle_TrackRecord();

  QString type() const { return "TrackRecord"; };

  Amg::Vector3D momentumDirection() const;
  double actualMomentum() const;
  Amg::Vector3D posStart() const;
  Amg::Vector3D posEnd() const;//We fake this one as a point 0.1mm away from posStart, in the momentumDirection.
  double hitTime() const;
  int actualPDGCodeFromSimHit() const;
  const HepMcParticleLink& particleLink() const;
  Trk::TrackParameters * createTrackParameters() const;

private:

  TrackRecord* m_trkrecord;//We keep this as non-const pointers due to wrong constness in TrackRecord methods!!
  double m_mom;
  Amg::Vector3D m_momdir;
  const HepMcParticleLink* m_link;
};

///////////////
//  Inlines  //
///////////////

inline Amg::Vector3D SimHitHandle_TrackRecord::momentumDirection() const
{
  return m_momdir;
}

inline double SimHitHandle_TrackRecord::actualMomentum() const
{
  return m_mom;
}

inline Amg::Vector3D SimHitHandle_TrackRecord::posStart() const
{
  return Amg::Hep3VectorToEigen(m_trkrecord->GetPosition());
}

inline Amg::Vector3D SimHitHandle_TrackRecord::posEnd() const
{
  return posStart() + 0.1*CLHEP::mm*(momentumDirection());//faking it... but shouldn't matter.
}

inline double SimHitHandle_TrackRecord::hitTime() const
{
  return m_trkrecord->GetTime();
}

inline int SimHitHandle_TrackRecord::actualPDGCodeFromSimHit() const
{
  return m_trkrecord->GetPDGCode();
}

inline const HepMcParticleLink& SimHitHandle_TrackRecord::particleLink() const
{
  return *m_link;
}

#endif
