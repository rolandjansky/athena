/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigTauHypo_TrigTauGenericHypoMT_H
#define TrigTauHypo_TrigTauGenericHypoMT_H

/********************************************************************
 *
 * NAME:     TrigTauGenericHypoMT.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigTauHypo
 *
 * AUTHORS:   P.O. DeViveiros
 * CREATED:   Sometime in early 2015
 *
 * DESCRIPTION:
 *
 * Implementation of generic cuts on TauID variables
 *
 *********************************************************************/

#include <vector>
#include "TFormula.h"

#include "Gaudi/Property.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"

#include "ITrigTauGenericHypoTool.h"
#include "TrigCompositeUtils/HLTIdentifier.h"

class TrigTauGenericHypoMT : public extends<AthAlgTool, ITrigTauGenericHypoTool> {
 public:
  TrigTauGenericHypoMT( const std::string& type, 
           const std::string& name, 
           const IInterface* parent );

  virtual StatusCode initialize() override;

  virtual StatusCode decide( std::vector<ITrigTauGenericHypoTool::ClusterInfo>& input )  const override;

  virtual bool decide( const ITrigTauGenericHypoTool::ClusterInfo& i ) const override;

 private:

  HLT::Identifier m_decisionId;
  std::vector<int> m_member;
  std::vector<std::string> m_formula;
  std::vector<TFormula> m_store;

  Gaudi::Property<bool>  m_decisionPerCluster{ this, "DecisionPerCluster", true, "Is multiplicity requirement refering to taus ( false ) or RoIs/clusters with taus ( false ), relevant only in when multiplicity > 1" };

  Gaudi::Property<bool>  m_acceptAll{ this, "AcceptAll", false, "Ignore selection" };

  ToolHandle<GenericMonitoringTool> m_monTool{ this, "MonTool", "", "Monitoring tool" };

};
#endif

