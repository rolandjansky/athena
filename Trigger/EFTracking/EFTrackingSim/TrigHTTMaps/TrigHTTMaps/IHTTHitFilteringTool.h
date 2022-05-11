// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#ifndef IHTTHITFILTERINGTOOL_H
#define IHTTHITFILTERINGTOOL_H

/**
 * @file IHTTHitFilteringTool.h
 * @author Will Kalderon - willam.kalderon@cern.ch, Julian Wollrath - wollrath@cern.ch
 * @date 2021
 * @brief Declares an abstract class that implements an interface for hit/cluster filtering.
 * This class is implemented in
 *      HTTHitFilteringTool.h
 */

#include "GaudiKernel/IAlgTool.h"
#include "TrigHTTObjects/HTTCluster.h"
#include "TrigHTTObjects/HTTLogicalEventInputHeader.h"
#include "HTTPlaneMap.h"

class IHTTHitFilteringTool : virtual public ::IAlgTool {
 public:
  DeclareInterfaceID(IHTTHitFilteringTool, 1, 0);
  virtual ~IHTTHitFilteringTool() = default;
  
  virtual StatusCode DoRandomRemoval(HTTLogicalEventInputHeader &, bool) = 0;
  virtual StatusCode GetPairedStripPhysLayers(const HTTPlaneMap*, std::vector<int> &) = 0;
  virtual StatusCode DoHitFiltering(HTTLogicalEventInputHeader &,
                                    std::vector<int>, std::vector<int>,
                                    std::vector<HTTCluster> &) = 0;

};

#endif // IHTTHITFILTERINGTOOL_H
