/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file  HiveExSvc.h
 * @brief Simple service that accumulates timings for Algorithms by name
 */


#ifndef HIVEEXSVC_H
#define HIVEEXSVC_H 1
 
#include "AthExHive/IHiveExSvc.h"
#include "GaudiKernel/ContextSpecificPtr.h"
#include "AthenaBaseComps/AthService.h"

#include <string>
#include <vector>
#include <list>
#include <mutex>
#include <memory>

class HiveExSvc : public extends1<AthService,IHiveExSvc> {

public:
  HiveExSvc(const std::string& name, ISvcLocator* svc);
  virtual ~HiveExSvc();
  
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  
  virtual void add(const std::string&, const unsigned int&) override;

private:

  // data structure to hold timing info
  struct tDat {
    tDat(const std::string& n, const unsigned int& t): algName(n), sleep_time(t){};
    std::string  algName {""};
    unsigned int sleep_time {0};
  };

  // vector of time data, one entry per event slot  
  std::vector< std::list<tDat> > m_times;

  // can't use a simple vector of mutexes, as the aren't copy/move
  // contructible
  std::vector< std::unique_ptr<std::mutex> > m_locks;
  
};

#endif
