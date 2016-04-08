/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*****************************************************************************
Name    : UserAnalysisSelectionTool.cxx
Package : offline/PhysicsAnalysis/AnalysisCommon/UserAnalysisUtils
Author  : Ketevi A. Assamagan
Created : November 2007
Purpose : User Analysis Selections - see UserAnalysisSelectionTool.h for details
*****************************************************************************/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/Property.h"

#include "egammaEvent/EMShower.h"

// Accessing data:
#include "CLHEP/Units/PhysicalConstants.h"

// User Tools
#include "UserAnalysisUtils/UserAnalysisSelectionTool.h"

#include <sstream>
#include <iomanip>
#include <iostream>

using namespace Analysis;
using namespace Rec;
using namespace std;
using CLHEP::GeV;

//------------------------------------------------------------------------------
UserAnalysisSelectionTool::UserAnalysisSelectionTool( const std::string& type,
                                                      const std::string& name, 
                                                      const IInterface* parent )
  : AthAlgTool( type, name, parent ) {
  declareInterface<UserAnalysisSelectionTool>( this );

  declareProperty("IsAtlfastData",          m_isAtlfast=false);

  /** caloCluster selection */
  declareProperty("CaloClusterE", m_caloClusterE=1.0*GeV);

  /** TrackParticle Pt */
  declareProperty("TrackParticlePt", m_trackParticlePt=1.0*GeV);

  /** Electron selection */
  declareProperty("ElectronPt",       m_electronPt=10*GeV);
  declareProperty("ElectronEta",      m_electronEta=2.5);
  declareProperty("ElectronIsEMFlag", m_electronIsEMFlag="Loose");
  declareProperty("ElectronIsEM",     m_electronIsEM=0);
  declareProperty("AuthorEgammaOnly", m_authorEgammaOnly=false);
  declareProperty("ElectronEtaWindowCut", m_electronEtaWindCut = false);
  declareProperty("ElectronEtaWindow", m_electronEtaWind = 0.075);
  declareProperty("ElectronEtaWindowCenter", m_electronEtaWindCent=1.445);
  declareProperty("DoElectronIsolation", m_doElectronIsolation=true);
  declareProperty("ElectronIsolationConeIndex", m_electronIsolationConeIndex=1);
  declareProperty("ElectronIsolationEt", m_electronIsolationEt=10*GeV);
  declareProperty("NormalizedElectronIsolationEt",m_normElectronIsolEt=0.2);

  /** Photon selection */
  declareProperty("PhotonPt",   m_photonPt=10*GeV);
  declareProperty("PhotonEta",  m_photonEta=3.2);
  declareProperty("PhotonIsEM", m_photonIsEM=0);

  /** Muon selection */
  declareProperty("MuonPt",                 m_muonPt=3.0*GeV);
  declareProperty("MuonEta",                m_muonEta=2.7);
  declareProperty("DoMuonIsolation",        m_doMuonIsolation=true);
  declareProperty("MuonIsolationConeIndex", m_muonIsolationConeIndex=1);
  declareProperty("MuonIsolationEt",        m_muonIsolationEt=10*GeV);
  declareProperty("UseMatchChi2",           m_useMatchChi2=false);
  declareProperty("MuonMatchChi2",          m_muonMatchChi2=100);
  declareProperty("NormalizedMuonIsolationEt",m_normMuonIsolEt=0.2);
  declareProperty("egDetailContainerName", m_egDetailContainerName="egDetailAOD");

  /** TauJet selection */
  declareProperty("TauJetPt",           m_tauJetPt=20*GeV);
  declareProperty("TauJetEta",          m_tauJetEta=2.5);
  declareProperty("TauJetLikelihood",   m_tauJetLikelihood=-6.0);
  declareProperty("TauEleBDTCut", m_tauEleBDTCut=0.5); // not yet set - No 23 1007

  /** Jet selection */
  declareProperty("JetPt",          m_jetPt=20*GeV);
  declareProperty("JetEta",         m_jetEta=5.0);
  declareProperty("BJetLikelihood", m_bJetLikelihood=6.0);

}

