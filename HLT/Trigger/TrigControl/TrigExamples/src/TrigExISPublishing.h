/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEXPARTIALEB_TRIGEXISPUBLISHING_H
#define TRIGEXPARTIALEB_TRIGEXISPUBLISHING_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

namespace hltinterface{
  class GenericHLTContainer;
}

/**
 * IS Publishing test algorithm
 *
 * Demonstrate IS publishing using the LAr noise burst schema.
 **/
class TrigExISPublishing : public AthReentrantAlgorithm {
public:
  TrigExISPublishing(const std::string& name, ISvcLocator* svcLoc);

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;

private:
  std::shared_ptr<hltinterface::GenericHLTContainer> m_IsObject;

  size_t   m_evntPos{0};
  size_t   m_timeTagPos{0};
};

#endif
