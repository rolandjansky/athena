/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IntersectionSolution.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkExUtils/IntersectionSolution.h"
// Gaudi
#include "GaudiKernel/MsgStream.h"
// STD
#include <iomanip>
#include <iostream>

// default constructor
Trk::IntersectionSolution::IntersectionSolution() = default;

// constructor
Trk::IntersectionSolution::IntersectionSolution(int dim)
  : std::vector<const Trk::TrackSurfaceIntersection*>(dim)
{}

// destructor
Trk::IntersectionSolution::~IntersectionSolution()
{
  this->freeMemory();
}

// free memory method for destructor
void
Trk::IntersectionSolution::freeMemory()
{
  Trk::IntersectionSolutionIter free_iter = this->begin();
  for (; free_iter != this->end(); free_iter++) {
    delete (*free_iter);
  }
}

// Overload of << operator for both, MsgStream and std::ostream for debug output
MsgStream&
Trk::operator<<(MsgStream& sl, const Trk::IntersectionSolution& isol)
{
  sl << "Trk::IntersectionSolution  " << std::endl;
  Trk::IntersectionSolutionIter output_iter = isol.begin();
  int isl = 0;
  for (; output_iter != isol.end(); output_iter++) {
    isl++;
    if (*output_iter)
      sl << "  " << isl << ".entry : " << **output_iter << std::endl;
    else
      sl << "  " << isl << ".entry : not filled " << std::endl;
  }
  return sl;
}

std::ostream&
Trk::operator<<(std::ostream& sl, const Trk::IntersectionSolution& isol)
{
  sl << "Trk::IntersectionSolution  " << std::endl;
  Trk::IntersectionSolutionIter output_iter = isol.begin();
  int isl = 0;
  for (; output_iter != isol.end(); output_iter++) {
    isl++;
    if (*output_iter)
      sl << "  " << isl << ".entry : " << **output_iter << std::endl;
    else
      sl << "  " << isl << ".entry : not filled " << std::endl;
  }
  return sl;
}

