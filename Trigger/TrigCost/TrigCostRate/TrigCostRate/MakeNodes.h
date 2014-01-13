/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_MAKENODES_H
#define ANP_MAKENODES_H

/**********************************************************************************
 * @Package: TrigCostRate
 * @Class  : MakeNodes
 * @Author : Rustem Ospanov
 *
 * @Brief  : This class make RateNode objects 
 *           - implements logic of trigger rate tree
 * 
 **********************************************************************************/

// C/C++
#include <bitset>
#include <functional>
#include <map>
#include <string>
#include <sstream>

// Trigger
#include "TrigMonitoringEvent/TrigMonConfig.h"

// Local
#include "TrigCostBase/Registry.h"

// Local
#include "TrigCostRate/RateChain.h"
#include "TrigCostRate/RateNode.h"

namespace Anp
{
  //
  // Class reads trigger configuration and creates tree for rate computation
  //
  class MakeNodes {
  public:

    //
    // Constructor do no work
    //
    MakeNodes();

    //
    // Destructor deletes owned objects
    //
    ~MakeNodes();

    //
    // Read trigger configuration
    //
    void ConfigReg(const Registry &reg);

    //
    // Read trigger configuration
    //
    void ConfigTrigger(const std::vector<TrigConfChain> &config);

    //
    // Update trigger configuration with new PS/PT
    //
    void UpdateTrigger(const std::vector<TrigConfChain> &config);

    //
    // Set trigger decisions for new event
    //
    void SetTrigger(const RateBits &bits);

    //
    // Create RateNode and RateElem objects for trigger chains
    //
    RateNode* CreateNode(const std::vector<TrigConfChain> &chains, const Registry &reg);

    //
    // Create RateNode and RateElem objects for trigger chains
    //
    RateNode* CreateMultiSeedNode(const TrigConfChain &chain, const Registry &reg);

  private:

    RateNode* MakeL1Node(RateNode *parent,
			 const TrigConfChain &item_config,
			 const std::vector<TrigConfChain> &menu_config);

    std::ostream& log() const;

  private:

    struct NodeData {

      NodeData() : chain(0), nodeL1(0), nodePS(0), nodePT(0), nodeDecis(0), nodeCPS(0) {}
      ~NodeData();

      TrigConfChain *chain;

      RateNode      *nodeL1;
      RateNode      *nodePS;
      RateNode      *nodePT;
      RateNode      *nodeDecis;
      RateNode      *nodeCPS;

      std::vector<NodeData *> children;
    };

    struct TrigData {

      TrigData() : decision(0) {}
      ~TrigData() {}
      
      double *decision;
    };
    
    typedef std::map<TrigConfChain *, NodeData *> NodeMap;
    typedef std::vector<std::pair<TrigConfChain *, unsigned *> > TrigVec;
    
    typedef std::vector<NodeData *> DataVec;
    typedef std::vector<RateNode *> RateVec;

  private:

    std::vector<TrigConfChain *> ConvertInput(const std::vector<TrigConfChain> &chains);

    std::vector<TrigConfChain *> GetInputChains(const TrigConfChain &chain);
    std::vector<TrigConfChain *> GetInputChains(const std::vector<TrigConfChain *> &chains);
    
    TrigConfChain* GetChain(const std::string &chain);
    
    std::map<std::string, std::vector<TrigConfChain *> > GetCPS(std::vector<TrigConfChain *> &chains) const;

    RateNode* MakeNode(RateNode *parent, TrigConfChain *chain, RateNode::NodeType type);

    NodeData* MakeData(NodeMap &nodes, TrigConfChain *chain);

    void FillTreeHLT(NodeMap &nodes, const std::vector<TrigConfChain *> &chains);
    void FillTreeCPS(NodeMap &nodes, std::vector<TrigConfChain *> chains);
    
    void Print(const std::vector<NodeData *> &vec, unsigned margin = 0);

    void Print(const RateNode *node, unsigned margin = 0);

  private:

    //
    // These are private and not implemented by design - do not change
    //
    MakeNodes(const MakeNodes &);
    MakeNodes& operator=(const MakeNodes &);

  private:
    
    bool                           fDebug;       // Print debug info
    bool                           fDoExpress;   // Include express stream PS
    bool                           fDoCPS;       // Treat CPS groups as regular groups
    bool                           fDoPTOnly;    // Turn off HLT trigger decisions - only PT rate

    TrigVec                        fChains;      // Trigger configuration and decision bits
    DataVec                        fData;        // Vector of NodeData objects one entry per trigger
    RateVec                        fRoot;        // Vector of RateNode roots
    
    unsigned                      *fDecisArray;  // Array of unsigned int with trigger decisions
  };
}

#endif
