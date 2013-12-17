/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_ROBSERVICE_H
#define ANP_ROBSERVICE_H

/**********************************************************************************
 * @Package: TrigCostExec
 * @Class  : ReadRob
 * @Author : Rustem Ospanov
 *
 * @Brief  : 
 *
 * Singleton class to match ROB ids to subdetectors and groups
 *  - this is clumsy because I do not want to depend on eformat
 *  - subdetector and group ids are simple index map
 *  - there is defintely easier way to do these lookups
 *
 **********************************************************************************/

#include <map>
#include <set>
#include <string>

// Local
#include "TrigCostExec/Util.h"
#include "TrigCostExec/ExecData.h"

class TrigMonConfig;
class TrigMonROB;

namespace Anp
{
  class Registry;
  
  class RobService {
  public:

    static RobService& Instance();

    void Config(const Registry &reg);

    void AddConfig(const TrigMonConfig &config);
    
    std::string GetSubDet(uint32_t rob_id) const;
    std::string GetGroup (uint32_t rob_id) const;

    uint32_t GetGroupIdFromSubDetId(uint32_t subdet_id) const;
    uint32_t GetSubDetId           (uint32_t rob_id)    const;
    uint32_t GetGroupId            (uint32_t rob_id)    const;
    uint32_t GetRosId              (uint32_t rob_id)    const;
    uint32_t GetRobinId            (uint32_t rob_id)    const;

    uint32_t GetNumberROBsInDet  (uint32_t subdet_id) const;
    uint32_t GetNumberROBsInGroup(uint32_t group_id)  const;

    std::string GetGrpFromSubDet (uint32_t subdet_id) const;
    std::string GetSubDetFromId  (uint32_t subdet_id) const;
    std::string GetGroupFromId   (uint32_t group_id)  const;
    std::string GetRosFromId     (uint32_t ros_id)    const;
    std::string GetRobinFromId   (uint32_t robin_id)  const;
    std::string GetHLTFromId     (uint32_t hlt_id)    const;

    std::vector<int> GetAllSubDet() const;
    
    std::set<uint32_t> GetSubDetFromROB(const TrigMonROB &rob) const;
    std::set<uint32_t> GetROSFromROB   (const TrigMonROB &rob) const;
    
    Exec::FarmNodeMap  GetFarmNodeMap() const {return  fHltMap;}
    Exec::FarmNodeMap *GetFarmNodeMap()       {return &fHltMap;}

  private:
    
    RobService();
    ~RobService() {}
    
    RobService(const RobService &);
    RobService& operator=(const RobService &);
    
    void ReadOldKey(const std::string &key, const std::string &val);
    void ReadNewKey(const std::string &key, const uint32_t val);
    
    std::ostream& log() const;

  private:

    struct SubDet {
      SubDet() : subdet_id(-1), group_id(-1) {}
      
      std::string subdet;
      uint32_t    subdet_id;

      std::string group;
      uint32_t    group_id;
    };
    
    bool fDebug;
    bool fUseLocal;

    std::map<unsigned, SubDet> fRobMap;   // ROB to subdetector map
    std::map<unsigned, ROS>    fRosMap;   // ROB to ROS map
    std::map<unsigned, ROBIN>  fRobinMap; // ROB to ROBIN map
    std::vector<ROS>           fRosVec;   // ROS data
    Exec::FarmNodeMap          fHltMap;   // Map HLT application id to name
  };
}

#endif
