/*                                                                                                                                                                                                          
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration                                                                                                                                   
*/

/********************************************************************
 *
 * NAME:     TrigEFTauDiKaonHypoTool.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigTauHypo
 *
 * AUTHOR:   J. Silva based on TrigEFTauMVHypoTool
 * CREATED:  Feb. 17, 2021
 *
  *********************************************************************/
#ifndef TRIGEFTAUDIKAONHYPOTOOL_H
#define TRIGEFTAUDIKAONHYPOTOOL_H

#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "ITrigEFTauMVHypoTool.h"

class StoreGateSvc;
namespace HLT {
  class TriggerElement;
}

class TrigEFTauDiKaonHypoTool : public extends<AthAlgTool, ITrigEFTauMVHypoTool> {

 public:

  TrigEFTauDiKaonHypoTool( const std::string& type,
		       const std::string& name,
		       const IInterface* parent );

  virtual ~TrigEFTauDiKaonHypoTool();

  virtual StatusCode initialize() override;

  virtual StatusCode decide( std::vector<ITrigEFTauMVHypoTool::TauJetInfo>& input )  const override;

  virtual bool decide( const ITrigEFTauMVHypoTool::TauJetInfo& i ) const override;


 private:

  /** Cuts to be applied */

  HLT::Identifier m_decisionId;

  Gaudi::Property<float>  m_massTrkSysMin{ this, "massTrkSysMin", 0., "Minimum DiTrack mass value" };
  Gaudi::Property<float>  m_massTrkSysMax{ this, "massTrkSysMax", 1000000000., "Maximum DiTrack mass value" };
  Gaudi::Property<float>  m_massTrkSysKaonMin{ this, "massTrkSysKaonMin", 0., "Minimum DiKaon mass value" };
  Gaudi::Property<float>  m_massTrkSysKaonMax{ this, "massTrkSysKaonMax", 1000000000., "Maximum DiKaon mass value" };
  Gaudi::Property<float>  m_massTrkSysKaonPiMin{ this, "massTrkSysKaonPiMin", 0., "Minimum KaonPi mass value" };
  Gaudi::Property<float>  m_massTrkSysKaonPiMax{ this, "massTrkSysKaonPiMax", 1000000000., "Maximum KaonPi mass value" };
  Gaudi::Property<float>  m_targetMassTrkSysKaonPi{ this, "targetMassTrkSysKaonPi", 0., "Target KaonPi mass value" };
  Gaudi::Property<float>  m_EtCalibMin{ this, "EtCalibMin", 0., "Minimum Et value" };
  Gaudi::Property<float>  m_leadTrkPtMin{ this, "leadTrkPtMin", 0., "Minimum Pt of Lead Track" };
  Gaudi::Property<int>  m_nTrackMax{ this, "nTrackMax", 2, "Maximum number of Tracks" };
  Gaudi::Property<int>  m_nTrackMin{ this, "nTrackMin", 1, "Minimum number of Tracks" };
  Gaudi::Property<int>  m_nWideTrackMax{ this, "nWideTrackMax", 1, "Maximum number of wide Tracks" };
  Gaudi::Property<float>  m_EMPOverTrkSysPMax{ this, "EMPOverTrkSysPMax", 5., "Maximum Cluster pt over ditrack pt" };
  Gaudi::Property<float>  m_dRmaxMax{ this, "dRmaxMax", 10., "Maximum dRmax value" };
  Gaudi::Property<float>  m_etOverPtLeadTrkMax{ this, "etOverPtLeadTrkMax", 10., "Maximum et/pt(lead track)" };
  Gaudi::Property<float>  m_etOverPtLeadTrkMin{ this, "etOverPtLeadTrkMin", 0., "Minimum et/pt(lead track)" };
  Gaudi::Property<bool>  m_acceptAll{ this, "AcceptAll", false, "Ignore selection" };

  ToolHandle<GenericMonitoringTool> m_monTool{ this, "MonTool", "", "Monitoring tool" };

};
#endif

