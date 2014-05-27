/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetPattRecoEvent/TrigFTK_TrackSeed.h"


TrigFTK_TrackSeed::TrigFTK_TrackSeed() :
  m_perigee(0)
{
  
}


TrigFTK_TrackSeed::TrigFTK_TrackSeed(const Trk::Perigee* mp) :
  m_perigee(0)
{
  
  if (!mp) return;
  m_perigee = new Trk::Perigee(*mp);
  
}


TrigFTK_TrackSeed::TrigFTK_TrackSeed(const std::list<Amg::Vector3D>& gpList) :
  m_perigee(0), m_gpList(gpList)
{
  
}


TrigFTK_TrackSeed::TrigFTK_TrackSeed(const Trk::Perigee* mp, const std::list<Amg::Vector3D>& gpList) :
  m_perigee(0), m_gpList(gpList)
{

  if (!mp) return;
  m_perigee = new Trk::Perigee(*mp);
  
}


TrigFTK_TrackSeed::TrigFTK_TrackSeed(const TrigFTK_TrackSeed& trackseed) :
  m_perigee(0), m_gpList(trackseed.m_gpList)
{

  if (!trackseed.m_perigee) return;
  m_perigee = new Trk::Perigee(*trackseed.m_perigee);
  
}


TrigFTK_TrackSeed::~TrigFTK_TrackSeed() 
{
  
  DeleteMeasuredPerigee();
  
}


void TrigFTK_TrackSeed::InitMeasuredPerigee(const Trk::Perigee* mp) 
{
  
  if (!mp) return;
  
  if (m_perigee) DeleteMeasuredPerigee();
  m_perigee = new Trk::Perigee(*mp);

}


void TrigFTK_TrackSeed::AddGlobalPosition(const Amg::Vector3D& gp) 
{
  
  m_gpList.push_back(gp);
  
}

void TrigFTK_TrackSeed::DeleteMeasuredPerigee() 
{

  delete m_perigee;
  m_perigee = 0;

}
