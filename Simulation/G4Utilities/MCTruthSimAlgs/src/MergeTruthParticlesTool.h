/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCTRUTHSIMALGS_MERGETRUTHPARTICLESTOOL_H
#define MCTRUTHSIMALGS_MERGETRUTHPARTICLESTOOL_H

#include "PileUpTools/PileUpToolBase.h"

#include "xAODTruth/TruthParticleContainer.h"

#include "Gaudi/Property.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "PileUpTools/PileUpMergeSvc.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <string>

/** @class MergeTruthParticlesTool
 *  @brief an algorithm to merge truth particles collections in the overlay store
 */
class MergeTruthParticlesTool : public PileUpToolBase {
public:
  MergeTruthParticlesTool(const std::string& type,
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
  ///Merge the Truth TruthParticleContainers using the PileUpMergeSvc
  virtual StatusCode processAllSubEvents(const EventContext& ctx) override final;

private:
  /// TruthParticleContainer Loop
  virtual StatusCode processTruthParticleContainer(const xAOD::TruthParticleContainer* inputTruthParticleContainer,
                                                   xAOD::TruthParticleContainer *outputTruthParticleContainer,
                                                   int eventNumber);
  StatusCode record(const xAOD::TruthParticleContainer* pTruthParticles, const std::string& truthParticleContainerName) const;
  ServiceHandle<PileUpMergeSvc> m_pMergeSvc{this, "PileUpMergeSvc", "PileUpMergeSvc", ""};
  Gaudi::Property<std::string> m_inputTruthParticleCollKey{this, "InputTruthParticleCollKey", "TruthPileupParticles", ""};
  Gaudi::Property<std::string> m_inTimeOutputTruthParticleCollKey{this, "InTimeOutputTruthParticleCollKey", "TruthPileupParticles", ""};
  Gaudi::Property<bool> m_includeSignalTruthParticles{this, "IncludeSignalTruthParticles", false, ""};
  xAOD::TruthParticleContainer *m_inTimeOutputTruthParticleContainer{};
  bool m_first_event{true};
};
#endif //MCTRUTHSIMALGS_MERGETRUTHPARTICLESTOOL_H
