/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTSHAPETOOLS_EEC_H
#define EVENTSHAPETOOLS_EEC_H

#include "EventShapeTools/EventShapeToolBase.h"

class EEC : public EventShapeToolBase
{
 public:
  
  EEC(const std::string& type,
      const std::string& name,
      const IInterface* pParent);
  
  virtual StatusCode calculate(const EventShapeTypes::es_4vect_vect_t* theParticles,
			       EventShapeStore *eventShapeStore );
  
 private:
  
  long n_Events;
};

#endif
