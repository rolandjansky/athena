/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigTrkPrecHypoTool_H
#define TrigTrkPrecHypoTool_H

#include "Gaudi/Property.h"
#include "xAODTau/TauJet.h"
#include "xAODTau/TauJetContainer.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"

#include "ITrigTrkPrecHypoTool.h"

class StoreGateSvc;

namespace HLT {
  class TriggerElement;
}

class TrigTrkPrecHypoTool : public extends<AthAlgTool, ITrigTrkPrecHypoTool> {
 public:
  TrigTrkPrecHypoTool( const std::string& type, 
           const std::string& name, 
           const IInterface* parent );

  virtual ~TrigTrkPrecHypoTool();

  virtual StatusCode initialize() override;

  virtual StatusCode decide( std::vector<ITrigTrkPrecHypoTool::TrackingInfo>& input )  const override;

  virtual bool decide( const ITrigTrkPrecHypoTool::TrackingInfo& i ) const override;

 private:

  HLT::Identifier m_decisionId;

  ToolHandle<GenericMonitoringTool> m_monTool{ this, "MonTool", "", "Monitoring tool" };

};
#endif

