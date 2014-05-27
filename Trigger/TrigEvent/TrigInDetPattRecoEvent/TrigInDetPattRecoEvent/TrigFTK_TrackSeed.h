/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIGFTKTRACKSEED_H__
#define __TRIGFTKTRACKSEED_H__


#include <list>
#include "TrkParameters/TrackParameters.h"
#include "GeoPrimitives/GeoPrimitives.h"


class TrigFTK_TrackSeed {
  
 public: 

  TrigFTK_TrackSeed();
  TrigFTK_TrackSeed(const Trk::Perigee*);
  TrigFTK_TrackSeed(const std::list<Amg::Vector3D>&);
  TrigFTK_TrackSeed(const Trk::Perigee*, const std::list<Amg::Vector3D>&);
  TrigFTK_TrackSeed(const TrigFTK_TrackSeed&);
  ~TrigFTK_TrackSeed();
  
  void InitMeasuredPerigee(const Trk::Perigee*);
  void AddGlobalPosition(const Amg::Vector3D&);

  const Trk::Perigee* GetMeasuredPerigee() { return m_perigee; }
  const std::list<Amg::Vector3D>& GetListOfGlobalPosition() { return m_gpList; }

 private:

  Trk::Perigee* m_perigee;
  std::list<Amg::Vector3D> m_gpList;

  void DeleteMeasuredPerigee();
  
};

#endif

