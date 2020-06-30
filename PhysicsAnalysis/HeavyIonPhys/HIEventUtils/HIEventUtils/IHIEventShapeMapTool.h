/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HIEVENTUTILS_HIEVENTSHAPEMAPTOOL_H
#define HIEVENTUTILS_HIEVENTSHAPEMAPTOOL_H

#include "xAODHIEvent/HIEventShape.h"
#include "HIEventUtils/HIEventShapeIndex.h"
#include "AsgTools/IAsgTool.h"
#include "AsgTools/AsgTool.h"
#include <string>

class IHIEventShapeMapTool : virtual public asg::AsgTool
{
    ASG_TOOL_INTERFACE(IHITowerWeightTool)

    public:

    virtual ~IHIEventShapeMapTool() {};
    virtual StatusCode initialize() = 0;
    virtual const HIEventShapeIndex* getIndex(std::string key)  = 0;
    virtual bool hasKey(std::string key)  = 0;
    virtual std::map<std::string,HIEventShapeIndex> getMap() { return m_map; }

    private:
    std::map<std::string,HIEventShapeIndex> m_map;
};
#endif
