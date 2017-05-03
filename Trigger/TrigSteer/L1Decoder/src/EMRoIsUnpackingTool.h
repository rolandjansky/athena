#ifndef L1DECODER_EMROISUNPACKINGTOOL_H
#define L1DECODER_EMROISUNPACKINGTOOL_H 1

// STL includes
#include <string>

#include "TrigConfInterfaces/ILVL1ConfigSvc.h"
#include "TrigConfL1Data/ThresholdConfig.h"
#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigT1Interfaces/RecEmTauRoI.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

// L1Decoder includes
#include "./IRoIsUnpackingTool.h"


class EMRoIsUnpackingTool : virtual public AthAlgTool, virtual public IRoIsUnpackingTool { 


 public: 
  EMRoIsUnpackingTool( const std::string& type,
		       const std::string& name, 
		       const IInterface* parent );

  virtual ~EMRoIsUnpackingTool(); 

  virtual StatusCode unpack(const EventContext& ctx,
			    const ROIB::RoIBResult& roib,
			    const HLT::IDVec& activeChains) const;
  
  // Athena algtool's Hooks
  virtual StatusCode  initialize();
  virtual StatusCode  beginRun();
  virtual StatusCode  finalize();
  
 private: 
  EMRoIsUnpackingTool();
  std::vector<TrigConf::TriggerThreshold*> m_emThresholds;
  SG::WriteHandleKey< TrigRoiDescriptorCollection > m_trigRoIsKey;
  SG::WriteHandleKey< DataVector<LVL1::RecEmTauRoI> > m_recEMTauRoIsKey;  
  ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc;
  float m_roIWidth;
}; 

#endif //> !L1DECODER_EMROISUNPACKINGTOOL_H
