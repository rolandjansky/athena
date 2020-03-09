/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_ITRIGJETINFOCOLLECTOR_H
#define TRIGHLTJETHYPO_ITRIGJETINFOCOLLECTOR_H
#include <string>

class ITrigJetHypoInfoCollector{
    
public:
  virtual ~ITrigJetHypoInfoCollector() = default;
  virtual void collect(const std::string&, const std::string&) = 0;
  virtual std::string toString() const = 0;
  virtual void write() const = 0;
};
#endif
