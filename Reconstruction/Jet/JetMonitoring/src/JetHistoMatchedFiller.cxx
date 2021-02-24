/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "JetMonitoring/JetHistoMatchedFiller.h"
#include "JetMonitoring/JetMonitoringAlg.h"
#include "AsgDataHandles/ReadDecorHandle.h"

JetHistoMatchedFiller::JetHistoMatchedFiller( const std::string& type,  const std::string & name ,const IInterface* parent):
  AthAlgTool( type, name, parent )
{
  declareInterface<IJetHistoFiller>(this);
  declareProperty("JetMatchedKey",m_matchedKey="NONE");
  declareProperty("JetPtDiffKey", m_ptdiffKey="NONE");
  declareProperty("JetEnergyDiffKey", m_energydiffKey="NONE");
  declareProperty("JetMassDiffKey", m_massdiffKey="NONE");
  declareProperty("JetPtRespKey", m_ptrespKey="NONE");
  declareProperty("JetEnergyRespKey", m_energyrespKey="NONE");
  declareProperty("JetMassRespKey", m_massrespKey="NONE");
  declareProperty("JetPtRefKey", m_ptrefKey="NONE");
  declareProperty("JetEtaRefKey", m_etarefKey="NONE");
}


StatusCode JetHistoMatchedFiller::initialize() {
  ATH_CHECK( m_matchedKey.initialize() );
  ATH_CHECK( m_ptdiffKey.initialize() );
  ATH_CHECK( m_energydiffKey.initialize() );
  ATH_CHECK( m_massdiffKey.initialize() );
  ATH_CHECK( m_ptrespKey.initialize() );
  ATH_CHECK( m_energyrespKey.initialize() );
  ATH_CHECK( m_massrespKey.initialize() );
  ATH_CHECK( m_ptrefKey.initialize() );
  ATH_CHECK( m_etarefKey.initialize() );
  return StatusCode::SUCCESS;
}


StatusCode JetHistoMatchedFiller::processJetContainer(const JetMonitoringAlg& parentAlg, const xAOD::JetContainer & jets, const EventContext& ctx ) const {
  if(jets.empty()) return StatusCode::SUCCESS;

  SG::ReadDecorHandle<xAOD::JetContainer, char>  matchedHandle(m_matchedKey, ctx);
  SG::ReadDecorHandle<xAOD::JetContainer, double> ptdiffHandle(m_ptdiffKey, ctx);
  SG::ReadDecorHandle<xAOD::JetContainer, double> energydiffHandle(m_energydiffKey, ctx);
  SG::ReadDecorHandle<xAOD::JetContainer, double> massdiffHandle(m_massdiffKey, ctx);
  SG::ReadDecorHandle<xAOD::JetContainer, double> ptrespHandle(m_ptrespKey, ctx);
  SG::ReadDecorHandle<xAOD::JetContainer, double> energyrespHandle(m_energyrespKey, ctx);
  SG::ReadDecorHandle<xAOD::JetContainer, double> massrespHandle(m_massrespKey, ctx);
  SG::ReadDecorHandle<xAOD::JetContainer, double> ptrefHandle(m_ptrefKey, ctx);
  SG::ReadDecorHandle<xAOD::JetContainer, double> etarefHandle(m_etarefKey, ctx);
 
  auto matched = Monitored::Collection("matched", jets, [matchedHandle](const xAOD::Jet * jet) {return matchedHandle(*jet);}); 
  auto dPt = Monitored::Scalar("ptdiff",0.0);
  auto dEnergy = Monitored::Scalar("energydiff",0.0);
  auto dMass = Monitored::Scalar("massdiff",0.0);
  auto rPt = Monitored::Scalar("ptresp",0.0);
  auto rEnergy = Monitored::Scalar("energyresp",0.0);
  auto rMass = Monitored::Scalar("massresp",0.0);
  auto ptRef = Monitored::Scalar("ptref",0.0);
  auto etaRef = Monitored::Scalar("etaref",0.0);

  // Loop over jets and fill pt, energy, mass differences and responses between matched jets, plus reference pT and eta
  
  for(const xAOD::Jet* jet : jets){
    bool matched = matchedHandle(*jet);
    if(matched){
        dPt = ptdiffHandle(*jet);
        dEnergy = energydiffHandle(*jet);
        dMass = massdiffHandle(*jet);
        rPt = ptrespHandle(*jet);
        rEnergy = energyrespHandle(*jet);
        rMass = massrespHandle(*jet);
        ptRef = ptrefHandle(*jet);
        etaRef = etarefHandle(*jet);
        parentAlg.fill(m_group, dPt, dEnergy, dMass, rPt, rEnergy, rMass, ptRef, etaRef);
    } 
  }
 
  parentAlg.fill(m_group,matched);
  
  return StatusCode::SUCCESS;
}


