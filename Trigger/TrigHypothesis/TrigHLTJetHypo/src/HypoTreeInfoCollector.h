/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_HYPOTREEINFOCOLLECTOR_H
#define TRIGHLTJETHYPO_HYPOTREEINFOCOLLECTOR_H

#include "./ITrigJetHypoInfoCollector.h"
#include <map>

class HypoTreeInfoCollector: public ITrigJetHypoInfoCollector {
    
public:
  virtual void collect(const std::string&, const std::string&) override;

  std::string toString() const;
  void write(std::string) const;

 private:
  std::map<std::string, std::string> m_info;

};
#endif
