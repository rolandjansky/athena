/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigTauGenericHypoMT_H
#define TrigTauGenericHypoMT_H

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
//#include "TrigInterfaces/HypoAlgo.h"


#include "Gaudi/Property.h"
#include "xAODTau/TauJet.h"
#include "xAODTau/TauJetContainer.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"

#include "ITrigTauGenericHypoTool.h"

class StoreGateSvc;

namespace HLT {
  class TriggerElement;
}

class TrigTauGenericHypoMT : public extends<AthAlgTool, ITrigTauGenericHypoTool> {
 public:
  TrigTauGenericHypoMT( const std::string& type, 
           const std::string& name, 
           const IInterface* parent );

  virtual ~TrigTauGenericHypoMT();

  virtual StatusCode initialize() override;

  virtual StatusCode decide( std::vector<ITrigTauGenericHypoTool::ClusterInfo>& input )  const override;

  virtual bool decide( const ITrigTauGenericHypoTool::ClusterInfo& i ) const override;

 private:

  /** Configurables */
  HLT::Identifier m_decisionId;

  std::vector<int> m_member;
  std::vector<std::string> m_formula;
  std::vector<TFormula> m_store;

  Gaudi::Property<bool>  m_decisionPerCluster{ this, "DecisionPerCluster", true, "Is multiplicity requirement refering to taus ( false ) or RoIs/clusters with taus ( false ), relevant only in when multiplicity > 1" };

  Gaudi::Property<bool>  m_acceptAll{ this, "AcceptAll", false, "Ignore selection" };

  ToolHandle<GenericMonitoringTool> m_monTool{ this, "MonTool", "", "Monitoring tool" };

};
#endif

