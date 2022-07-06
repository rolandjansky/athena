// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#ifndef HTTROADFINDERI_H
#define HTTROADFINDERI_H

/**
 * @file IHTTRoadFinderTool.h
 * @author Riley Xu - rixu@cern.ch
 * @date 10/23/19
 * @brief Interface declaration for road finder tools
 *
 * This class is implemented by
 *      - HTTRoadUnionTool
 *      - HTTPatternMatchTool
 *      - HTTSectorMatchTool
 *      - HTTHoughTransformTool
 *      - HTTHough1DShiftTool
 */

#include "GaudiKernel/IAlgTool.h"

#include "TrigHTTObjects/HTTTypes.h"
#include "TrigHTTObjects/HTTRoad.h"

#include <vector>

class HTTHit;


/**
 * A road finder returns a vector of roads given a vector of hits.
 *
 * Note that the roads are owned by the tool, and are cleared at each successive
 * call of getRoads().
 */


class IHTTRoadFinderTool : virtual public IAlgTool
{
    public:
        DeclareInterfaceID(IHTTRoadFinderTool, 1, 0);
        virtual StatusCode getRoads(const std::vector<const HTTHit*> & hits, std::vector<HTTRoad*> & roads) = 0;
};


#endif
