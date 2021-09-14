/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_ITrigBphysState_H
#define TRIG_ITrigBphysState_H

#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTrigBphys/TrigBphysContainer.h"
#include "BeamSpotConditionsData/BeamSpotData.h"


class EventContext;


/**
 * @class ITrigBphysState
 * @brief Base class for TrigBphys state objects
 */

class ITrigBphysState {
 public:
  ITrigBphysState() = delete;
  ITrigBphysState(const EventContext& context,
                  const TrigCompositeUtils::DecisionContainer& previousDecisions,
                  TrigCompositeUtils::DecisionContainer& decisions,
                  xAOD::TrigBphysContainer* trigBphysCollection = nullptr,
                  const InDet::BeamSpotData* beamSpotData = nullptr)
      : m_context(&context),
        m_previousDecisions(&previousDecisions),
        m_decisions(&decisions),
        m_trigBphysCollection(trigBphysCollection),
        m_beamSpotData(beamSpotData) {}
  virtual ~ITrigBphysState() = default;

  inline const EventContext& context() const { return *m_context; }
  inline const TrigCompositeUtils::DecisionContainer& previousDecisions() const { return *m_previousDecisions; }
  inline TrigCompositeUtils::DecisionContainer& decisions() { return *m_decisions; }
  inline xAOD::TrigBphysContainer& trigBphysCollection() { return *m_trigBphysCollection; }
  void setTrigBphysCollection(xAOD::TrigBphysContainer* trigBphysCollection) { m_trigBphysCollection = trigBphysCollection; }
  Amg::Vector3D beamSpotPosition() const { return (m_beamSpotData ? m_beamSpotData->beamPos() : Amg::Vector3D::Zero(3)); }

 private:
  const EventContext* m_context;
  const TrigCompositeUtils::DecisionContainer* m_previousDecisions;
  TrigCompositeUtils::DecisionContainer* m_decisions;
  xAOD::TrigBphysContainer* m_trigBphysCollection;
  const InDet::BeamSpotData* m_beamSpotData;
};

#endif  // TRIG_ITrigBphysState_H
