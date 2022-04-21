/*
    Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HTTCLUSTERINGOFFLINETOOL_H
#define HTTCLUSTERINGOFFLINETOOL_H

/*
 * httClustering
 * ---------------
 *
 * Routines to perform clustering in the pixels, based on TrigHTTSim
 *
 */

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigHTTMaps/HTTClusteringToolI.h"
#include "TrigHTTObjects/HTTHit.h"
#include "TrigHTTObjects/HTTCluster.h"
#include <cmath>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <stack>
#include <queue>

class HTTClusteringOfflineTool : public extends <AthAlgTool,HTTClusteringToolI> {
public:

  HTTClusteringOfflineTool(const std::string&, const std::string&, const IInterface*);

  virtual ~HTTClusteringOfflineTool() = default;

  virtual StatusCode DoClustering(HTTLogicalEventInputHeader &, std::vector<HTTCluster> &) const override;

 private:

};

#endif // HTTCLUSTERINGOFFLINETOOL_H
