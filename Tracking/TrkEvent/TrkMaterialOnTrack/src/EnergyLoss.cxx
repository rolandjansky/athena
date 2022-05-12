/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EnergyLoss.cxx (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "GaudiKernel/MsgStream.h"
#include <ostream>
#include <string>

//! Overload of << operator for MsgStream for debug output
MsgStream&
Trk::operator<<(MsgStream& sl, const EnergyLoss& eloss)
{
  return eloss.dump(sl);
}

//! Overload of << operator for std::ostream for debug outputstd::ostream&
std::ostream&
Trk::operator<<(std::ostream& sl, const EnergyLoss& eloss)
{
  return eloss.dump(sl);
}

// Overload of << operator for MsgStream and std::ostream for debug output
MsgStream&
Trk::EnergyLoss::dump(MsgStream& sl) const
{
  sl << "EnergyLoss :   ( delta(E), sigma(dE) ) = \t"
     << "(" << deltaE() << ", \t" << sigmaDeltaE() << ")";
  return sl;
}

std::ostream&
Trk::EnergyLoss::dump(std::ostream& sl) const
{
  sl << "EnergyLoss :   ( delta(E), sigma(dE) ) = \t"
     << "(" << deltaE() << ", \t" << sigmaDeltaE() << ")";
  return sl;
}

