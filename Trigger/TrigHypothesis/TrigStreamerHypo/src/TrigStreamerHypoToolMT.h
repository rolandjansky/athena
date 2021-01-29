/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigStreamerHypoToolMTMT_H
#define TrigStreamerHypoToolMTMT_H
/********************************************************************
 *
 * NAME:     TrigStreamerHypoToolMTMT.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigStreamerHypo
 *
 *
 *********************************************************************/


#include "TrigCompositeUtils/HLTIdentifier.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"

#include "ITrigStreamerHypoToolMT.h"

class TrigStreamerHypoToolMT: public extends<AthAlgTool, ITrigStreamerHypoToolMT>  {

 public:
  
  TrigStreamerHypoToolMT(const std::string& type,
                    const std::string& name,
                    const IInterface* parent);
  virtual ~TrigStreamerHypoToolMT();
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  virtual StatusCode decide(std::vector<ITrigStreamerHypoToolMT::HypoInfo>& hypoInfo) const override;
  virtual const HLT::Identifier& getId() const override; 
 private:

  // Identifier is used to keep track of which tool made which decision.
  // The information is stored in the event store.
  HLT::Identifier m_decisionId;    

};
#endif

