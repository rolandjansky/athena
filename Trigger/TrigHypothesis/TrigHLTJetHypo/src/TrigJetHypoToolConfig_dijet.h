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
#include "./ConditionsDefsMT.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"

#include "./ConditionsDefsMT.h"
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
  virtual std::unique_ptr<IGroupsMatcherMT> getMatcher() const override;
  virtual std::optional<ConditionsMT> getConditions() const override;
  virtual std::size_t requiresNJets() const override {return 2;}


 private:

  Gaudi::Property<std::vector<std::string>>
    m_massMins_str{this, "mass_mins", {}, "min mass for each dijet"};

  Gaudi::Property<std::vector<std::string>>
    m_massMaxs_str{this, "mass_maxs", {}, "max mass for each dijet"};

  Gaudi::Property<std::vector<std::string>>
    m_dEtaMins_str{this, "deta_mins", {}, "min dEta for jets in each dijet"};

  Gaudi::Property<std::vector<std::string>>
    m_dEtaMaxs_str{this, "deta_maxs", {}, "max dEta for jets in each dijet"};

  Gaudi::Property<std::vector<std::string>>
    m_dPhiMins_str{this, "dphi_mins", {}, "min dPhi for jets in each dijet"};

  Gaudi::Property<std::vector<std::string>>
    m_dPhiMaxs_str{this, "dphi_maxs", {}, "max dPhi for jets in each dijet"};

  std::vector<double> m_massMins;
  std::vector<double> m_massMaxs;
  std::vector<double> m_dEtaMins;
  std::vector<double> m_dEtaMaxs;
  std::vector<double> m_dPhiMins;
  std::vector<double> m_dPhiMaxs;

  virtual StatusCode checkVals()  const override;
};
#endif
