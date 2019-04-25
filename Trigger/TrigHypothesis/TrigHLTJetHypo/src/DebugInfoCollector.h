/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_DEBUGINFOCOLLECTOR_H
#define TRIGHLTJETHYPO_DEBUGINFOCOLLECTOR_H

#include "./ITrigJetHypoInfoCollector.h"
#include <map>

class DebugInfoCollector: public ITrigJetHypoInfoCollector {
    
public:
  virtual void collect(const std::string&, const std::string&) override;

  std::string toString() const noexcept;
  void write(std::string, uint32_t run, uint32_t event) const;
  void write(std::string, std::size_t) const;

 private:
  std::map<std::string, std::string> m_info;
  std::map<std::string, std::size_t> m_calls;  

};
#endif
