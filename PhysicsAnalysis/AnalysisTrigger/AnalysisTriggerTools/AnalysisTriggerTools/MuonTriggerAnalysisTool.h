/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANALYSISTRIGGERTOOLS_MUONTRIGGERANALYSISTOOL_H
#define ANALYSISTRIGGERTOOLS_MUONTRIGGERANALYSISTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrigDecisionEvent/TrigDecision.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigSteering/Lvl1ResultAccessTool.h"
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "TrigT1Interfaces/RecMuonRoiSvc.h"

#include "muonEvent/MuonContainer.h"

#include <string>
#include <map>
#include <fstream>

class StoreGateSvc;
class IAnalysisTools;
class TruthParticle;
//class Trig::ChainGroup;

namespace LVL1 {
  class RecMuonRoiSvc;
}

class MuonTriggerAnalysisTool : public AthAlgTool  {

 public:

  MuonTriggerAnalysisTool( const std::string& type, const std::string& name,
			   const IInterface* parent );

  virtual ~MuonTriggerAnalysisTool();

  static const InterfaceID& interfaceID( );
  
  virtual StatusCode initialize();
  virtual StatusCode finalize();

  // for mctpi patch
  float muctpiEta_min[64][30];
  float muctpiEta_max[64][30];
  float muctpiPhi_min[64][30];
  float muctpiPhi_max[64][30];

  /** Get functions */
  bool checkMuon(const TruthParticle* truthParticle, const std::string chain);
  bool checkMuon(const Rec::TrackParticle* particle, const std::string chain);
  bool checkMuon(const Trk::Track* track, const std::string chain);

  bool checkMuon(const TruthParticle* truthParticle, const std::string chain, double& deltaR);
  bool checkMuon(const Rec::TrackParticle* particle, const std::string chain, double& deltaR);
  bool checkMuon(const Rec::TrackParticle* particle, const std::string chain, double& deltaR, std::map<std::string, double>& output);
  bool checkMuon(const Trk::Track* track, const std::string chain, double& deltaR);

  /** Special functions to cross-check standalone muons, for the moment use offline */
  bool checkMuonStandalone(const Rec::TrackParticle* particle, const std::string chain);

  bool dumpLvl1();
  
 private:

  /// Get a handle on Store Gate 
  StoreGateSvc* m_storeGate;
  /// get a handle on the analysis tool helper
  IAnalysisTools * m_analysisTools;

  // handle on the TriggerDecisionTool
  ToolHandle<Trig::TrigDecisionTool> m_trigDec;

  ToolHandle<HLT::Lvl1ResultAccessTool> m_lvl1Tool;

  ServiceHandle<LVL1::RecMuonRoiSvc> m_rpcRoiService; 
  ServiceHandle<LVL1::RecMuonRoiSvc> m_tgcRoiService; 

  // matching functions
  bool muonLvl1Match(const I4Momentum* particle, const std::string chain, double& deltaR, std::map<std::string, double>& output);
  bool muonLvl2Match(const I4Momentum* particle, const std::string chain, double& deltaR);
  bool muonEFMatch(const I4Momentum* particle, const std::string chain, double& deltaR);

  const Trig::ChainGroup* m_all;
  const Trig::ChainGroup* m_allL1;
  const Trig::ChainGroup* m_allL2;
  const Trig::ChainGroup* m_allEF;

  bool m_isInOneBC;
  bool m_isInThreeBC;

  // deltaR cuts for defining the match
  double m_muonLvl1DRMatchBarrel;
  double m_muonLvl2DRMatchBarrel;
  double m_muonEFDRMatchBarrel;
  
  double m_muonLvl1DRMatchEndcap;
  double m_muonLvl2DRMatchEndcap;
  double m_muonEFDRMatchEndcap;

};


#endif  // ANALYSISTRIGGERTOOLS_MUONTRIGGERANALYSISTOOL_H
