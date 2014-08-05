/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCTRUTHSIMALGS_MERGETRUTHJETSTOOL_H
#define MCTRUTHSIMALGS_MERGETRUTHJETSTOOL_H

#include "PileUpTools/PileUpToolBase.h"

#include "AsgTools/AsgTool.h"
#include "JetInterface/IJetExecuteTool.h"
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
  StatusCode initialize();
  ///called before the subevts loop. Not (necessarily) able to access
  ///SubEvents
  virtual StatusCode prepareEvent(unsigned int nInputEvents);
  ///called at the end of the subevts loop. Not (necessarily) able to access
  ///SubEvents
  virtual StatusCode mergeEvent();
  ///called for each active bunch-crossing to process current SubEvents
  /// bunchXing is in ns
  virtual StatusCode
    processBunchXing(int bunchXing,
                     PileUpEventInfo::SubEvent::const_iterator bSubEvents,
                     PileUpEventInfo::SubEvent::const_iterator eSubEvents);
  ///Merge the Truth JetContainers using the PileUpMergeSvc
  virtual StatusCode processAllSubEvents();

private:
  /// JetContainer Loop
  virtual void processJetContainer(const xAOD::JetContainer* inputJetContainer, xAOD::JetContainer *outputJetContainer, const double& ptCut, const float& timeOfBCID);
  StatusCode record(const xAOD::JetContainer* pjets, std::string jetcontainername) const;
  ToolHandle<IJetExecuteTool> m_intool;
  ServiceHandle<PileUpMergeSvc> m_pMergeSvc;
  xAOD::JetContainer *m_inTimeOutputJetContainer;
  xAOD::JetContainer *m_outOfTimeOutputJetContainer;
  std::string m_inputJetCollKey;
  std::string m_inTimeOutputJetCollKey;
  std::string m_outOfTimeOutputJetCollKey;
  double m_inTimePtCut;
  double m_outOfTimePtCut;
};
#endif //MCTRUTHSIMALGS_MERGETRUTHJETSTOOL_H
