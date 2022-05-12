/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HTTSPACEPOINTSTOOL_H
#define HTTSPACEPOINTSTOOL_H

#include <array>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigHTTObjects/HTTCluster.h"
#include "TrigHTTObjects/HTTHit.h"
#include "TrigHTTMaps/HTTSpacePointsToolI.h"

class HTTSpacePointsTool : public extends<AthAlgTool, HTTSpacePointsToolI> {
 public:
    HTTSpacePointsTool(const std::string &, const std::string &, const IInterface *);
    virtual ~HTTSpacePointsTool() = default;

    virtual StatusCode initialize() override;

    virtual StatusCode DoSpacePoints(HTTLogicalEventInputHeader &, std::vector<HTTCluster> &) override;

 private:
    void SpacePointFinder(const std::vector<HTTHit> &, std::vector<std::array<int, 2>> &);


    Gaudi::Property<bool> m_duplicate {this, "Duplication", false, "Duplicate spacepoint to layer on the other side of the stave"};
    Gaudi::Property<bool> m_filter {this, "Filtering", false, "Filter out incomplete spacepoints"};
    Gaudi::Property<bool> m_filterClose {this, "FilteringClosePoints", false, "Filter out single hits close to spacepoints"};
};

#endif // HTTSPACEPOINTSTOOL_H
