/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/* IAlg tool that splits up incomming jets and sends then to its children */

#ifndef TRIGHLTJETHYPO_COMBINATIONSHELPERTOOL_H
#define TRIGHLTJETHYPO_COMBINATIONSHELPERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "ITrigJetHypoToolHelperMT.h"
#include "ConditionsDefsMT.h"
#include "ITrigJetHypoToolConfig.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"

class ITrigJetInfoCollector;
class xAODJetCollector;

class CombinationsHelperTool: public extends<AthAlgTool, ITrigJetHypoToolHelperMT> {
 public:
  
  CombinationsHelperTool(const std::string& type,
                         const std::string& name,
                         const IInterface* parent);
  

  virtual StatusCode initialize() override;

  virtual bool pass(HypoJetVector&,
		    xAODJetCollector&,
		    const std::unique_ptr<ITrigJetHypoInfoCollector>&
		    ) const override;

  virtual StatusCode getDescription(ITrigJetHypoInfoCollector&) const override;

 private:

  // Used to generate helper objects foe TrigHLTJetHelper
 // from user supplied values
 ToolHandle<ITrigJetHypoToolConfig> m_config {
   this, "HypoConfigurer", {}, "Configurer to set up TrigHLTJetHypoHelper2"}; 

  // Object to make jet groups. Jet groups
  // are vectors of jets selected from a jet vector
  // which is, in this case, the incoming jet vector.
  std::unique_ptr<IJetGrouper> m_grouper;

  ConditionsMT m_conditions;
  
   ToolHandleArray<ITrigJetHypoToolHelperMT> m_children {
     this, "children", {}, "list of child jet hypo helpers"};

   /* 
  Gaudi::Property<unsigned int>
    m_size{this, "groupSize", {}, "Jet group size"};
   */

  Gaudi::Property<int>
    m_parentNodeID {this, "parent_id", {}, "hypo tool tree parent node id"};
  
  Gaudi::Property<int>
    m_nodeID {this, "node_id", {}, "hypo tool tree node id"};


  bool testGroup(HypoJetVector&,
		 xAODJetCollector&,
		 const std::unique_ptr<ITrigJetHypoInfoCollector>&) const;
  void collectData(const std::string& setuptime,
                   const std::string& exetime,
		   const std::unique_ptr<ITrigJetHypoInfoCollector>&,
                   bool) const;

  std::string toString() const;

};
#endif
