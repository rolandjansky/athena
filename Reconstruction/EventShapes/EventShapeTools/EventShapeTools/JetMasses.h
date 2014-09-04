/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTSHAPETOOLS_JETMASSES_H
#define EVENTSHAPETOOLS_JETMASSES_H

#include "EventShapeTools/EventShapeToolBase.h"

class JetMasses : public EventShapeToolBase
{
 public:
  
  JetMasses(const std::string& type,
	    const std::string& name,
	    const IInterface* pParent);
  
  virtual StatusCode calculate(const EventShapeTypes::es_4vect_vect_t* theParticles,
			       EventShapeStore *eventShapeStore );

  virtual StatusCode initialize();
  
 private:
  
  bool m_calc3D;
  
};
#endif
