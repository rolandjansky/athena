#ifndef BEAMSPOT_CONDITIONSALGS_BEAMSPOTCONDALG_H
#define BEAMSPOT_CONDITIONSALGS_BEAMSPOTCONDALG_H 1

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "GaudiKernel/ICondSvc.h"

#include "BeamSpotConditionsData/BeamSpotData.h"

class BeamSpotCondAlg : public AthAlgorithm 
{ 
 public: 
  BeamSpotCondAlg( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~BeamSpotCondAlg(); 

                                        //IS EXECUTED:
  virtual StatusCode  initialize();     //once, before any input is loaded
  virtual StatusCode  execute();        //per event
  virtual StatusCode  finalize();       //once, after all events processed
  

 private: 

  SG::ReadCondHandleKey<AthenaAttributeList> m_readKey { this, "BeamSpotFolder", "/Indet/Beampos", 
                                                         "DB folder from which to read raw beam spot data" };

  SG::WriteCondHandleKey<InDet::BeamSpotData> m_writeKey { this, "BeamSpotDataKey", "BeamSpotData",
                                                           "Key for derived conditions in conditions store" };

  ServiceHandle<ICondSvc> m_condSvc { this, "ConditionsService", "CondSvc", "name of conditions service" };

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
