// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#ifndef HTTSPACEPOINTSTOOLI_H
#define HTTSPACEPOINTSTOOLI_H

/**
 * @file HTTSpacePointsToolI.h
 * @author Julian Wollrath - wollrath@cern.ch
 * @date 2021
 * @brief Declares an abstract class that implements an interface for spacepoint formation.
 * This class is implemented in
 *      HTTSpacePointsTool.h
 */

#include "GaudiKernel/IAlgTool.h"
#include "TrigHTTObjects/HTTCluster.h"
#include "TrigHTTObjects/HTTLogicalEventInputHeader.h"

const InterfaceID IID_HTTSpacePointsToolI("HTTSpacePointsToolI", 1, 0);

class HTTSpacePointsToolI : virtual public ::IAlgTool {
 public:
    DeclareInterfaceID(HTTSpacePointsToolI, 1, 0);
    virtual ~HTTSpacePointsToolI() = default;

    virtual StatusCode DoSpacePoints(HTTLogicalEventInputHeader &, std::vector<HTTCluster> &) = 0;
};

#endif // HTTSPACEPOINTSTOOLI_H
