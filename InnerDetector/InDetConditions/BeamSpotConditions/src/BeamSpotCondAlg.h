/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef BEAMSPOT_CONDITIONSALGS_BEAMSPOTCONDALG_H
#define BEAMSPOT_CONDITIONSALGS_BEAMSPOTCONDALG_H 1

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "GaudiKernel/EventIDRange.h"
#include <limits>

#include "BeamSpotConditionsData/BeamSpotData.h"

class BeamSpotCondAlg : public AthReentrantAlgorithm 
{ 
 public: 
  BeamSpotCondAlg( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~BeamSpotCondAlg(); 

                                        //IS EXECUTED:
  virtual StatusCode  initialize() override;     //once, before any input is loaded
  virtual StatusCode execute(const EventContext& ctx) const override;     //per event
  
  static const EventIDRange alwaysValid;

 private: 

  SG::ReadCondHandleKey<AthenaAttributeList> m_readKey { this, "BeamSpotFolder", "/Indet/Beampos", 
                                                         "DB folder from which to read raw beam spot data" };

  SG::WriteCondHandleKey<InDet::BeamSpotData> m_writeKey { this, "BeamSpotDataKey", "BeamSpotData",
                                                           "Key for derived conditions in conditions store" };

  Gaudi::Property<bool>  m_useDB   { this, "useDB",   true,  "read beam spot from conditions DB" };
  Gaudi::Property<int>   m_status  { this, "status",  1,     "default status" };
  Gaudi::Property<float> m_posX    { this, "posX",    0.0f,  "default X position" };
  Gaudi::Property<float> m_posY    { this, "posY",    0.0f,  "default Y position" };
  Gaudi::Property<float> m_posZ    { this, "posZ",    0.0f,  "default Z position" };
  Gaudi::Property<float> m_sigmaX  { this, "sigmaX",  0.15f, "default X width" };
  Gaudi::Property<float> m_sigmaY  { this, "sigmaY",  0.15f, "default Y width" };
  Gaudi::Property<float> m_sigmaZ  { this, "sigmaZ",  53.0f, "default Z width" };
  Gaudi::Property<float> m_tiltX   { this, "tiltX",   0.0f,  "default X tilt" };
  Gaudi::Property<float> m_tiltY   { this, "tiltY",   0.0f,  "default Y tilt" };
  Gaudi::Property<float> m_sigmaXY { this, "sigmaXY", 0.0f,  "default XY width" };

}; 

#endif //> !BEAMSPOT_CONDITIONSALGS_BEAMSPOTCONDALG_H
