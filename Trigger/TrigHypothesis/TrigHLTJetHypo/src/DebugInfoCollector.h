/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_DEBUGINFOCOLLECTOR_H
#define TRIGHLTJETHYPO_DEBUGINFOCOLLECTOR_H

#include "./ITrigJetHypoInfoCollector.h"
#include "./JetTrigTimer.h"
#include <map>
#include <vector>

class DebugInfoCollector: public ITrigJetHypoInfoCollector {
    
public:
  DebugInfoCollector(const std::string& name="Unknown",
		     bool byTime=true);

  ~DebugInfoCollector();

  virtual void collect(const std::string&, const std::string&) override;

  virtual std::string toString() const override;

  virtual void write() const override;

 private:

  //info : key is a string porovided by callee
  //value is an vector of (time, message).
  std::map<std::string, std::vector<std::pair<double, std::string>>> m_info;
  std::string m_name;
  bool m_byTime;

  JetTrigTimer m_timer;
  std::string toStringByMsgKey() const;
  std::string toStringByTime() const;

};
#endif
