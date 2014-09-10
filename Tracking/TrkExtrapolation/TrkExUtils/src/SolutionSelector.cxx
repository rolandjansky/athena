/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SolutionSelector.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////


#include "TrkExUtils/SolutionSelector.h"
#include <math.h>

Trk::SolutionSelector::SolutionSelector()
{}

Trk::SolutionSelector::~SolutionSelector()
{}

Trk::SelectedSolution Trk::SolutionSelector::select(Trk::PropDirection dir, double pl1, double pl2)
{
  Trk::SelectedSolution sol = Trk::none;
  switch (dir) {
    case alongMomentum :
       {
          if (pl1 > 0. && pl2 > 0.) sol = (pl1 < pl2 ) ? Trk::first : Trk::second;
          else if (pl1 > 0.) sol = Trk::first;
          else if (pl2 > 0.) sol = Trk::second;

       } break;
    case oppositeMomentum :
       {
          if (pl1 < 0. && pl2 < 0.) sol = (pl1 > pl2 ) ? Trk::first : Trk::second;
          else if (pl1 < 0.) sol = Trk::first;
          else if (pl2 < 0.) sol = Trk::second;

       } break;
    case anyDirection :
    default           :
       {
          sol = (fabs(pl1) < fabs(pl2)) ? Trk::first : Trk::second;
       }

  }

  return sol;
}


