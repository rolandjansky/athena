/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETHYPOTOOLConfig_DIJET_H
#define TRIGJETHYPOTOOLConfig_DIJET_H
/********************************************************************
 *
 * NAME:     TrigJetHypoToolConfig_dijetTool.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 *
 *********************************************************************/


#include "ITrigJetHypoToolConfig.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsDefs.h"
#include "DecisionHandling/HLTIdentifier.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "AthenaMonitoring/GenericMonitoringTool.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsDefs.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ICleaner.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJetGrouper.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CleanerBridge.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsDefs.h"

class TrigJetHypoToolConfig_dijet:
public extends<AthAlgTool, ITrigJetHypoToolConfig> {

 public:
  
  TrigJetHypoToolConfig_dijet(const std::string& type,
                          const std::string& name,
                          const IInterface* parent);
  virtual ~TrigJetHypoToolConfig_dijet();

  virtual StatusCode initialize() override;
  virtual std::vector<std::shared_ptr<ICleaner>> getCleaners() const override;
  virtual std::unique_ptr<IJetGrouper> getJetGrouper() const override;
  virtual Conditions getConditions() const override;

 private:

  Gaudi::Property<std::vector<double>>
    m_massMins{this, "mass_mins", {}, "min mass for each dijet"};

  Gaudi::Property<std::vector<double>>
    m_massMaxs{this, "mass_maxs", {}, "max mass for each dijet"};

  Gaudi::Property<std::vector<double>>
    m_dEtaMins{this, "deta_mins", {}, "min dEta for jets in each dijet"};

  Gaudi::Property<std::vector<double>>
    m_dEtaMaxs{this, "deta_maxs", {}, "max dEta for jets in each dijet"};

  Gaudi::Property<std::vector<double>>
    m_dPhiMins{this, "dphi_mins", {}, "min dPhi for jets in each dijet"};

  Gaudi::Property<std::vector<double>>
    m_dPhiMaxs{this, "dphi_maxs", {}, "max dPhi for jets in each dijet"};

  virtual StatusCode checkVals()  const override;

};
#endif
