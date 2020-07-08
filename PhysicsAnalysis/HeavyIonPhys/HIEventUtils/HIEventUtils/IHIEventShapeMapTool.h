/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HIEVENTUTILS_IHIEVENTSHAPEMAPTOOL_H
#define HIEVENTUTILS_IHIEVENTSHAPEMAPTOOL_H

#include "xAODHIEvent/HIEventShape.h"
#include "HIEventUtils/HIEventShapeIndex.h"
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
    virtual StatusCode insert(std::string key, const HIEventShapeIndex& index, bool clobber=false) = 0;
    virtual const HIEventShapeIndex* getIndex(std::string key) const  = 0;
    virtual bool hasKey(std::string key)  = 0;

    virtual std::map<std::string,HIEventShapeIndex> getMap() const { return m_map; }

    private:
    std::map<std::string,HIEventShapeIndex> m_map;

};
#endif
