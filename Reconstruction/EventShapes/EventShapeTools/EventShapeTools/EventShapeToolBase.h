/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTSHAPETOOLS_EVENTSHAPETOOLBASE_H
#define EVENTSHAPETOOLS_EVENTSHAPETOOLBASE_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "EventShapeEvent/EventShapeStore.h"
#include "EventShapeEvent/EventShapeTypes.h"

#include "EventShapeInterface/IEventShapeToolBase.h"

class EventShapeToolBase : public AthAlgTool,
                           virtual public IEventShapeToolBase
{
  
 public:
  
  EventShapeToolBase(const std::string& type,
		     const std::string& name,
		     const IInterface* pParent)
    : AthAlgTool(type, name, pParent)
    { m_cutCentral=0; };
  
  virtual StatusCode initialize()
  { return StatusCode::SUCCESS; };
  
  virtual StatusCode initTool(double cutCentral)
  { m_cutCentral = cutCentral; return StatusCode::SUCCESS; };
  
  virtual StatusCode calculate(const EventShapeTypes::es_4vect_vect_t* theParticles,
			       EventShapeStore *eventShapeStore ) = 0;
  
  virtual StatusCode finalize()
  { return StatusCode::SUCCESS; };
  
 protected:
  
  double m_cutCentral;
  
};

#endif
