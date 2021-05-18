/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TrigEgammaForwardPrecisionElectronHypoTool_h
#define TrigEgammaForwardPrecisionElectronHypoTool_h

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "ITrigEgammaForwardPrecisionElectronHypoTool.h"
#include "StoreGate/ReadDecorHandle.h"
#include "xAODEventInfo/EventInfo.h"

/**
 * @class Implementation of the precision selection for electrons
 * @brief 
 **/

class TrigEgammaForwardPrecisionElectronHypoTool : public extends<AthAlgTool, ITrigEgammaForwardPrecisionElectronHypoTool> { 
 public: 
  TrigEgammaForwardPrecisionElectronHypoTool( const std::string& type, 
        const std::string& name, 
        const IInterface* parent );

  virtual StatusCode initialize() override;

  virtual StatusCode decide( std::vector<ITrigEgammaForwardPrecisionElectronHypoTool::ElectronInfo>& input)  const override;
  virtual bool decide( const ITrigEgammaForwardPrecisionElectronHypoTool::ElectronInfo& i) const override;

 private:
  HLT::Identifier m_decisionId;
  
  Gaudi::Property< std::vector<float> > m_etabin { this, "EtaBins", {} , "Bins of eta" }; //!<  selection variable for PRECISION electron selection:eta bins
  Gaudi::Property<std::string>          m_pidName{this,"PidName", "", "Pid name"};
  ToolHandle< GenericMonitoringTool >   m_monTool { this, "MonTool", "", "Monitoring tool" };


  int findCutIndex( float eta ) const;

}; 

#endif 
