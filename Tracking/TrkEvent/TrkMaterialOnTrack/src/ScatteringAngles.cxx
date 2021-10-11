/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkMaterialOnTrack/ScatteringAngles.h"
#include "GaudiKernel/MsgStream.h"
#include <string>


// Overload of << operator for MsgStream for debug output
MsgStream&
Trk::operator<<(MsgStream& sl, const Trk::ScatteringAngles& saos)
{
  std::string name("ScatteringAngles: ");
  sl << name << "deltaPhi        : " << saos.deltaPhi() << endmsg;
  sl << name << "deltaTheta      : " << saos.deltaTheta() << endmsg;
  sl << name << "sigmaDeltaPhi   : " << saos.sigmaDeltaPhi() << endmsg;
  sl << name << "sigmaDeltaTheta : " << saos.sigmaDeltaTheta() << endmsg;
  return sl;
}
// Overload of << operator for std::ostream for debug output
std::ostream&
Trk::operator<<(std::ostream& sl, const Trk::ScatteringAngles& saos)
{
  sl << "ScatteringAngles: " << std::endl;
  sl << "\t deltaPhi        : " << saos.deltaPhi() << std::endl;
  sl << "\t deltaTheta      : " << saos.deltaTheta() << std::endl;
  sl << "\t sigmaDeltaPhi   : " << saos.sigmaDeltaPhi() << std::endl;
  sl << "\t sigmaDeltaTheta : " << saos.sigmaDeltaTheta() << std::endl;
  return sl;
}

