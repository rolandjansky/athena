/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigMissingETHypoToolMTMT_H
#define TrigMissingETHypoToolMTMT_H
/********************************************************************
 *
 * NAME:     TrigMissingETHypoToolMTMT.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigMissingETHypo
 *
 *
 *********************************************************************/


#include "DecisionHandling/HLTIdentifier.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "AthenaMonitoring/GenericMonitoringTool.h"

#include "xAODTrigMissingET/TrigMissingETContainer.h"
#include "TrigMissingETHypo/ITrigMissingETHypoToolMT.h"

class TrigMissingETHypoToolMT: public extends<AthAlgTool, ITrigMissingETHypoToolMT>  {

 public:
  
  TrigMissingETHypoToolMT(const std::string& type,
                    const std::string& name,
                    const IInterface* parent);
  virtual ~TrigMissingETHypoToolMT();
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  virtual StatusCode
    decide(const xAOD::TrigMissingETContainer*, bool& pass) const override;
  virtual const HLT::Identifier& getId() const override; 
 private:

  // Identifier is used to keep track of which tool made which decision.
  // The information is stored in the event store.
  HLT::Identifier m_decisionId;    

 private:

 Gaudi::Property<float>
   m_metThreshold{this, "metThreshold", 50, "MET threshold [GeV]" };
 
 // Monitored variables...
 /*
  declareMonitoredVariable("Et", m_et);
  declareMonitoredVariable("Eta", m_eta);
  declareMonitoredVariable("Phi", m_phi);
*/


};
#endif

