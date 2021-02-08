/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_IJETGROUPPRODUCT_H
#define TRIGHLTJETHYPO_IJETGROUPPRODUCT_H

#include "./DebugInfoCollector.h"
#include <vector>
#include <optional>
#include <memory>

typedef std::unique_ptr<ITrigJetHypoInfoCollector> Collector;

class IJetGroupProduct{
  /*
   * Interface to jet group amkers for FastReducer. 
   */
 public:
  virtual std::optional<std::vector<std::size_t>> next(const Collector&) = 0;
};

#endif
