// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#ifndef HTTCLUSTERINGTOOLI_H
#define HTTCLUSTERINGTOOLI_H

/**
 * @file HTTClusteringToolI.h
 * @author Alex Martyniuk - martyniu@cern.ch
 * @date 04/12/19
 * @brief Declares an abstract class that implements an interface for pixel clustering.
 * This class is implemented in
 *      HTTClusteringTool.h
 *      HTTClusteringFTKTool.h
 *      HTTClusteringOfflineTool.h
 */

#include "GaudiKernel/IAlgTool.h"
#include "TrigHTTObjects/HTTHit.h"
#include "TrigHTTObjects/HTTCluster.h"
#include "TrigHTTObjects/HTTLogicalEventInputHeader.h"

class HTTClusteringToolI : virtual public ::IAlgTool
{
    public:
	DeclareInterfaceID( HTTClusteringToolI, 1, 0);
	virtual ~HTTClusteringToolI() = default;

	virtual StatusCode DoClustering(HTTLogicalEventInputHeader &, std::vector<HTTCluster> &) const = 0;
};

#endif //HTTCLUSTERINGTOOLI_H
