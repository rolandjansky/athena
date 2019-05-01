/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_DEBUGINFOCOLLECTOR_H
#define TRIGHLTJETHYPO_DEBUGINFOCOLLECTOR_H

#include "./ITrigJetHypoInfoCollector.h"
#include <map>

class DebugInfoCollector: public ITrigJetHypoInfoCollector {
    
public:
  DebugInfoCollector(const std::string& name="Unknown");
  virtual void collect(const std::string&, const std::string&) override;

  virtual std::string toString() const override;
  virtual void write() const override;

 private:
  std::map<std::string, std::string> m_info;
  std::map<std::string, std::size_t> m_calls;
  std::string m_name;

};
#endif
