/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigStreamerHypoTool_H
#define TrigStreamerHypoTool_H
/********************************************************************
 *
 * NAME:     TrigStreamerHypoTool.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigStreamerHypo
 *
 *
 *********************************************************************/


#include "TrigCompositeUtils/HLTIdentifier.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"

#include "ITrigStreamerHypoTool.h"

class TrigStreamerHypoTool: public extends<AthAlgTool, ITrigStreamerHypoTool>  {

 public:
  
  TrigStreamerHypoTool(const std::string& type,
                    const std::string& name,
                    const IInterface* parent);
  virtual ~TrigStreamerHypoTool();
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  virtual StatusCode decide(std::vector<ITrigStreamerHypoTool::HypoInfo>& hypoInfo) const override;
  virtual const HLT::Identifier& getId() const override; 
 private:

  // Identifier is used to keep track of which tool made which decision.
  // The information is stored in the event store.
  HLT::Identifier m_decisionId;
  Gaudi::Property<bool> m_pass{ this, "Pass", true, "Pass or reject"};

};
#endif

