/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EstimatedBremOnTrack.cxx (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkMaterialOnTrack/EstimatedBremOnTrack.h"
#include "GaudiKernel/MsgStream.h"
#include "TrkSurfaces/Surface.h"
#include <ostream>
#include <string>

Trk::EstimatedBremOnTrack::EstimatedBremOnTrack(double tInX0,
                                                double retainedEnFraction,
                                                double sEnFr,
                                                double sQoPs,
                                                const Surface& sf,
                                                SearchDirection searchDirection)
  : Trk::MaterialEffectsBase(
      tInX0,
      sf,
      1 << static_cast<int>(Trk::MaterialEffectsBase::BremPoint))
  , m_retainedEnFraction(retainedEnFraction)
  , m_sigmaRetainedEnFraction(sEnFr)
  , m_sigmaQoverPsquared(sQoPs)
  , m_bremSearchDirection(searchDirection)
{}


// Overload of << operator for MsgStream for debug output
MsgStream&
Trk::EstimatedBremOnTrack::dump(MsgStream& sl) const
{
  sl << "EstimatedBremOnTrack based on" << endmsg;
  Trk::MaterialEffectsBase::dump(sl);
  sl << "EstBremOT retained p fraction z: " << retainedEnFraction() << endmsg;
  sl << "EstBremOT sigma(z)             : " << sigmaRetainedEnFraction()
     << endmsg;
  sl << "EstBremOT q/p noise term to cov: " << sigmaQoverPsquared() << endmsg;
  sl << "EstBremOT brem search direction: " << searchDirection() << endmsg;
  return sl;
}
// Overload of << operator for std::ostream for debug output
std::ostream&
Trk::EstimatedBremOnTrack::dump(std::ostream& sl) const
{
  sl << "EstimatedBremOnTrack based on" << std::endl;
  Trk::MaterialEffectsBase::dump(sl);
  sl << "\t EstBremOT retained p fraction z: " << retainedEnFraction()
     << std::endl;
  sl << "\t EstBremOT sigma(z)             : " << sigmaRetainedEnFraction()
     << std::endl;
  sl << "\t EstBremOT q/p noise term to cov: " << sigmaQoverPsquared()
     << std::endl;
  sl << "\t EstBremOT brem search direction: " << searchDirection()
     << std::endl;
  return sl;
}

