/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGCALOHYPO_TRIGLARNOISEBURSTHYPOALG
#define TRIGCALOHYPO_TRIGLARNOISEBURSTHYPOALG

#include <string>
#include <mutex>

#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaKernel/SlotSpecificObj.h"
#include "CaloEvent/CaloCellContainer.h"
#include "LArRecConditions/LArBadChannelCont.h"
#include "CaloInterface/ILArNoisyROTool.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"

namespace hltinterface{
  class GenericHLTContainer;
}


/**
 * @class TrigLArNoiseBurstRecoAlg
 * @brief Implements LArNoiseBurst detection for the new HLT framework
 **/
class TrigLArNoiseBurstRecoAlg : public AthReentrantAlgorithm {
 public: 

  TrigLArNoiseBurstRecoAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual StatusCode  initialize() override;
  virtual StatusCode  execute( const EventContext& context ) const override;

 private: 
  SG::ReadHandleKey<CaloCellContainer > m_cellContainerKey { this, "CellContainerKey", "CellsClusters","SG Key of cells"};
  SG::ReadCondHandleKey<LArBadFebCont> m_knownBadFEBsVecKey {this, "BadFEBsKey", "LArKnownBadFEBs", "key to read the known Bad FEBs"};
  SG::ReadCondHandleKey<LArBadFebCont> m_knownMNBFEBsVecKey {this, "MNBFEBsKey", "LArKnownMNBFEBs", "key to read the known MNB FEBs"};
  SG::WriteHandleKey<bool> m_noiseBurstDetected{this, "NoiseBurstKey", "HLT_NoiseBurst", "Flag which records if a noise burst was detected in the event"};

  ToolHandle<ILArNoisyROTool> m_noisyROTool {this, "Tool", "LArNoisyROTool", "Noise Burst Calculation Tool"};

  Gaudi::Property< bool >  m_badFEBFlaggedPartitions         { this, "BadFEBFlaggedPartitions" , true, "flag to be used for NB detection" };
  Gaudi::Property< bool >  m_satTightFlaggedPartitions      { this, "SatTightFlaggedPartitions", true, "flag to be used for NB detection" };
  Gaudi::Property< bool >  m_mNBLooseFlaggedPartitions      { this, "MNBLooseFlaggedPartitions", true, "flag to be used for NB detection" };
  Gaudi::Property< bool >  m_mNBTightFlaggedPartitions      { this, "MNBTightFlaggedPartitions", true, "flag to be used for NB detection" };
  Gaudi::Property< bool >  m_mNBTight_PsVetoFlaggedPartitions{ this, "MNBTight_PsVetoFlaggedPartitions", true, "flag to be used for NB detection" };

  ToolHandle< GenericMonitoringTool > m_monTool { this, "MonTool", "", "Monitoring tool" };
  unsigned int m_mask;

  // lock for IS publishing
  mutable std::mutex m_pubIS_mtx;
  std::shared_ptr<hltinterface::GenericHLTContainer> m_IsObject;

  long int m_timeTagPosToClear;
  long int m_publishTime;
  std::string m_name_of_is;
  size_t   m_evntPos;
  size_t   m_timeTagPos;
  size_t   m_timeTagPosns;
  mutable long int m_timeTagPosRec ATLAS_THREAD_SAFE;
}; 
#endif
