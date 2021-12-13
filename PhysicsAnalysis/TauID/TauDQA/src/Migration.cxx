/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <utility>

#include "Migration.h"

namespace Tau{

  Migration::Migration(PlotBase* pParent, const std::string& sDir, std::string sTauJetContainerName):
    PlotBase(pParent, sDir),
    m_migration_panTau(nullptr),
    m_migration_panTauProto(nullptr),
    m_migration_cellBased(nullptr),
    m_sTauJetContainerName(std::move(sTauJetContainerName))
  {
  }

  Migration::~Migration()
  {
  }

  void Migration::initializePlots()
  {
    m_migration_panTau = Book1D("panTau_migration",m_sTauJetContainerName + " panTau migration",DECAYSIZE,0,DECAYSIZE);
    m_migration_panTauProto = Book1D("panTauProto_migration",m_sTauJetContainerName + " panTau proto migration",DECAYSIZE,0,DECAYSIZE);
    m_migration_cellBased = Book1D("cellBased_migration",m_sTauJetContainerName + " cellBased migration",DECAYSIZE,0,DECAYSIZE);
    m_migration_panTau->GetXaxis()->SetLabelSize(0.05);
    m_migration_panTauProto->GetXaxis()->SetLabelSize(0.05);
    m_migration_cellBased->GetXaxis()->SetLabelSize(0.05);
    for(int i=1; i<= DECAYSIZE;i++){
      m_migration_panTauProto->GetXaxis()->SetBinLabel(i,m_lable[i-1]);
      m_migration_panTau->GetXaxis()->SetBinLabel(i,m_lable[i-1]);
      m_migration_cellBased->GetXaxis()->SetBinLabel(i,m_lable[i-1]);
    }
  }


  void Migration::fill(const xAOD::TauJet& thisTau, xAOD::TauJetParameters::DecayMode trueMode, float weight)
  {
    int isPanTauCandidate = 0;
    bool foundDetail = thisTau.panTauDetail(xAOD::TauJetParameters::PanTauDetails::PanTau_isPanTauCandidate, isPanTauCandidate);
    if ( !foundDetail || !isPanTauCandidate ) return;

    // panTau
    int recMode = xAOD::TauJetParameters::DecayMode::Mode_Error;
    foundDetail = thisTau.panTauDetail(xAOD::TauJetParameters::PanTauDetails::PanTau_DecayMode, recMode);
    if ( foundDetail ) decayModeFill(trueMode, recMode, m_migration_panTau, weight);

    // panTauProto
    recMode = xAOD::TauJetParameters::DecayMode::Mode_Error;
    foundDetail = thisTau.panTauDetail(xAOD::TauJetParameters::PanTauDetails::PanTau_DecayModeProto, recMode);
    if ( foundDetail ) {
      decayModeFill(trueMode, recMode, m_migration_panTauProto, weight);
    }

    int cellP = thisTau.nTracks();

    // Get number of neutral pions
    int nPi0_tau = 0;
    const std::vector<ElementLink<xAOD::PFOContainer>>& cellBased_neutralPFO = thisTau.protoNeutralPFOLinks();
    for(const auto& link : cellBased_neutralPFO) {
      const xAOD::PFO* PFO = *link;
      int myNPi0Proto = 0;
      if ( PFO->attribute(xAOD::PFODetails::nPi0Proto, myNPi0Proto) ) {
	nPi0_tau+=myNPi0Proto;
      }
    }
    decayModeFill(trueMode, cellP, nPi0_tau, m_migration_cellBased, weight);
  }

