/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __ITRIGINDETTRACKEXTRAPOLATOR_H__ 
#define __ITRIGINDETTRACKEXTRAPOLATOR_H__ 

#include <vector>
#include "GaudiKernel/IAlgTool.h"
#include "TrigInDetEvent/TrigInDetTrackFitPar.h"
#include "TrigInDetEvent/TrigInDetTrack.h"

static const InterfaceID IID_ITrigInDetTrackExtrapolator("ITrigTrackExtrapolator", 1 , 0); 

class ITrigInDetTrackExtrapolator: virtual public IAlgTool 
{
 public:

  static const InterfaceID& interfaceID() {
    return IID_ITrigInDetTrackExtrapolator;
  }

  //  virtual TrigInDetTrackFitPar* extrapolate(const TrigInDetTrack*, 
  //					    const IRoiDescriptor*) = 0;

  virtual TrigInDetTrackFitPar* extrapolateToBarrel(const TrigInDetTrack*, 
						    double) = 0;
  virtual TrigInDetTrackFitPar* extrapolateToEndcap(const TrigInDetTrack*, 
						    double) = 0;
  virtual StatusCode extrapolateToCalo(const TrigInDetTrack*,double,double,double&,double&) = 0;
};

#endif
