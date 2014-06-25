/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMON_TE_H
#define TRIGMON_TE_H

/**
  @author Rustem Ospanov
  @date July 2009 

  @brief Summary of one trigger element from navigation tree.
*/

// Framework
#include "CLIDSvc/CLASS_DEF.h"

// C/C++
#include <iostream>
#include <stdint.h>
#include <vector>

// Local
#include "TrigMonitoringEvent/TrigConfSeq.h"
#include "TrigMonitoringEvent/TrigMonVar.h"

class TrigMonTE
{
 public:
  
  enum Type { 
    kELEM    = 0,      // Regular TE in navigation tree (default)
    kINIT    = 1,      // Initial TE 
    kROI     = 2,      // RoI TE attached to initial TE
    kL1TH    = 3       // L1 threshold TE attached to ROI TE
  };
  enum State {
    activeState   = 0x010000,  // Active/inactive state of TE
    errorState    = 0x020000,  // There was an error in algorithms dealing with this TE
    terminalNode  = 0x040000,  // Node is terminal (no more TriggerElement are seeded by it)
    outputL2Node  = 0x080000,  // This is output TE of passed or passedRaw L2 chain
    outputEFNode  = 0x100000,  // This is output TE of passed or passedRaw EF chain
    topologicalTE = 0x200000   // Topological TE (more than 1 parent)
  };
    
  TrigMonTE();
  TrigMonTE(uint32_t id, uint16_t index);
  ~TrigMonTE() {}
    
  void addChildIndex (uint16_t index) { m_child.push_back(index); }
  void addParentIndex(uint16_t index) { m_parent.push_back(index); }

  void addRoiId(uint8_t roiid)  { m_roi.push_back(roiid); }
  void addClid (uint32_t clid) { m_clid.push_back(clid); }
  
  void addVar(const TrigMonVar &var);
  
  void setType (Type  type);
  void addState(State state);
  
  void clearIndexes() { m_child.clear(); m_parent.clear(); }
  void clearClids()   { m_clid.clear(); }
  
  uint32_t getId()     const { return m_id; }
  uint16_t getIndex()  const;
  Type     getType()   const;
  
  bool getActiveState()  const { return m_encoded & activeState; }
  bool getErrorState()   const { return m_encoded & errorState; }
  bool isTerminalNode()  const { return m_encoded & terminalNode; }
  bool isOutputL2Node()  const { return m_encoded & outputL2Node; }
  bool isOutputEFNode()  const { return m_encoded & outputEFNode; }
  bool isTopologicalTE() const { return m_encoded & topologicalTE; }
  
  const std::vector<uint16_t>& getChildIndex()  const { return m_child; }
  const std::vector<uint16_t>& getParentIndex() const { return m_parent; }

  const std::vector<uint8_t>&  getRoiId() const { return m_roi; }
  const std::vector<uint32_t>& getClid()  const { return m_clid; }
  
  const std::vector<TrigMonVar> getVar()    const;
  const std::vector<uint16_t>&  getVarKey() const { return m_var_key; }
  const std::vector<float>&     getVarVal() const { return m_var_val; }

  void print(std::ostream &os = std::cout);

 private:
  
  uint32_t                 m_id;           // Trigger element id
  uint32_t                 m_encoded;      // Encoded word (see below)
  
  std::vector<uint16_t>    m_child;        // Index of child (successor) TEs
  std::vector<uint16_t>    m_parent;       // Index of parent (predecessors) TEs
  std::vector<uint8_t>     m_roi;          // RoiId of seeding roi(s)
  std::vector<uint32_t>    m_clid;         // Clids of attached features

  std::vector<uint16_t>    m_var_key;      // Variable key
  std::vector<float>       m_var_val;      // Variable value
};

std::string str(const TrigMonTE &);
  
//
// m_encoded: low 16 bits store index
//
// m_encoded: top 16 bits store State and Type enums
//
// state      [s] 6 bits
// type       [t] 2 bits
// free       [f] 8 bits
// 

//
// Global comparison operators
//
inline bool operator==(const TrigMonTE &lhs, const TrigMonTE &rhs) { 
  return lhs.getId() == rhs.getId();
}
inline bool operator <(const TrigMonTE &lhs, const TrigMonTE &rhs) { 
  return lhs.getId() < rhs.getId();
}

inline bool operator==(const TrigMonTE &te, const TrigConfSeq &seq) { 
  return te.getId() == seq.getId();
}
inline bool operator==(const TrigConfSeq &seq, const TrigMonTE &te) { 
  return te.getId() == seq.getId();
}

CLASS_DEF( TrigMonTE , 233333104 , 1 )

#endif
