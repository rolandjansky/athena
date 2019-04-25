/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETHYPOTOOLMT_H
#define TRIGJETHYPOTOOLMT_H
/********************************************************************
 *
 * NAME:     TrigJetHypoToolMT.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 *
 *********************************************************************/


#include "DecisionHandling/HLTIdentifier.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "AthenaMonitoring/GenericMonitoringTool.h"


#include "ITrigJetHypoToolHelperMT.h"
#include "ITrigJetHypoToolMT.h"

#include "xAODEventInfo/EventInfo.h"

struct EventSN{
  std::size_t m_id{0};
  std::size_t getSN(){return m_id++;}
};


class TrigJetHypoToolMT: public extends<AthAlgTool, ITrigJetHypoToolMT> {

public:

  TrigJetHypoToolMT(const std::string& type,
                      const std::string& name,
                      const IInterface* parent);
virtual ~TrigJetHypoToolMT();
virtual StatusCode initialize() override;
virtual StatusCode finalize() override;

// ITrigJetHypoToolMT interface
virtual StatusCode
decide(const xAOD::JetContainer*, bool& pass) const override;
virtual const HLT::Identifier& getId() const override; 
private:

// Identifier is used to keep track of which tool made which decision.
// The information is stored in the event store.
HLT::Identifier m_decisionId;  

private:

ToolHandle<ITrigJetHypoToolHelperMT> m_helper {
this, "helper_tool", {}, "Jet hypo helper AlgTool"};

Gaudi::Property<bool>
m_visitDebug {this, "visit_debug", false, "debug flag"};


 SG::ReadHandleKey<xAOD::EventInfo> m_evt{
   this, "Event", "Event", "read handle key for event info object"};
 
 std::unique_ptr<EventSN> m_eventSN;


};
#endif
