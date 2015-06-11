/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __ITRIGHITFILTER_H__ 
#define __ITRIGHITFILTER_H__ 

#include <vector>
#include "GaudiKernel/IAlgTool.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"

class TrigSiSpacePoint;
class IRoiDescriptor;

static const InterfaceID IID_ITrigHitFilter("ITrigHitFilter", 1 , 0); 

class ITrigHitFilter: virtual public IAlgTool 
{
 public:

  static const InterfaceID& interfaceID() {
    return IID_ITrigHitFilter;
  }
  virtual void findTracks( std::vector<const TrigSiSpacePoint* >, 
			   TrigInDetTrackCollection&,
		  	   const IRoiDescriptor*, 
			   double = 0.0, 
			   double = 0.0, double = 0.0,
			   const int = 0 ) = 0;
};

#endif
