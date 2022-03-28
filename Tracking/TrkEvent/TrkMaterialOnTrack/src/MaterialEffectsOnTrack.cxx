/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <ostream>
#include <string>

// Overload of << operator for MsgStream for debug output
MsgStream&
Trk::MaterialEffectsOnTrack::dump(MsgStream& sl) const
{
  sl << "MaterialEffectsOnTrack based on" << endmsg;
  Trk::MaterialEffectsBase::dump(sl);
  sl << "MaterialEffects, Scatterer? : " << (scatteringAngles() ? "yes:" : "none")
     << endmsg;
  if (scatteringAngles()) {
    sl << *scatteringAngles() << endmsg;
  }
  sl << "MaterialEffects, E-loss ?   : " << (energyLoss() ? "yes:" : "none")
     << endmsg;
  if (energyLoss()) {
    sl << *energyLoss() << endmsg;
  }
  return sl;
}
// Overload of << operator for std::ostream for debug output
std::ostream&
Trk::MaterialEffectsOnTrack::dump(std::ostream& sl) const
{
  sl << "MaterialEffectsOnTrack based on" << std::endl;
  Trk::MaterialEffectsBase::dump(sl);
  sl << "MaterialEffects, Scatterer? : " << (scatteringAngles() ? "yes:" : "none")
     << std::endl;
  if (scatteringAngles()) {
    sl << *scatteringAngles() << std::endl;
  }
  sl << "MaterialEffects, E-loss ?   : " << (energyLoss() ? "yes:" : "none")
     << std::endl;
  if (energyLoss()) {
    sl << *energyLoss() << std::endl;
  }
  return sl;
}

