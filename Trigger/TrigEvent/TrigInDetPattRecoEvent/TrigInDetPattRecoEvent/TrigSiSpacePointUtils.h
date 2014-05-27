/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIG_SISPACEPOINT_UTILS_H__
#define __TRIG_SISPACEPOINT_UTILS_H__

#include <iostream>
#include "TrigInDetEvent/TrigSiSpacePointBase.h"


struct orderSpacePointsInR {
  bool operator ()(const TrigSiSpacePointBase* sp1, const TrigSiSpacePointBase* sp2) {
    return (sp1->r() < sp2->r());
  }
};

struct orderSpacePointsInLayer {
  bool operator ()(const TrigSiSpacePointBase* sp1, const TrigSiSpacePointBase* sp2) {
    return (sp1->layer() < sp2->layer());
  }
};

struct orderSpacePointsInIndex {
  bool operator ()(const TrigSiSpacePointBase* sp1, const TrigSiSpacePointBase* sp2) {
    return (sp1->index() < sp2->index());
  }
};

struct equalSpacePointsInIndex {
  bool operator ()(const TrigSiSpacePointBase* sp1, const TrigSiSpacePointBase* sp2) {
    return (sp1->index() == sp2->index());
  }
};

inline std::ostream& operator<<(std::ostream& s, 
				const TrigSiSpacePointBase& sp) {
  return   s << "[ i="     << sp.index() 
	     << "\tlyr="   << sp.layer() 
	     << "\trho="   << sp.r() 
	     << "\tphi="   << sp.phi() 
	     << "\tzed="   << sp.z() 
	     << "\t( eta=" << sp.eta() << " )\t]";
}

#endif
