/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigMissingETHypoTool_H
#define TrigMissingETHypoTool_H
/********************************************************************
 *
 * NAME:     TrigMissingETHypoTool.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigMissingETHypo
 *
 *
 *********************************************************************/


#include "TrigCompositeUtils/HLTIdentifier.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"

#include "xAODTrigMissingET/TrigMissingETContainer.h"
#include "ITrigMissingETHypoTool.h"

class TrigMissingETHypoTool: public extends<AthAlgTool, ITrigMissingETHypoTool>  {

 public:
  
  TrigMissingETHypoTool(const std::string& type,
                    const std::string& name,
                    const IInterface* parent);
  virtual ~TrigMissingETHypoTool();
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

