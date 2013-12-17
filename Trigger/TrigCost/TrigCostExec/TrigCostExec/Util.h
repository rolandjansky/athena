/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// C/C++
#include <set>
#include <string>
#include <vector>

// Trigger
#include "TrigMonitoringEvent/TrigMonEvent.h"

//local
#include "TrigCostExec/ExecBase.h"

namespace Anp
{

  struct ROBIN {
    ROBIN() : robin("") {}
    
    std::string        robin;
    uint32_t           robin_id;
    std::set<uint32_t> robs;
  };

  struct ROS {
    ROS() : ros_id(0) {}
    
    std::string        ros;
    uint32_t           ros_id;
    std::set<uint32_t> robs;
    std::vector<ROBIN> robins;
  };

  bool ReadROSXML(const std::string &xml_path, std::vector<ROS> &ros_vec, bool debug = false);

  std::set<uint32_t> ReadROB(Registry &ros, bool debug = false);

  std::string AsStringRoIType(TrigMonRoi::Type type);
  std::string AsStringDetType(unsigned         type);

  TH1* MakeLBHist(const std::string &hname, const std::string &yaxis, unsigned min_lb, unsigned max_lb, TDirectory *dir);

  std::pair<unsigned,unsigned> GetMaxLB(const Exec::Summary &sum);

}
