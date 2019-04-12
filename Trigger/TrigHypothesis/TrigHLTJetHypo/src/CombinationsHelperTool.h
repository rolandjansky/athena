/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/* IAlg tool that splits up incomming jets and sends then to its children */

#ifndef TRIGHLTJETHYPO_COMBINATIONSHELPERTOOL_H
#define TRIGHLTJETHYPO_COMBINATIONSHELPERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "ITrigJetHypoToolHelperMT.h"
#include "ITrigJetHypoToolConfig.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"

class ITrigJetInfoCollector;

class CombinationsHelperTool: public extends<AthAlgTool, ITrigJetHypoToolHelperMT> {
 public:
  
  CombinationsHelperTool(const std::string& type,
                         const std::string& name,
                         const IInterface* parent);

  bool pass(HypoJetVector&, ITrigJetHypoInfoCollector*) const;

  virtual StatusCode getDescription(ITrigJetHypoInfoCollector&) const override;

 private:

   ToolHandleArray<ITrigJetHypoToolHelperMT> m_children {
     this, "children", {}, "list of child decidables"};
   
  Gaudi::Property<unsigned int>
    m_size{this, "groupSize", {}, "Jet group size"};
  

  Gaudi::Property<int>
    m_parentNodeID {this, "parent_id", {}, "hypo tool tree parent node id"};
  
  Gaudi::Property<int>
    m_nodeID {this, "node_id", {}, "hypo tool tree node id"};


  bool testGroup(HypoJetVector&, ITrigJetHypoInfoCollector*) const;
  void collectData(const std::string& setuptime,
                   const std::string& exetime,
                   ITrigJetHypoInfoCollector*,
                   bool) const;

  std::string toString() const;

};
#endif
