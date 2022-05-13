// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#ifndef HTTROADFILTERI_H
#define HTTROADFILTERI_H

/**
 * @file IHTTRoadFilterTool.h
 * @author Elliot Lipeles  lipeles@cern.ch
 * @date 03/25/21
 * @brief Interface declaration for road filter tools
 *
 * This class is implemented by
 *      - HTTEtaPatternFilterTool
 */

#include "GaudiKernel/IAlgTool.h"

#include "TrigHTTObjects/HTTTypes.h"
#include "TrigHTTObjects/HTTRoad.h"

#include <vector>


/**
 * A road filter returns a vector of roads given a vector of roads.
 *
 * Note that the postfilter_roads are owned by the tool, and are cleared at each successive
 * call of filterRoads().
 */

class IHTTRoadFilterTool : virtual public IAlgTool
{
    public:
        DeclareInterfaceID(IHTTRoadFilterTool, 1, 0);
        virtual StatusCode filterRoads(const std::vector<HTTRoad*> & prefilter_roads, std::vector<HTTRoad*> & postfilter_roads) = 0;
};


#endif
