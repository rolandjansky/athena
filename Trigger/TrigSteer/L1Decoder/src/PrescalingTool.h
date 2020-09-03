/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef L1DECODER_PRESCALINGTOOL_H 
#define L1DECODER_PRESCALINGTOOL_H

// L1Decoder includes
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
#include "xAODEventInfo/EventInfo.h"

// STL includes
#include <string>



/**
 * @class Implementation of prescaling tool for the offline use
 * @brief Uses the HLTPrescaleSet to configure the prescaling.
 * The HLTPrescalingSet can change within the job, therefor the set is accessed from DetectorStore for each event
 * The seed of the RNG is reset at every event and thus decisions are reproducible.
 **/

class PrescalingTool : public extends<AthAlgTool, IPrescalingTool> {
 public:

   // no default constructor
   PrescalingTool() = delete;

   // constructor
   PrescalingTool( const std::string& type, const std::string& name, const IInterface* parent );

   // destructor
   virtual ~PrescalingTool();

   virtual StatusCode initialize() override;

   /** @brief method to prescale chains
    * If the prescale set input key is not defined or the chain does not exist in the prescale set, then the chains (or all chains) are
    * kept according to the property KeepUnknownChains
    *
    * @param[in] ctx event context is needed for accessing the random engine
    * @param[in] initialyActive vector of HLTIdentifiers of initially active chains (because they were seeded by L1)
    * @param[out] remainActive vector that is being filled with HLTIdentifiers of chains surviving the prescaling
    */
   virtual StatusCode prescaleChains( const EventContext& ctx,  const HLT::IDVec& initialyActive,
                                      HLT::IDVec& remainActive ) const override;  
 private: 

   // input data
   SG::ReadCondHandleKey<TrigConf::HLTPrescalesSet> m_hltPrescaleSetInputKey{ this, "HLTPrescales", "HLTPrescales", "HLT prescales set"};
   SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey{ this, "EventInfo", "EventInfo", "Event Info Object Key"};

   // properties
   Gaudi::Property<bool> m_keepUnknownChains{ this, "KeepUnknownChains", true, "If True then chains for which prescaling information is not set are kept" };
   ToolHandle<GenericMonitoringTool> m_monTool{this,"MonTool","","Monitoring tool"};

   // random engine for calculating prescales
   ATHRNG::RNGWrapper m_RNGEngines;

}; 


#endif 
