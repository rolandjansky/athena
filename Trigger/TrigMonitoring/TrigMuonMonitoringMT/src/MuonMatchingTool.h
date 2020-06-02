/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONMONITORINGMT_MUONMATCHINGTOOL_H
#define TRIGMUONMONITORINGMT_MUONMATCHINGTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTrigMuon/L2StandAloneMuonContainer.h"
#include "xAODTrigger/MuonRoIContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "FourMomUtils/xAODP4Helpers.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "TRandom1.h"
#include <string>
#include <memory>
#include <tuple>


class MuonMatchingTool : public AthAlgTool {

 public:
  static const InterfaceID& interfaceID();
  MuonMatchingTool(const std::string& type, const std::string &name, const IInterface* parent);

  virtual StatusCode initialize() override;


  enum L1Items{ L1_MU4=1, L1_MU6, L1_MU10, L1_MU11, L1_MU15, L1_MU20, L1_MU21,ERROR};
  int L1ItemSTI(std::string l1item) const {
    if( "L1_MU4"==l1item) return L1Items::L1_MU4;
    if( "L1_MU6"==l1item) return L1Items::L1_MU6;
    if( "L1_MU10"==l1item) return L1Items::L1_MU10;
    if( "L1_MU11"==l1item) return L1Items::L1_MU11;
    if( "L1_MU15"==l1item) return L1Items::L1_MU15;
    if( "L1_MU20"==l1item) return L1Items::L1_MU20;
    if( "L1_MU21"==l1item) return L1Items::L1_MU21;
    return L1Items::ERROR;   
  }



  StatusCode matchL1(const xAOD::Muon *mu, SG::ReadHandle<xAOD::MuonRoIContainer> &murois, std::string trigger, bool &pass) const;
  StatusCode matchSA(const xAOD::Muon *mu, std::string trigger, bool &pass) const;
  StatusCode matchCB(const xAOD::Muon *mu, std::string trigger, bool &pass) const;
  StatusCode matchEF(const xAOD::Muon *mu, std::string trigger, bool &pass) const;

  const Trk::TrackParameters* extTrackToPivot(const xAOD::TrackParticle *track) const;
  const Trk::TrackParameters* extTrackToTGC(const xAOD::TrackParticle *track) const;
  const Trk::TrackParameters* extTrackToRPC(const xAOD::TrackParticle *track) const;

  static double reqdRL1byPt(double mupt);


 private:
  // private methods
  // Template methods that perform different matching schemes for T=xAOD::L2StandAloneMuon, xAOD::L2CombinedMuon and xAOD::Muon (EF).
  // See MuonMatchingTool.cxx for specialization and MuonMatchingTool.icc for implementation
  template<class T> inline std::tuple<double,double> trigPosForMatch(const T *trig) const;
  template<class T> inline std::tuple<double,double> offlinePosForMatch(const xAOD::Muon *mu) const;
  template<class T> StatusCode match(const xAOD::Muon *mu, std::string trigger, double reqdR, bool &pass) const;
  double FermiFunction(double x, double x0, double w) const;


  // properties
  Gaudi::Property<bool> m_use_extrapolator {this, "UseExtrapolator", false, "Flag to enable the extrapolator for matching offline and trigger muons"};

  // tools
  PublicToolHandle<Trig::TrigDecisionTool> m_trigDec {this, "TrigDecisionTool", "Trig::TrigDecisionTool/TrigDecisionTool", "TrigDecisionTool"};
  PublicToolHandle<Trk::IExtrapolator> m_extrapolator {"Trk::Extrapolator/AtlasExtrapolator"};
  //The extrapolator is currently not available. Once it gets available, initialize it with the following, which attempts to retrieve:
  //{this, "Extrapolator", "Trk::Extrapolator/AtlasExtrapolator", "Track extrapolator"}; 
};

#include "MuonMatchingTool.icc"

#endif //TRIGMUONMONITORINGMT_MUONMATCHINGTOOL_H
