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

// EDM include(s):
#include "egammaEvent/Electron.h"
#include "egammaEvent/Photon.h"
#include "egammaEvent/EMShower.h"
#include "muonEvent/Muon.h"
#include "tauEvent/TauJet.h"
#include "JetEvent/Jet.h"
#include "MissingETEvent/MissingET.h"
#include "Particle/TrackParticleContainer.h"
#include "CaloEvent/CaloClusterContainer.h"

// Local include(s):
#include "UserAnalysisUtils/UserAnalysisSelectionTool.h"

/// Convenience GeV definition
static const double GeV = 1000.0;

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
   declareProperty("ElectronIsolationConeIndex",
                   m_electronIsolationConeIndex=1);
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
   declareProperty("egDetailContainerName",
                   m_egDetailContainerName="egDetailAOD");

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

StatusCode UserAnalysisSelectionTool::initialize() {

   ATH_MSG_DEBUG( "in initialize()" );
   ATH_MSG_DEBUG( " electron Flag/isEM val " << m_electronIsEMFlag << ","
                  << m_electronIsEM );

   return StatusCode::SUCCESS;
}

bool UserAnalysisSelectionTool::
isSelected( const Analysis::Electron* electron ) const {

   if( ! electron ) {
      return false;
   }

   ATH_MSG_VERBOSE( "in electron isSelected()" );

   bool select = ( ( electron->pt() > m_electronPt ) &&
                   ( std::abs( electron->eta() ) < m_electronEta ) );

   if( m_isAtlfast ) {
      return select;
   }

   if( m_electronIsEMFlag == "Loose" ) {
      select = select && ( ! electron->isem( egammaPIDObs::ElectronLoose ) );
   } else if( m_electronIsEMFlag == "Medium" ) {
      select = select && ( ! electron->isem( egammaPIDObs::ElectronMedium ) );
   } else if( m_electronIsEMFlag == "Tight" ) {
      select = select && ( ! electron->isem( egammaPIDObs::ElectronTight ) );
   } else {
      select = select && ( ( electron->isem() & m_electronIsEM ) ==0 );
   }

   ATH_MSG_VERBOSE( "in electron isSelected() -1 " );

   if( m_authorEgammaOnly ) {
      select = select && electron->author( egammaParameters::AuthorElectron );
   }

   ATH_MSG_VERBOSE( "in electron isSelected() - 2 " );
   if( m_electronEtaWindCut ) {
      const double diff = std::abs( electron->eta() ) - m_electronEtaWindCent;
      select = select && ( std::abs( diff ) > m_electronEtaWind );
   }

   ATH_MSG_VERBOSE( "in electron isSelected() " << m_egDetailContainerName );

   if( m_doElectronIsolation ) {
      const EMShower* egdetail =
            electron->detail< EMShower >( m_egDetailContainerName );
      ATH_MSG_VERBOSE( "in electron isSelected() " << egdetail );
      double etIsol = 0.;
      if( egdetail ) {
         const auto helper1 = m_electronIsolationConeIndex;
         const egammaParameters::ParamDef helper2 =
               static_cast< egammaParameters::ParamDef >( helper1 );
         etIsol = egdetail->parameter( helper2 );
      }
      select = select && ( etIsol < m_electronIsolationEt );
      if( electron->pt() ) {
         select = select && ( ( etIsol / electron->pt() ) <
                              m_normElectronIsolEt );
      }
   }
   ATH_MSG_VERBOSE("in electron isSelected() - 4 ");

   return select;
}

bool UserAnalysisSelectionTool::
isSelected( const Analysis::Photon* photon ) const  {

   if( ! photon ) {
      return false;
   }

   bool select = ( ( photon->pt() > m_photonPt ) &&
                   ( std::abs( photon->eta() ) < m_photonEta ) );

   if( m_isAtlfast ) {
      return select;
   }

   select = select && ( photon->isem() == m_photonIsEM );
   return select;
}

bool UserAnalysisSelectionTool::
isSelected( const Analysis::Muon* muon ) const {

   if( ! muon ) {
      return false;
   }

   bool select = ( ( muon->pt() > m_muonPt ) &&
                   ( std::abs( muon->eta() ) < m_muonEta ) );

   if( m_isAtlfast ) {
      return select;
   }
   if( m_useMatchChi2 && muon->isCombinedMuon() ) {
      select = select && ( muon->matchChi2() < m_muonMatchChi2 );
   }
   if( m_doMuonIsolation ) {
      const MuonParameters::ParamDef helper =
            static_cast< MuonParameters::ParamDef >( m_muonIsolationConeIndex );
      const double etIsol = muon->parameter( helper );
      select = select && ( etIsol < m_muonIsolationEt );
      if ( muon->pt() ) {
         select = select && ( ( etIsol / muon->pt() ) < m_normMuonIsolEt );
      }
   }

   return select;
}

bool UserAnalysisSelectionTool::isSelected( const Jet* jet ) const {

   if( ! jet ) {
      return false;
   }

   bool select = ( ( jet->pt() > m_jetPt ) &&
                   ( std::abs( jet->eta() ) < m_jetEta ) );

   return select;
}

bool UserAnalysisSelectionTool::
isSelected( const Rec::TrackParticle* trackParticle ) const {

   if( ! trackParticle ) {
      return false;
   }

   bool select = trackParticle->pt() > m_trackParticlePt;

   return select;
}

bool UserAnalysisSelectionTool::
isSelected( const CaloCluster* caloCluster ) const {

   if( ! caloCluster ) {
      return false;
   }

   bool select = caloCluster->e() > m_caloClusterE;

   return select;
}

bool UserAnalysisSelectionTool::
isSelected( const Analysis::TauJet* tauJet ) const {

   if( ! tauJet ) {
      return false;
   }

   const int numTrack = tauJet->numTrack();
   bool select = ( ( tauJet->pt() > m_tauJetPt ) &&
                   ( std::abs( tauJet->eta() ) < m_tauJetEta ) &&
                   ( ( std::abs( tauJet->charge() ) - 1.0 ) < 0.001 ) &&
                   ( ( numTrack == 1 ) || ( numTrack == 3 ) ) );

   const Analysis::TauPID* tauId = tauJet->tauID();
   if ( tauId ) {
      const auto likelihood =
            tauId->discriminant( TauJetParameters::Likelihood );
      const auto bdtelescore =
            tauId->discriminant( TauJetParameters::BDTEleScore );
      select = select && ( ( likelihood > m_tauJetLikelihood ) &&
                           ( bdtelescore > m_tauEleBDTCut ) );
   }

   return select;
}

bool UserAnalysisSelectionTool::isBJet( const Jet* jet ) const {

   if( ! jet ) {
      return false;
   }

   bool select = this->isSelected( jet );
   return ( select && ( jet->getFlavourTagWeight() > m_bJetLikelihood ) );
}
