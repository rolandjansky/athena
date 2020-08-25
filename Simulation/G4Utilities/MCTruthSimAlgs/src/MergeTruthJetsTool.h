/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCTRUTHSIMALGS_MERGETRUTHJETSTOOL_H
#define MCTRUTHSIMALGS_MERGETRUTHJETSTOOL_H

#include "PileUpTools/PileUpToolBase.h"

#include "xAODJet/JetContainer.h"

#include "Gaudi/Property.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "PileUpTools/PileUpMergeSvc.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <string>

/** @class MergeTruthJetsTool
 *  @brief an algorithm to merge truth jet collections in the overlay store
 */
class MergeTruthJetsTool : public PileUpToolBase {
public:
  MergeTruthJetsTool(const std::string& type,
                           const std::string& name,
                           const IInterface* parent);
  /// Initialize
  StatusCode initialize() override final;
  ///called before the subevts loop. Not (necessarily) able to access
  ///SubEvents
  virtual StatusCode prepareEvent(const EventContext& ctx, unsigned int nInputEvents) override final;
  ///called at the end of the subevts loop. Not (necessarily) able to access
  ///SubEvents
  virtual StatusCode mergeEvent(const EventContext& ctx) override final;
  ///called for each active bunch-crossing to process current SubEvents
  /// bunchXing is in ns
  virtual StatusCode processBunchXing(int bunchXing,
                                      SubEventIterator bSubEvents,
                                      SubEventIterator eSubEvents) override final;
  ///Merge the Truth JetContainers using the PileUpMergeSvc
  virtual StatusCode processAllSubEvents(const EventContext& ctx) override final;

  // ///implementation of passing filter
  // virtual bool filterPassed() const override final { return (!m_vetoOnInTime || m_filterPassed); }

  ///implementation of filter reset
  virtual void resetFilter() override final { m_first_event=true; m_signal_max_pT=-1.;  m_pileup_max_pT=-1.; m_filterPassed=true; }

private:
  /// JetContainer Loop
  virtual double processJetContainer(const xAOD::JetContainer* inputJetContainer, xAOD::JetContainer *outputJetContainer, const double& ptCut, const float& timeOfBCID);
  StatusCode record(const xAOD::JetContainer* pjets, std::string jetcontainername) const;
  ServiceHandle<PileUpMergeSvc> m_pMergeSvc{this, "PileUpMergeSvc", "PileUpMergeSvc", ""};
  Gaudi::Property<std::string> m_inputJetCollKey{this, "InputTruthJetCollKey", "AntiKt4TruthJets", ""};
  Gaudi::Property<std::string> m_inTimeOutputJetCollKey{this, "InTimeOutputTruthJetCollKey", "InTimeAntiKt4TruthJets", ""};
  Gaudi::Property<std::string> m_outOfTimeOutputJetCollKey{this, "OutOfTimeTruthJetCollKey", "OutOfTimeAntiKt4TruthJets", ""};
  Gaudi::Property<double> m_inTimePtCut{this, "InTimePtCut", 10.0*Gaudi::Units::GeV, ""};
  Gaudi::Property<double> m_outOfTimePtCut{this, "OutOfTimePtCut", 15.0*Gaudi::Units::GeV, ""};
  Gaudi::Property<bool> m_activateFilter{this, "ActivateFilter",  false, ""};
  Gaudi::Property<bool> m_includeSignalJets{this, "IncludeSignalJets", false, ""};
  xAOD::JetContainer *m_inTimeOutputJetContainer{};
  xAOD::JetContainer *m_outOfTimeOutputJetContainer{};
  bool m_first_event{true};
  double m_signal_max_pT{-1.0};
  double m_pileup_max_pT{-1.0};
};
#endif //MCTRUTHSIMALGS_MERGETRUTHJETSTOOL_H
