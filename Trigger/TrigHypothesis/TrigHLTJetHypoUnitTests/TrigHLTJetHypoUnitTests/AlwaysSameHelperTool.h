/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_ALWAYSSAMEHELPERTOOL_H
#define TRIGHLTJETHYPO_ALWAYSSAMEHELPERTOOL_H
/********************************************************************
 *
 * NAME:     AlwaysSameHelperTool.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * Checks lhs and rhs IChilddrenTrigJetHypoToolHelper c
 * set pass to true if both fail or if both pass.
 * used for hypo implementation comparisons
 *
 *********************************************************************/

#include "AthenaBaseComps/AthAlgTool.h"

#include "TrigHLTJetHypo/ITrigJetHypoToolHelper.h"
#include "TrigHLTJetHypo/../src/ITrigJetHypoToolConfig.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"
#include "TrigHLTJetHypo/../src/ITrigJetHypoInfoCollector.h"

class ITrigJetHypoInfoCollector;
class xAODJetCollector;

class AlwaysSameHelperTool: public extends<AthAlgTool, ITrigJetHypoToolHelper> {

 public:

  AlwaysSameHelperTool(const std::string& type,
		       const std::string& name,
		       const IInterface* parent);
  
  bool pass(HypoJetVector&,
	    xAODJetCollector&,
            const std::unique_ptr<ITrigJetHypoInfoCollector>&) const override;
  
  virtual StatusCode getDescription(ITrigJetHypoInfoCollector&) const override;
  virtual std::string toString() const override;
  virtual std::size_t requiresNJets() const override;
  
 private:

  Gaudi::Property<bool>
    m_pass {this, "passflag", false, "const pass value"};

};
#endif
