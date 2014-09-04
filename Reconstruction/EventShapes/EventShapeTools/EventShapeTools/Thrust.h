/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTSHAPETOOLS_THRUST_H
#define EVENTSHAPETOOLS_THRUST_H

#include "EventShapeTools/EventShapeToolBase.h"

class Thrust : public EventShapeToolBase
{
 public:
  
  Thrust(const std::string& type,
	 const std::string& name,
	 const IInterface* pParent);
  
  virtual StatusCode calculate(const EventShapeTypes::es_4vect_vect_t* theParticles,
			       EventShapeStore *eventShapeStore );
  
  virtual StatusCode initialize();
  
 private:
  
  bool m_calc3D;
  
};

#endif
