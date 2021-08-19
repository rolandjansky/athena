/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef HLTSEEDING_PRESCALINGTOOL_H
#define HLTSEEDING_PRESCALINGTOOL_H

// HLTSeeding includes
#include "IPrescalingTool.h"

// framework includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "AthenaKernel/RNGWrapper.h"
#include "AthenaMonitoringKernel/Monitored.h"

// data includes
#include "TrigConfData/HLTPrescalesSet.h"
#include "TrigConfData/HLTMenu.h"
#include "xAODEventInfo/EventInfo.h"

// STL includes
#include <string>

/**
 * @class Implementation of prescaling tool for the HLT use
 * @brief Uses the HLTPrescalesSet to configure the prescaling
 *
 * The HLTPrescalesSet can change within the job, therefore the set is accessed from DetectorStore for each event.
 * The seed of the RNG is reset at every event and thus decisions are reproducible.
 **/
class PrescalingTool : public extends<AthAlgTool, IPrescalingTool> {
 public:

   // no default constructor
   PrescalingTool() = delete;

   // constructor
   PrescalingTool( const std::string& type, const std::string& name, const IInterface* parent );

   virtual StatusCode initialize() override;
   virtual StatusCode start() override;

   /** @brief The method to prescale chains
    *
    * If the prescale set input key is not defined or the chain does not exist in the prescale set,
    * then the chains (or all chains) are kept according to the property KeepUnknownChains
    *
    * @param[in] ctx event context is needed for accessing the random engine
    * @param[in] initiallyActive vector of HLTIdentifiers of initially active chains (because they were seeded by L1)
    * @param[out] remainActive vector that is being filled with HLTIdentifiers of chains surviving the prescaling
    */
   virtual StatusCode prescaleChains( const EventContext& ctx,  const HLT::IDVec& initiallyActive,
                                      HLT::IDVec& remainActive ) const override;

 private:

   SG::ReadCondHandleKey<TrigConf::HLTPrescalesSet> m_hltPrescaleSetInputKey{
    this, "HLTPrescales", "HLTPrescales", "HLT prescales set"};

   SG::ReadHandleKey<TrigConf::HLTMenu> m_HLTMenuKey{
    this, "HLTTriggerMenu", "DetectorStore+HLTTriggerMenu", "HLT Menu"};

   SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey{
    this, "EventInfo", "EventInfo", "Event Info Object Key"};

   Gaudi::Property<bool> m_keepUnknownChains{
    this, "KeepUnknownChains", false, "If True then chains for which prescaling information is not set are kept"};

   ToolHandle<GenericMonitoringTool> m_monTool{
     this, "MonTool", "", "Monitoring tool"};

   /// Random engine for calculating prescales
   ATHRNG::RNGWrapper m_RNGEngines;

   using ChainSet = std::set<HLT::Identifier>;
   ChainSet m_nonCPSChains;
   std::map<std::string, HLT::IDVec> m_CPSGroups;

   TrigConf::HLTPrescalesSet::HLTPrescale m_prescaleForUnknownChain;
   HLT::Identifier m_costChainID{""};
};


#endif
