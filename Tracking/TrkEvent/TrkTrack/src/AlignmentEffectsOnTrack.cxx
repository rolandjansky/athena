/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkTrack/AlignmentEffectsOnTrack.h"
#include "GaudiKernel/MsgStream.h"
#include "TrkSurfaces/Surface.h"

Trk::AlignmentEffectsOnTrack::AlignmentEffectsOnTrack(
  float deltaTranslation,
  float sigmaDeltaTranslation,
  float deltaAngle,
  float sigmaDeltaAngle,
  const std::vector<Identifier>& affectedTSOS,
  const Trk::Surface* surface)
  : SurfacePtrHolder(surface)
  , m_deltaTranslation(deltaTranslation)
  , m_sigmaDeltaTranslation(sigmaDeltaTranslation)
  , m_deltaAngle(deltaAngle)
  , m_sigmaDeltaAngle(sigmaDeltaAngle)
  , m_affectedTSOS(affectedTSOS)
{}

/**Overload of << operator for both, MsgStream and std::ostream for debug
 * output*/
MsgStream&
Trk::operator<<(MsgStream& sl, const Trk::AlignmentEffectsOnTrack& aeot)
{
  if (sl.level() < MSG::INFO) {
    sl << "AlignmentEffectsOnTrack:" << endmsg;
    sl << "deltaTranslation = " << aeot.deltaTranslation() << endmsg;
    sl << "sigmaDeltaTranslation = " << aeot.deltaTranslation() << endmsg;
    sl << "deltaAngle = " << aeot.deltaAngle() << endmsg;
    sl << "sigmaDeltaAngle = " << aeot.sigmaDeltaAngle() << endmsg;
    sl << "surface = " << aeot.associatedSurface() << endmsg;
  }
  return sl;
}

std::ostream&
Trk::operator<<(std::ostream& sl, const Trk::AlignmentEffectsOnTrack& aeot)
{
  sl << "AlignmentEffectsOnTrack:" << std::endl;
  sl << "deltaTranslation = " << aeot.deltaTranslation() << std::endl;
  sl << "sigmaDeltaTranslation = " << aeot.deltaTranslation() << std::endl;
  sl << "deltaAngle = " << aeot.deltaAngle() << std::endl;
  sl << "sigmaDeltaAngle = " << aeot.sigmaDeltaAngle() << std::endl;
  sl << "surface = " << aeot.associatedSurface() << std::endl;
  return sl;
}
