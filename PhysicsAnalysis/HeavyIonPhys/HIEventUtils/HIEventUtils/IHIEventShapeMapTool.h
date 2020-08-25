/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HIEVENTUTILS_IHIEVENTSHAPEMAPTOOL_H
#define HIEVENTUTILS_IHIEVENTSHAPEMAPTOOL_H

#include "HIEventUtils/HIEventDefs.h"
#include "xAODHIEvent/HIEventShape.h"
#include "HIEventUtils/HIEventShapeIndex.h"
#include "xAODHIEvent/HIEventShapeContainer.h"
#include "AsgTools/IAsgTool.h"
#include "AsgTools/AsgTool.h"

#include <string>
#include <map>

class IHIEventShapeMapTool : virtual public asg::AsgTool
{
    ASG_TOOL_INTERFACE(IHIEventShapeMapTool)

    public:

    virtual ~IHIEventShapeMapTool() {};

    virtual StatusCode initialize() = 0;
    virtual const HIEventShapeIndex* getIndex(HI::BinningScheme key) const  = 0;
    virtual const HIEventShapeIndex* getIndexFromShape(const xAOD::HIEventShapeContainer* shape) const = 0;

    virtual bool hasKey(HI::BinningScheme key)  = 0;


};
#endif
