/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGCALOHYPO_TRIGLARNOISEBURSTALGOHYPOTOOLINC_H
#define TRIGCALOHYPO_TRIGLARNOISEBURSTALGOHYPOTOOLINC_H 1

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigCaloHypo/ITrigLArNoiseBurstHypoTool.h"
#include "CaloInterface/ILArNoisyROTool.h"

/**
 * @class Implementation of the CaloCell Noise Burst selection 
 * @brief 
 **/

class TrigLArNoiseBurstHypoToolInc : public extends<AthAlgTool, ITrigLArNoiseBurstHypoTool> { 
 public: 
  TrigLArNoiseBurstHypoToolInc( const std::string& type, 
             const std::string& name, 
             const IInterface* parent );

  virtual StatusCode initialize() override;

  virtual StatusCode decide( std::vector<ITrigLArNoiseBurstHypoTool::CaloCellNoiseInfo>& input )  const override;

  virtual bool decide( const ITrigLArNoiseBurstHypoTool::CaloCellNoiseInfo& i ) const override;


 private:
  ToolHandle<ILArNoisyROTool> m_noisyROTool;
  HLT::Identifier m_decisionId;

  Gaudi::Property< bool >  m_badFEBFlaggedPartitions         { this, "BadFEBFlaggedPartitions" , true, "flag to be used for NB detection" };
  Gaudi::Property< bool >  m_satTightFlaggedPartitions      { this, "SatTightFlaggedPartitions", true, "flag to be used for NB detection" };
  Gaudi::Property< bool >  m_mNBLooseFlaggedPartitions      { this, "MNBLooseFlaggedPartitions", true, "flag to be used for NB detection" };
  Gaudi::Property< bool >  m_mNBTightFlaggedPartitions      { this, "MNBTightFlaggedPartitions", true, "flag to be used for NB detection" };
  Gaudi::Property< bool >  m_mNBTight_PsVetoFlaggedPartitions{ this, "MNBTight_PsVetoFlaggedPartitions", true, "flag to be used for NB detection" };

  //ToolHandle< GenericMonitoringTool > m_monTool { this, "MonTool", "", "Monitoring tool" };
  unsigned int m_mask;
  
}; 

#endif //> !TRIGCALOHYPO_TRIGLARNOISEBURSTHYPOTOOLINC_H
