/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkTrack/AlignmentEffectsOnTrack.h"
#include "TrkSurfaces/Surface.h"
#include "GaudiKernel/MsgStream.h"

Trk::AlignmentEffectsOnTrack::AlignmentEffectsOnTrack(float deltaTranslation, 
                                                      float sigmaDeltaTranslation, 
                                                      float deltaAngle,  
                                                      float sigmaDeltaAngle, 
                                                      const std::vector< Identifier>& affectedTSOS,
                                                      const Trk::Surface* surface) :
  m_deltaTranslation(deltaTranslation), 
  m_sigmaDeltaTranslation(sigmaDeltaTranslation), 
  m_deltaAngle(deltaAngle), 
  m_sigmaDeltaAngle(sigmaDeltaAngle), 
  m_affectedTSOS(affectedTSOS),
  m_surface(surface)
{}
  
Trk::AlignmentEffectsOnTrack::AlignmentEffectsOnTrack(const Trk::AlignmentEffectsOnTrack& rhs):
  m_deltaTranslation(rhs.m_deltaTranslation), 
  m_sigmaDeltaTranslation(rhs.m_sigmaDeltaTranslation), 
  m_deltaAngle(rhs.m_deltaAngle), 
  m_sigmaDeltaAngle(rhs.m_sigmaDeltaAngle),
  m_affectedTSOS(rhs.m_affectedTSOS)
{
  if (this!=&rhs){
    m_surface = ( rhs.m_surface->isFree() ? rhs.m_surface->clone() : rhs.m_surface );
  } else {
    m_surface = rhs.m_surface; // This is primarily an attempt to reassure coverity.
  }
}

Trk::AlignmentEffectsOnTrack& Trk::AlignmentEffectsOnTrack::operator=(const Trk::AlignmentEffectsOnTrack& rhs)
{
  if (this!=&rhs){
    m_deltaTranslation = rhs.m_deltaTranslation; 
    m_sigmaDeltaTranslation = rhs.m_sigmaDeltaTranslation; 
    m_deltaAngle = rhs.m_deltaAngle; 
    m_sigmaDeltaAngle = rhs.m_sigmaDeltaAngle;
    if ( m_surface->isFree() ) { delete m_surface;}
    m_surface = ( rhs.m_surface->isFree() ? rhs.m_surface->clone() : rhs.m_surface );
    m_affectedTSOS = rhs.m_affectedTSOS ;
  }
  return *this;
}  

Trk::AlignmentEffectsOnTrack::~AlignmentEffectsOnTrack(){
  if ( m_surface && m_surface->isFree() ) { 
    delete m_surface; m_surface = nullptr;
  }
}

/**Overload of << operator for both, MsgStream and std::ostream for debug output*/ 
MsgStream& Trk::operator << ( MsgStream& sl, const Trk::AlignmentEffectsOnTrack& aeot)
{  
  if (sl.level()<MSG::INFO) 
  {
      sl<<"AlignmentEffectsOnTrack:"<<endmsg;
      sl <<"deltaTranslation = "<<aeot.deltaTranslation()<<endmsg;
      sl <<"sigmaDeltaTranslation = "<<aeot.deltaTranslation()<<endmsg;
      sl <<"deltaAngle = "<<aeot.deltaAngle()<<endmsg;
      sl <<"sigmaDeltaAngle = "<<aeot.sigmaDeltaAngle()<<endmsg;
      sl <<"surface = "<<aeot.associatedSurface()<<endmsg;
  }
  return sl;  
}

std::ostream& Trk::operator << ( std::ostream& sl, const Trk::AlignmentEffectsOnTrack& aeot)
{
  sl<<"AlignmentEffectsOnTrack:"<<std::endl;
  sl <<"deltaTranslation = "<<aeot.deltaTranslation()<<std::endl;
  sl <<"sigmaDeltaTranslation = "<<aeot.deltaTranslation()<<std::endl;
  sl <<"deltaAngle = "<<aeot.deltaAngle()<<std::endl;
  sl <<"sigmaDeltaAngle = "<<aeot.sigmaDeltaAngle()<<std::endl;
  sl <<"surface = "<<aeot.associatedSurface()<<std::endl;
  return sl;
}
