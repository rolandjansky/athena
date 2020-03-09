/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONMONITORINGMT_MUONMATCHINGTOOL_H
#define TRIGMUONMONITORINGMT_MUONMATCHINGTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTrigger/MuonRoIContainer.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "FourMomUtils/xAODP4Helpers.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "TRandom1.h"
#include <string>
#include <memory>


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
  StatusCode matchL1(const xAOD::Muon *mu, std::string trigger, bool &pass) const;
  StatusCode matchSA(const xAOD::Muon *mu, std::string trigger, bool &pass) const;
  StatusCode matchCB(const xAOD::Muon *mu, std::string trigger, bool &pass) const;
  StatusCode matchEF(const xAOD::Muon *mu, std::string trigger, bool &pass) const;



 private:
  template<class T> StatusCode match(const xAOD::Muon *mu, std::string trigger, double reqdR, bool &pass) const;
  double FermiFunction(double x, double x0, double w) const;

  ToolHandle<Trig::TrigDecisionTool> m_trigDec;
  std::unique_ptr<TRandom1> m_rndm;
  bool m_ToyDecision;

};

#include "MuonMatchingTool.icc"

#endif //TRIGMUONMONITORINGMT_MUONMATCHINGTOOL_H
