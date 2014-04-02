/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IInDetAlignTrackSelTool.h

// Sergio Gonzalez Sevilla, started 04/7/05
// Miguel Olivo Gomez, extended 07/6/06

// AlgTool to select high quality tracks for the inner detector 
// (Pixel+SCT) alignment algorithms.
// This AlgTool provides a track selection based on the following cut variables:
// Momentum, pt, number of shared hits, number of holes and chi2 probability. 
// Returns 0 in case a track is not accepted, otherwise 1

#ifndef INDETALIGNTOOLS_ALIGNTRACKSELTOOL_IH
#define INDETALIGNTOOLS_ALIGNTRACKSELTOOL_IH

#include "GaudiKernel/IAlgTool.h"
#include "TrkTrack/Track.h"

static const InterfaceID 
  IID_INDETALIGN_IInDetAlignTrackSelTool("IInDetAlignTrackSelTool",1,0);

class IInDetAlignTrackSelTool: virtual public IAlgTool {
 public:
  static const InterfaceID& interfaceID();

  virtual int getStatus(const Trk::Track&) const = 0;
};

inline const InterfaceID& IInDetAlignTrackSelTool::interfaceID()
{ return   IID_INDETALIGN_IInDetAlignTrackSelTool; }

#endif // INDETALIGNTOOLS_ALIGNTRACKSELTOOL_IH
