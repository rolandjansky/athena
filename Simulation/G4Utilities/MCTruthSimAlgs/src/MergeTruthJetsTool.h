/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCTRUTHSIMALGS_MERGETRUTHJETSTOOL_H
#define MCTRUTHSIMALGS_MERGETRUTHJETSTOOL_H

#include "PileUpTools/PileUpToolBase.h"

#include "xAODJet/JetContainer.h"

#include "GaudiKernel/Property.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include <string>

class PileUpMergeSvc;
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
  virtual StatusCode prepareEvent(unsigned int nInputEvents) override final;
  ///called at the end of the subevts loop. Not (necessarily) able to access
  ///SubEvents
  virtual StatusCode mergeEvent() override final;
  ///called for each active bunch-crossing to process current SubEvents
  /// bunchXing is in ns
  virtual StatusCode processBunchXing(int bunchXing,
                                      SubEventIterator bSubEvents,
                                      SubEventIterator eSubEvents) override final;
  ///Merge the Truth JetContainers using the PileUpMergeSvc
  virtual StatusCode processAllSubEvents() override final;

  // ///implementation of passing filter
  // virtual bool filterPassed() const override final { return (!m_vetoOnInTime || m_filterPassed); }

  ///implementation of filter reset
  virtual void resetFilter() override final { m_first_event=true; m_signal_max_pT=-1.;  m_pileup_max_pT=-1.; m_filterPassed=true; }

private:
  /// JetContainer Loop
  virtual double processJetContainer(const xAOD::JetContainer* inputJetContainer, xAOD::JetContainer *outputJetContainer, const double& ptCut, const float& timeOfBCID);
  StatusCode record(const xAOD::JetContainer* pjets, std::string jetcontainername) const;
  ServiceHandle<PileUpMergeSvc> m_pMergeSvc;
  xAOD::JetContainer *m_inTimeOutputJetContainer;
  xAOD::JetContainer *m_outOfTimeOutputJetContainer;
  std::string m_inputJetCollKey;
  std::string m_inTimeOutputJetCollKey;
  std::string m_outOfTimeOutputJetCollKey;
  double m_inTimePtCut;
  double m_outOfTimePtCut;
  bool m_activateFilter;
  bool m_includeSignalJets;
  bool m_first_event;
  double m_signal_max_pT;
  double m_pileup_max_pT;
};
#endif //MCTRUTHSIMALGS_MERGETRUTHJETSTOOL_H
