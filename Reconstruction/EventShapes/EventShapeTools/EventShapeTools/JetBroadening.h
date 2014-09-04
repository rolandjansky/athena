/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTSHAPETOOLS_JETBROADENING_H
#define EVENTSHAPETOOLS_JETBROADENING_H

#include "EventShapeTools/EventShapeToolBase.h"

class JetBroadening : public EventShapeToolBase
{
 public:
  
  JetBroadening(const std::string& type,
		const std::string& name,
		const IInterface* pParent);
  
  virtual StatusCode calculate(const EventShapeTypes::es_4vect_vect_t* theParticles,
			       EventShapeStore *eventShapeStore );
  
  virtual StatusCode initialize();
  
 private:
  
  bool m_calc3D;
  
};

#endif
