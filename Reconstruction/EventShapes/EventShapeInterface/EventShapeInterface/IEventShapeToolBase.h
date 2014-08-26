// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTSHAPEINTERFACE_IEVENTSHAPETOOLBASE_H
#define EVENTSHAPEINTERFACE_IEVENTSHAPETOOLBASE_H

#include "GaudiKernel/IAlgTool.h"

class EventShapeStore;
#include "EventShapeEvent/EventShapeTypes.h"

// Declaration of the interface ID ( interface id, major version, minor version) 
static const InterfaceID IID_IEventShapeToolBase("IEventShapeToolBase", 1 , 0); 

class IEventShapeToolBase : virtual public IAlgTool
{
 public:
  
  static const InterfaceID& interfaceID() { return IID_IEventShapeToolBase; }
  
  virtual ~IEventShapeToolBase() { };
  
  virtual StatusCode initialize() = 0;
  
  virtual StatusCode initTool(double cutCentral) = 0;
  
  virtual StatusCode calculate(const EventShapeTypes::es_4vect_vect_t* theParticles,
			       EventShapeStore *eventShapeStore ) = 0;
  
  virtual StatusCode finalize() = 0;
  
};
#endif
