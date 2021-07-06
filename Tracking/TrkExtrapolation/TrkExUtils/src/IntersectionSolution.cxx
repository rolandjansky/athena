/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IntersectionSolution.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkExUtils/IntersectionSolution.h"
#include "TrkExUtils/TrackSurfaceIntersection.h"
// Gaudi
#include "GaudiKernel/MsgStream.h"
// STD
#include <ostream>
#include <sstream>

// Overload of << operator for both, MsgStream and std::ostream for debug output
MsgStream&
Trk::operator<<(MsgStream& sl, const Trk::IntersectionSolution& isol)
{
  std::ostringstream out;
  out << isol;
  sl << out.str();
  return sl;
}

std::ostream&
Trk::operator<<(std::ostream& sl, const Trk::IntersectionSolution& isol)
{
  sl << "Trk::IntersectionSolution  \n";
  int isl = 0;
  for (const auto & entry:isol) {
    isl++;
    if (entry)
      sl << "  " << isl << ".entry : " << *entry << "\n";
    else
      sl << "  " << isl << ".entry : not filled " << "\n";
  }
  return sl;
}