  void Migration::decayModeFill(int trueMode, int recMode, TH1 *histo, float weight)
  {
    if ( recMode >= xAOD::TauJetParameters::DecayMode::Mode_Other || trueMode >= xAOD::TauJetParameters::DecayMode::Mode_Other ) return;
   
    switch ( trueMode ) {
    case xAOD::TauJetParameters::DecayMode::Mode_1p0n:
      switch ( recMode ) {
      case xAOD::TauJetParameters::DecayMode::Mode_1p0n:
	histo->Fill(t10r10 + 0.5, weight);
	break;
      case xAOD::TauJetParameters::DecayMode::Mode_1p1n:
	histo->Fill(t10r11 + 0.5, weight);
	break;
      case xAOD::TauJetParameters::DecayMode::Mode_1pXn:
	histo->Fill(t10r1x + 0.5, weight);
	break;
      default:
	histo->Fill(t1r3 + 0.5, weight);
	break;
      }
      break;
    case xAOD::TauJetParameters::DecayMode::Mode_1p1n:
      switch ( recMode ) {
      case xAOD::TauJetParameters::DecayMode::Mode_1p0n:
	histo->Fill(t11r10 + 0.5, weight);
	break;
      case xAOD::TauJetParameters::DecayMode::Mode_1p1n:
	histo->Fill(t11r11 + 0.5, weight);
	break;
      case xAOD::TauJetParameters::DecayMode::Mode_1pXn:
	histo->Fill(t11r1x + 0.5, weight);
	break;
      default:
	histo->Fill(t1r3 + 0.5, weight);
	break;
      }
      break;
    case xAOD::TauJetParameters::DecayMode::Mode_1pXn:
      switch ( recMode ) {
      case xAOD::TauJetParameters::DecayMode::Mode_1p0n:
	histo->Fill(t1xr10 + 0.5, weight);
	break;
      case xAOD::TauJetParameters::DecayMode::Mode_1p1n:
	histo->Fill(t1xr11 + 0.5, weight);
	break;
      case xAOD::TauJetParameters::DecayMode::Mode_1pXn:
	histo->Fill(t1xr1x + 0.5, weight);
	break;
      default:
	histo->Fill(t1r3 + 0.5, weight);
	break;
      }
      break;
    case xAOD::TauJetParameters::DecayMode::Mode_3p0n:
      switch ( recMode ) {
      case xAOD::TauJetParameters::DecayMode::Mode_3p0n:
	histo->Fill(t30r30 + 0.5, weight);
	break;
      case xAOD::TauJetParameters::DecayMode::Mode_3pXn:
	histo->Fill(t30r3x + 0.5, weight);
	break;
      default:
	histo->Fill(t3r1 + 0.5, weight);
	break;
      }
      break;
    case xAOD::TauJetParameters::DecayMode::Mode_3pXn:
      switch ( recMode ) {
      case xAOD::TauJetParameters::DecayMode::Mode_3p0n:
	histo->Fill(t3xr30 + 0.5, weight);
	break;
      case xAOD::TauJetParameters::DecayMode::Mode_3pXn:
	histo->Fill(t3xr3x + 0.5, weight);
	break;
      default:
	histo->Fill(t3r1 + 0.5, weight);
	break;
      }
      break;
    }
    return;
  }

  void Migration::decayModeFill(int trueMode, int recP, int recN, TH1 *histo, float weight)
  {
    xAOD::TauJetParameters::DecayMode recMode = xAOD::TauJetParameters::DecayMode::Mode_Error;
    if ( recP == 1 && recN == 0 ) {
      recMode = xAOD::TauJetParameters::DecayMode::Mode_1p0n;
    }
    if ( recP == 1 && recN == 1 ) {
      recMode = xAOD::TauJetParameters::DecayMode::Mode_1p1n;
    }
    if ( recP == 1 && recN > 1 ) {
      recMode = xAOD::TauJetParameters::DecayMode::Mode_1pXn;
    }
    if ( recP == 3 && recN == 0 ) {
      recMode = xAOD::TauJetParameters::DecayMode::Mode_3p0n;
    }
    if ( recP == 3 && recN >= 1 ){
      recMode = xAOD::TauJetParameters::DecayMode::Mode_3pXn;
    }
    this->decayModeFill(trueMode, recMode, histo, weight);
  }
      
}
