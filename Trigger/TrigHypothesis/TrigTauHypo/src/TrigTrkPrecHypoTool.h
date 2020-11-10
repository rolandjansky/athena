/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

  Gaudi::Property<float>  m_lowerPtCut{ this, "lowerPtCut", 20000.0, "" };
  Gaudi::Property<float>  m_lowerTrackPtCut{ this, "lowerTrackPtCut", 0.0, "" };
  Gaudi::Property<float>  m_clusterCone{ this, "clusterCone", 0.2, "" };
  Gaudi::Property<float>  m_coreSize{ this, "coreSize", 0.2, "" };
  Gaudi::Property<float>  m_outerSize{ this, "outerSize", 0.4 , "" };
  Gaudi::Property<float>  m_deltaRLeadTrkRoI{ this, "deltaRLeadTrkRoI", 0.2 , "" };
  Gaudi::Property<float>  m_deltaZ0Cut{ this, "deltaZ0Cut", 2., "" };

  Gaudi::Property<int>  m_tracksInCoreCut{ this, "tracksInCoreCut", 3, "" };
  Gaudi::Property<int>  m_tracksInIsoCut{ this, "tracksInIsoCut", 1, "" };

  Gaudi::Property<bool>  m_rejectNoTracks{ this, "rejectNoTracks", false, "" };
  Gaudi::Property<bool>  m_relax_highpt{ this, "relax_highpt", true, "" };

  Gaudi::Property<double>  m_highpt_threshold{ this, "highpt_threshold", 200000., "" };

  Gaudi::Property<bool>  m_acceptAll{ this, "AcceptAll", false, "Ignore selection" };

  ToolHandle<GenericMonitoringTool> m_monTool{ this, "MonTool", "", "Monitoring tool" };

};
#endif

