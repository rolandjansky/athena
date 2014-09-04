/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTSHAPETOOLS_FOXWOLFRAM_H
#define EVENTSHAPETOOLS_FOXWOLFRAM_H

#include "EventShapeTools/EventShapeToolBase.h"

class FoxWolfram : public EventShapeToolBase
{
 public:
  
  FoxWolfram(const std::string& type,
	     const std::string& name,
	     const IInterface* pParent);
  
  virtual StatusCode calculate(const EventShapeTypes::es_4vect_vect_t* theParticles,
			       EventShapeStore *eventShapeStore );
  
  virtual StatusCode initialize();
  
 private:
  
  unsigned int m_order;
  
  bool m_calc3D;
  
  bool m_modified;
  
};

#endif
