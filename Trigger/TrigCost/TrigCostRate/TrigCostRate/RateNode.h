/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_RATENODE_H
#define ANP_RATENODE_H

/**********************************************************************************
 * @Package: TrigCostRate
 * @Class  : RateNode
 * @Author : Rustem Ospanov
 *
 * @Brief  : Node class for trigger rate calculation tree
 * 
 **********************************************************************************/

// C/C++
#include <vector>

namespace Anp
{
  class RateNode {
  public:

    enum NodeType {
      kUnknown,
      kNodeRoot,      // Top level root node
      kNodeDecis,     // Node for item/chain decision
      kNodeL1,        // Node for PS decision/weight
      kNodePS,        // Node for PS decision/weight
      kNodePT,        // Node for PT decision/weight
      kNodeCPS        // Node for coherent PS tree
    };

  public:

    //
    // Ctor for root node
    //
    RateNode();

    //
    // Ctor for simple trigger node - but can have more than one parent
    //
    RateNode(NodeType type, RateNode *parent, unsigned *decision);

    ~RateNode();

    void AddChild(RateNode *child);
    
    void SetWeight(double weight);
    
    double GetNodeWeight() const;

    double ComputeWeight() const;

    std::string GetTypeAsString() const;
    
    const std::vector<RateNode *>& GetChildren() const { return fChildren; }

    void Print(unsigned margin = 0, bool print_children = false) const;

    unsigned CountNodes() const;

  private:

    NodeType                   fType;         // Node type
    unsigned                  *fDecision;     // Chain decision status for this event
    double                     fWeight;       // Weight contribution    

    std::vector<RateNode *>    fChildren;     // List of (not-owned) children nodes
  };

  inline double RateNode::ComputeWeight() const {
    //
    // Check trigger decision
    //  
    if(fDecision && !(*fDecision)) {
      return 0.0;
    }

    if(fChildren.empty()) {
      return fWeight;
    }
    
    //
    // Compute recursively weight of children nodes
    //
    double weight = 0.0;
    
    for(std::vector<RateNode *>::const_iterator it = fChildren.begin(); it != fChildren.end(); ++it) {
      const double child_weight = (*it)->ComputeWeight();
      weight = child_weight + weight*(1.0 - child_weight);
    }
    
    return weight*fWeight;
  }
}

#endif
