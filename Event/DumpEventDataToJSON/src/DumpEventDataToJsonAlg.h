/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DUMPEVENTDATATOJSONALG_H
#define DUMPEVENTDATATOJSONALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODJet/JetContainer.h"
#include "StoreGate/ReadHandleKey.h"
#include "TrkExInterfaces/IExtrapolationEngine.h"
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <map>

class StoreGateSvc;

/** Algorithm to dump some Event data to JSON. 
*/
class DumpEventDataToJsonAlg : public AthAlgorithm
{
 public:
  
  /// Algorithm constructor
  DumpEventDataToJsonAlg(const std::string& name, ISvcLocator* pService);

  virtual ~DumpEventDataToJsonAlg();

  /// inherited from Algorithm
  virtual StatusCode initialize() override;
  virtual StatusCode execute () override;
  virtual StatusCode finalize() override;

 protected:
   
   template <class TYPE> 
   StatusCode getAndFillArrayOfContainers(nlohmann::json& event, const SG::ReadHandleKeyArray< TYPE >& keys, const std::string jsonType);
   
   template <class TYPE> 
   nlohmann::json getData( const TYPE& object);
   
   SG::ReadHandleKey<xAOD::EventInfo>                     m_eventInfoKey
   { this, "EventInfoKey", "EventInfo", "Key for the Event Info" };
   
   SG::ReadHandleKeyArray<xAOD::TrackParticleContainer>   m_trackParticleKeys
   { this, "TrackParticleContainerKeys", {"InDetTrackParticles", "CombinedMuonTrackParticles" }, "Keys for TrackParticle Containers" };
   
   SG::ReadHandleKeyArray<xAOD::JetContainer>             m_jetKeys
   { this, "JetContainerKeys", {"AntiKt4EMTopoJets"}, "Keys for Jet Containers" };
   
   std::string                                            m_outputJSON_Name   {"EventData.json"};
   nlohmann::json                                         m_eventData; 
   
   Gaudi::Property<bool>                                  m_extrapolateTracks 
     {this, "ExtrapolateTracks", false, "If true, attempt to extrapolate tracks and add additional positions."};
   ToolHandle<Trk::IExtrapolationEngine>                  m_extrapolator { this, "Extrapolator","Trk::ExtrapolationEngine/AtlasExtrapolation"};
};
#endif