//------------------------------------------------------------------------------
StatusCode UserAnalysisSelectionTool::initialize() {

  ATH_MSG_DEBUG(MSG::DEBUG << "in initialize()");


  ATH_MSG_DEBUG(" electron Flag/isEM val "<< m_electronIsEMFlag <<","<<m_electronIsEM);
  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------
StatusCode UserAnalysisSelectionTool::finalize() {

  ATH_MSG_DEBUG("in finalize()");
 
  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------
UserAnalysisSelectionTool::~UserAnalysisSelectionTool()
{}

bool UserAnalysisSelectionTool::isSelected( const Analysis::Electron * electron ) const
{
  ATH_MSG_DEBUG("in electron isSelected()");

  bool select = false;
  if ( !electron ) return select;

  if ( m_isAtlfast ) {
    select = electron->pt()>m_electronPt && fabs(electron->eta())<m_electronEta;
    return select;
  }

  if ( m_electronIsEMFlag == "Loose" )                 select = electron->isem(egammaPID::ElectronLoose)==0;
  else if ( m_electronIsEMFlag == "Medium" )           select = electron->isem(egammaPID::ElectronMedium)==0;
  //else if ( m_electronIsEMFlag == "TightNoIsolation" ) select = electron->isem(egammaPID::ElectronTightNoIsolation)==0;
  else if ( m_electronIsEMFlag == "Tight" )            select = electron->isem(egammaPID::ElectronTight)==0;
  else select = ((electron->isem()&m_electronIsEM)==0); 

  ATH_MSG_DEBUG("in electron isSelected() -1 ");

  if ( m_authorEgammaOnly ) select = select && electron->author(egammaParameters::AuthorElectron);
  select = select && electron->pt()>m_electronPt && fabs(electron->eta())<m_electronEta;
  // check if electron is in bad eta region

  ATH_MSG_DEBUG("in electron isSelected() - 2 ");
  if(m_electronEtaWindCut) {

    double diff = fabs( electron->eta()) - m_electronEtaWindCent;
    select = select &&( fabs(diff)  > m_electronEtaWind );
  }

  ATH_MSG_DEBUG("in electron isSelected() " << m_egDetailContainerName);

  if ( m_doElectronIsolation ) {
    const EMShower* egdetail = electron->detail<EMShower>(m_egDetailContainerName);
    ATH_MSG_VERBOSE("in electron isSelected() " << egdetail);
    double etIsol = 0.;
    if(egdetail) {
      etIsol = egdetail->parameter( static_cast<egammaParameters::ParamDef>(m_electronIsolationConeIndex) );
    }
    select = select && ( etIsol < m_electronIsolationEt );
    if ( electron->pt() ) select = select && ( (etIsol/electron->pt()) < m_normElectronIsolEt );
  }
  ATH_MSG_DEBUG("in electron isSelected() - 4 ");

  return select;
}

bool UserAnalysisSelectionTool::isSelected( const Analysis::Photon * photon ) const 
{
  bool select = false;
  if ( !photon ) return select;

  if ( m_isAtlfast ) {
    select = photon->pt()>m_photonPt && fabs(photon->eta())<m_photonEta;
    return select;
  }
 
  select = photon->pt()>m_photonPt && fabs(photon->eta())<m_photonEta && photon->isem()==m_photonIsEM;
  return select;
}

bool UserAnalysisSelectionTool::isSelected( const Analysis::Muon * muon ) const
{
  bool select = false;
  if ( !muon ) return select;

  if ( m_isAtlfast ) {
    select = muon->pt()>m_muonPt && fabs(muon->eta())<m_muonEta;
    return select;
  }

  select = muon->pt()>m_muonPt && fabs(muon->eta())<m_muonEta;
  if ( m_useMatchChi2 && muon->isCombinedMuon() ) select = select && muon->matchChi2() < m_muonMatchChi2;
  if ( m_doMuonIsolation ) {
    double etIsol = muon->parameter( static_cast<MuonParameters::ParamDef>(m_muonIsolationConeIndex) );
    select = select && ( etIsol < m_muonIsolationEt );
    if ( muon->pt() ) select = select && ( (etIsol/muon->pt()) < m_normMuonIsolEt );
 
  }

  return select;
}

bool UserAnalysisSelectionTool::isSelected( const Jet* jet ) const
{
  bool select = false;
  if ( !jet ) return select;
  select = jet->pt() > m_jetPt && fabs(jet->eta()) < m_jetEta;

  if ( m_isAtlfast ) return select;

  return select;
}

bool UserAnalysisSelectionTool::isSelected( const Rec::TrackParticle * trackParticle ) const
{
  bool select = false;
  if ( !trackParticle ) return select;
  select = trackParticle->pt() > m_trackParticlePt; 

  if ( m_isAtlfast ) return select;

  return select;
}

bool UserAnalysisSelectionTool::isSelected( const CaloCluster* caloCluster ) const
{
  bool select = false;
  if ( !caloCluster ) return select;
  select = caloCluster->e() > m_caloClusterE;

  if ( m_isAtlfast ) return select;

  return select;
}

bool UserAnalysisSelectionTool::isSelected( const Analysis::TauJet * tauJet ) const {

  bool select = false;
  if ( !tauJet ) return select;

  int numTrack = tauJet->numTrack();
  select = tauJet->pt()>m_tauJetPt &&
           fabs(tauJet->eta())<m_tauJetEta &&
           fabs(tauJet->charge())==1.0 &&
           (numTrack==1 || numTrack==3);

  const Analysis::TauPID* tauId = tauJet->tauID();
  if ( tauId ) {
    select = select &&
             tauId->discriminant( TauJetParameters::Likelihood ) > m_tauJetLikelihood &&
             tauId->discriminant( TauJetParameters::BDTEleScore )> m_tauEleBDTCut;
  }

  return select;

}

bool UserAnalysisSelectionTool::isBJet( const Jet * jet ) const {

  /** first check that it is a selected jet */
  if ( !jet ) return false;

  bool is_bjet = this->isSelected( jet );
  return ( is_bjet && jet->getFlavourTagWeight()>m_bJetLikelihood );
}

