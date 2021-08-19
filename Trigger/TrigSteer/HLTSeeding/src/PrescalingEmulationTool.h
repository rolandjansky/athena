/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef HLTSEEDING_PRESCALINGEMULATIONTOOL_H
#define HLTSEEDING_PRESCALINGEMULATIONTOOL_H

// HLTSeeding includes
#include "IPrescalingTool.h"

// Framework includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "xAODEventInfo/EventInfo.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "AthenaKernel/RNGWrapper.h"

// STL includes
#include <string>

/**
 * @class Implementation of prescaling tool for the offline use
 * @brief Uses property of this tool to configure the prescaling.
 * 
 * The prescaling info is constant over the whole job.
 * For running online, where prescaling changes according to conditions the @c PrescalingTool shall be used.
 * Prescaling decision is a function of the chain ID only. The seed of the RNG is reset at every event and thus
 * decisions are reproducible.
 **/

class PrescalingEmulationTool : public extends<AthAlgTool, IPrescalingTool> {
public:
  PrescalingEmulationTool( const std::string& type,
                           const std::string& name,
                           const IInterface* parent );

  virtual StatusCode initialize() override;

  virtual StatusCode prescaleChains( const EventContext& ctx,
                                     const HLT::IDVec& initiallyActive,
                                     HLT::IDVec& remainActive ) const override;

 private:
  PrescalingEmulationTool();

  Gaudi::Property<bool> m_keepUnknownChains{
    this, "KeepUnknownChains", true, "If True then chains for which prescaling information is not set are kept"};

  Gaudi::Property< std::vector<std::string> > m_prescalingConfig{
    this, "Prescales", {}, "The prescaling info in the form: \"chainName:PSValue\""};

  ATHRNG::RNGWrapper m_RNGEngines;

  typedef std::map<HLT::Identifier, float> PrescalingInfo;
  PrescalingInfo m_prescalingInfo;
};

#endif
