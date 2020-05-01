/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEFTAUMVHYPOTOOL_H
#define TRIGEFTAUMVHYPOTOOL_H

#include "TrigInterfaces/HypoAlgo.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "ITrigEFTauMVHypoTool.h"

class StoreGateSvc;

namespace HLT {
  class TriggerElement;
}

class TrigEFTauMVHypoTool : public extends<AthAlgTool, ITrigEFTauMVHypoTool> {
 public:
  TrigEFTauMVHypoTool( const std::string& type, 
           const std::string& name, 
           const IInterface* parent );

  virtual ~TrigEFTauMVHypoTool();

  virtual StatusCode initialize() override;

  virtual StatusCode decide( std::vector<ITrigEFTauMVHypoTool::TauJetInfo>& input )  const override;

  virtual bool decide( const ITrigEFTauMVHypoTool::TauJetInfo& i ) const override;

 private:

  HLT::Identifier m_decisionId;

  Gaudi::Property<int>  m_numTrackMin{ this, "numTrackMin", -999, "Minimum number of tracks" };
  Gaudi::Property<int>  m_numTrackMax{ this, "numTrackMax", 5, "Maximum number of tracks" };
  Gaudi::Property<float>  m_numWideTrackMax{ this, "numTrackWideTrackMax",999, "Maximum number of wide tracks" };

  Gaudi::Property<double>  m_EtCalibMin{ this, "EtCalibMin", -10000., "pT Minimum cut" };
  Gaudi::Property<int>  m_level{ this, "level", -1, "Select ID Working point" };
  Gaudi::Property<int>  m_method{ this, "method", 0, "Select ID method" };
  Gaudi::Property<bool>  m_highpt{ this, "highpt", true , "Turn on/off high pt selection" };
  Gaudi::Property<double>  m_highpttrkthr{ this, "highpttrkthr", 200000. , "Track threshold for high pt taus" };
  Gaudi::Property<double>  m_highptidthr{ this, "highptidthr", 330000., "pT threshold for loosening ID level cut" };
  Gaudi::Property<double>  m_highptjetthr{ this, "highptjetthr", 440000., "pT threshold for disabling ID level cut" };

  Gaudi::Property<bool>  m_applyIDon0p{ this, "applyIDon0p", true, "Apply Id on 0-prong taus" };
  Gaudi::Property<int>  m_numTrack{ this, "numTrack", -100, "Tau number of tracks" };
  Gaudi::Property<int>  m_numWideTrack{ this, "numWideTrack", 1, "Tau number of wide tracks" };

  Gaudi::Property<bool>  m_acceptAll{ this, "AcceptAll", false, "Ignore selection" };

  ToolHandle<GenericMonitoringTool> m_monTool{ this, "MonTool", "", "Monitoring tool" };


};
#endif

