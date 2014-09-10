// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrackD3PDMaker/PerigeePair.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2011
 * @brief Holder for a pair of perigee objects, before and after refitting.
 */


#include "TrkParameters/TrackParameters.h"

#ifndef TRACKD3PDMAKER_PERIGEEPAIR_H
#define TRACKD3PDMAKER_PERIGEEPAIR_H


namespace D3PD {


/*
 * @brief Holder for a pair of perigee objects, before and after refitting.
 */
struct PerigeePair
{
  const Trk::Perigee* newtrack;
  const Trk::Perigee* oldtrack;
  PerigeePair (const Trk::Perigee* the_newtrack,
               const Trk::Perigee* the_oldtrack)
    : newtrack (the_newtrack), oldtrack (the_oldtrack) {}
};


} // namespace D3PD


#endif // not TRACKD3PDMAKER_PERIGEEPAIR_H
