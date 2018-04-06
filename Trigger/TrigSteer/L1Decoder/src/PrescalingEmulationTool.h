/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef L1DECODER_PRESCALINGEMULATIONTOOL_H 
#define L1DECODER_PRESCALINGEMULATIONTOOL_H

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "xAODEventInfo/EventInfo.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "AthenaKernel/RNGWrapper.h"
// L1Decoder includes
#include "./IPrescalingTool.h"



/**
 * @class Implementation of prescaling tool for the offline use
 * @brief Uses property of this tool to configure the prescaling.
 * The prescaling info is constant over  the whole job. 
 * For running online, where prescaling changes according to conditions another implementation shall be used.
 * Prescaling decision is a function of the chain ID only. The seed of the RNG is reset at every event and thus decisions are reproducible.
 **/

class PrescalingEmulationTool : public extends<AthAlgTool, IPrescalingTool> { 
public:
  PrescalingEmulationTool( const std::string& type,
			   const std::string& name, 
			   const IInterface* parent );

  virtual ~PrescalingEmulationTool(); 

  virtual StatusCode  initialize() override;
  virtual StatusCode  finalize() override;

  virtual StatusCode prescaleChains( const EventContext& ctx,  
				     const HLT::IDVec& initialyActive,
				     HLT::IDVec& remainActive ) const override;  
 private: 

  PrescalingEmulationTool();  
  //ServiceHandle<IAtRndmGenSvc> m_RNGSvc{ this, "RNGSvc", "AtRanluxGenSvc/AtRanluxGenSvc", "RNG Factory Service" };
  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfo{ this, "EventInfo", "EventInfo", "EventInfo object, source of CTP time used to reseed the RNG" };
  Gaudi::Property<bool> m_keepUnknownChains{ this, "KeepUnknownChains", true, "If True then chains for which prescaling information is not set are kept" }; 
  Gaudi::Property< std::vector<std::string> > m_prescalingConfig{ this, "Prescales", {}, "The prescaling info in the form: \"chainName:PSValue\"" }; 
  ATHRNG::RNGWrapper m_RNGEngines;
  
  typedef std::map<HLT::Identifier, float> PrescalingInfo;
  PrescalingInfo m_prescalingInfo;
}; 


#endif 
