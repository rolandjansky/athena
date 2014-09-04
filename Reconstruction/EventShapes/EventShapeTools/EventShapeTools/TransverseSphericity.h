/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTSHAPETOOLS_TRANSVERSESPHERICITY_H
#define EVENTSHAPETOOLS_TRANSVERSESPHERICITY_H

#include "EventShapeTools/EventShapeToolBase.h"

class TransverseSphericity : public EventShapeToolBase
{
 public:
  
  TransverseSphericity(const std::string& type,
		       const std::string& name,
		       const IInterface* pParent);
  
  virtual StatusCode calculate(const EventShapeTypes::es_4vect_vect_t* theParticles,
			       EventShapeStore *eventShapeStore );
};

#endif
