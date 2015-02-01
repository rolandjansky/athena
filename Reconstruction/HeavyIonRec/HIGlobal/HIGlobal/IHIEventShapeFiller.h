/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __INTERFACE_HIEVENTSHAPEMODIFIER_H__
#define __INTERFACE_HIEVENTSHAPEMODIFIER_H__

#include "AsgTools/IAsgTool.h"
#include "HIGlobal/HICaloHelper.h"
#include "xAODHIEvent/HIEventShapeContainer.h"

class CaloCell;

class IHIEventShapeFiller : virtual public asg::IAsgTool 
{
  ASG_TOOL_INTERFACE(IHIEventShapeFiller)
public:
  virtual ~IHIEventShapeFiller() {};
  //virtual void Modify(xAOD::HIEventShapeContainer*) = 0;

  virtual StatusCode InitializeCollection()=0;
  virtual StatusCode RecordCollection(const std::string &event_shape_key, const std::string &event_shapeaux_key)=0;
  virtual StatusCode UpdateWithCell(const CaloCell* theCell, float geoWeight) = 0;

  virtual const xAOD::HIEventShapeContainer* GetHIEventShapeContainer()=0;   
};

#endif
