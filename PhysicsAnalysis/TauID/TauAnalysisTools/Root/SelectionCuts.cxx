// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SELECTIONCUTS_CXX
#define SELECTIONCUTS_CXX

#include "TauAnalysisTools/SelectionCuts.h"

using namespace TauAnalysisTools;

//______________________________________________________________________________
SelectionCut::SelectionCut(std::string sName, TauAnalysisTools::TauSelectionTool* tTST)
  : m_sName(sName)
  , m_hHistCutPre(0)
  , m_hHistCut(0)
  , m_tTST(tTST)
{
}

//______________________________________________________________________________
SelectionCut::~SelectionCut()
{
  delete m_hHistCutPre;
  delete m_hHistCut;
}

//______________________________________________________________________________
void SelectionCut::writeControlHistograms()
{
  if (m_tTST->m_bCreateControlPlots)
  {
    m_hHistCutPre->Write();
    m_hHistCut->Write();
  }
}

//______________________________________________________________________________
TH1F* SelectionCut::CreateControlPlot(const char* sName, const char* sTitle, int iBins, double dXLow, double dXUp)
{
  // hHist.SetDirectory(0);
  if (m_tTST->m_fOutFile and m_tTST->m_bCreateControlPlots)
  {
    TH1F* hHist = new TH1F(sName, sTitle, iBins, dXLow, dXUp);
    hHist->SetDirectory(0);
    return hHist;
  }

  return NULL;
}

//______________________________________________________________________________
void SelectionCut::fillHistogramCutPre(const xAOD::TauJet& xTau)
{
  fillHistogram(xTau, *m_hHistCutPre);
}

//______________________________________________________________________________
void SelectionCut::fillHistogramCut(const xAOD::TauJet& xTau)
{
  fillHistogram(xTau, *m_hHistCut);
}

//_______________________________SelectionCutPt_________________________________
//______________________________________________________________________________
SelectionCutPt::SelectionCutPt(TauSelectionTool* tTST)
  : SelectionCut("CutPt", tTST)
{
  m_hHistCutPre = CreateControlPlot("hPt_pre","Pt_pre;#tau-p_{T} [GeV]; events",100,0,100);
  m_hHistCut = CreateControlPlot("hPt_cut","Pt_cut;#tau-p_{T} [GeV]; events",100,0,100);
}

//______________________________________________________________________________
void SelectionCutPt::fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist)
{
  hHist.Fill(xTau.pt()/1000.);
}

//______________________________________________________________________________
bool SelectionCutPt::accept(const xAOD::TauJet& xTau)
{
  m_tTST->m_aAccept.addCut( "Pt",
                            "Selection of taus according to their transverse momentum" );
  // save tau pt in GeV
  double pt = xTau.pt() / 1000.;
  // in case of only one entry in vector, run for lower limits
  if (m_tTST->m_vPtRegion.size() == 1)
  {
    if ( pt >= m_tTST->m_vPtRegion.at(0) )
    {
      m_tTST->m_aAccept.setCutResult( "Pt", true );
      return true;
    }
  }
  unsigned int iNumPtRegion = m_tTST->m_vPtRegion.size()/2;
  for( unsigned int iPtRegion = 0; iPtRegion < iNumPtRegion; iPtRegion++ )
  {
    if ( pt >= m_tTST->m_vPtRegion.at(iPtRegion*2) and pt <= m_tTST->m_vPtRegion.at(iPtRegion*2+1))
    {
      m_tTST->m_aAccept.setCutResult( "Pt", true );
      return true;
    }
  }
  // ATH_MSG_VERBOSE( "Tau failed pt region requirement, tau pt: " << pt );
  return false;
}

//_____________________________SelectionCutAbsEta_______________________________
//______________________________________________________________________________
SelectionCutAbsEta::SelectionCutAbsEta(TauSelectionTool* tTST)
  : SelectionCut("CutAbsEta", tTST)
{
  m_hHistCutPre = CreateControlPlot("hEta_pre","Eta_pre;#tau-#eta; events",100,-3,3);
  m_hHistCut = CreateControlPlot("hEta_cut","Eta_cut;#tau-#eta; events",100,-3,3);
}

//______________________________________________________________________________
void SelectionCutAbsEta::fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist)
{
  hHist.Fill(xTau.eta());
}

//______________________________________________________________________________
bool SelectionCutAbsEta::accept(const xAOD::TauJet& xTau)
{
  // check regions of eta, if tau is in one region then return true; false otherwise
  m_tTST->m_aAccept.addCut( "AbsEta",
                            "Selection of taus according to their absolute pseudorapidity" );
  unsigned int iNumEtaRegion = m_tTST->m_vAbsEtaRegion.size()/2;
  for( unsigned int iEtaRegion = 0; iEtaRegion < iNumEtaRegion; iEtaRegion++ )
  {
    if ( std::abs( xTau.eta() ) >= m_tTST->m_vAbsEtaRegion.at(iEtaRegion*2) and std::abs( xTau.eta() ) <= m_tTST->m_vAbsEtaRegion.at(iEtaRegion*2+1))
    {
      m_tTST->m_aAccept.setCutResult( "Eta", true );
      return true;
    }
  }
  // ATH_MSG_VERBOSE( "Tau failed eta region requirement, tau eta: " << xTau.eta() );
  return false;
}

//____________________________SelectionCutAbsCharge_____________________________
//______________________________________________________________________________
SelectionCutAbsCharge::SelectionCutAbsCharge(TauSelectionTool* tTST)
  : SelectionCut("CutAbsCharge", tTST)
{
  m_hHistCutPre = CreateControlPlot("hCharge_pre","Charge_pre;charge; events",7,-3.5,3.5);
  m_hHistCut = CreateControlPlot("hCharge_cut","Charge_cut;charge; events",7,-3.5,3.5);
}

//______________________________________________________________________________
void SelectionCutAbsCharge::fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist)
{
  hHist.Fill(xTau.charge());
}

//______________________________________________________________________________
bool SelectionCutAbsCharge::accept(const xAOD::TauJet& xTau)
{
  // check charge, if tau has one of the charges requiered then return true; false otherwise
  m_tTST->m_aAccept.addCut( "AbsCharge",
                            "Selection of taus according to their absolute charge" );
  for( unsigned int iCharge = 0; iCharge < m_tTST->m_vAbsCharges.size(); iCharge++ )
  {
    if ( std::abs( xTau.charge() ) == m_tTST->m_vAbsCharges.at(iCharge) )
    {
      m_tTST->m_aAccept.setCutResult( "AbsCharge", true );
      return true;
    }
  }
  // ATH_MSG_VERBOSE( "Tau failed charge requirement, tau charge: " << xTau.charge() );
  return false;
}

//_____________________________SelectionCutNTracks______________________________
//______________________________________________________________________________
SelectionCutNTracks::SelectionCutNTracks(TauSelectionTool* tTST)
  : SelectionCut("CutNTrack", tTST)
{
  m_hHistCutPre = CreateControlPlot("hNTrack_pre","NTrack_pre;number of tracks; events",22,-1.5,20.5);
  m_hHistCut = CreateControlPlot("hNTrack_cut","NTrack_cut;number of tracks; events",22,-1.5,20.5);
}

//______________________________________________________________________________
void SelectionCutNTracks::fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist)
{
  hHist.Fill(xTau.nTracks());
}

//______________________________________________________________________________
bool SelectionCutNTracks::accept(const xAOD::TauJet& xTau)
{
  // check track multiplicity, if tau has one of the number of tracks requiered then return true; false otherwise
  m_tTST->m_aAccept.addCut( "NTrack",
                            "Selection of taus according to their number of associated tracks" );
  for( size_t iNumTrack = 0; iNumTrack < m_tTST->m_vNTracks.size(); iNumTrack++ )
  {
    if ( xTau.nTracks() == m_tTST->m_vNTracks.at(iNumTrack) )
    {
      m_tTST->m_aAccept.setCutResult( "NTrack", true );
      return true;
    }
  }
  // ATH_MSG_VERBOSE( "Tau failed nTracks requirement, tau number of tracks: " << xTau.nTracks() );
  return false;
}

//___________________________SelectionCutBDTJetScore____________________________
//______________________________________________________________________________
SelectionCutBDTJetScore::SelectionCutBDTJetScore(TauSelectionTool* tTST)
  : SelectionCut("CutJetBDTScore", tTST)
{
  m_hHistCutPre = CreateControlPlot("hJetBDT_pre","JetBDT_pre;BDTJetScore; events",100,0,1);
  m_hHistCut = CreateControlPlot("hJetBDT_cut","JetBDT_cut;BDTJetScore; events",100,0,1);
}

//______________________________________________________________________________
void SelectionCutBDTJetScore::fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist)
{
  hHist.Fill(xTau.discriminant(xAOD::TauJetParameters::BDTJetScore));
}

//______________________________________________________________________________
bool SelectionCutBDTJetScore::accept(const xAOD::TauJet& xTau)
{
  // check JetBDTscore, if tau has a JetBDT score in one of the regions requiered then return true; false otherwise
  m_tTST->m_aAccept.addCut( "JetBDTScore",
                            "Selection of taus according to their JetBDTScore" );
  double dJetBDTScore = xTau.discriminant(xAOD::TauJetParameters::BDTJetScore);
  unsigned int iNumJetBDTRegion = m_tTST->m_vJetBDTRegion.size()/2;
  for( unsigned int iJetBDTRegion = 0; iJetBDTRegion < iNumJetBDTRegion; iJetBDTRegion++ )
  {
    if ( dJetBDTScore >= m_tTST->m_vJetBDTRegion.at(iJetBDTRegion*2) and dJetBDTScore <= m_tTST->m_vJetBDTRegion.at(iJetBDTRegion*2+1))
    {
      m_tTST->m_aAccept.setCutResult( "JetBDTScore", true );
      return true;
    }
  }
  // ATH_MSG_VERBOSE( "Tau failed JetBDTScore requirement, tau JetBDTScore: " << dJetBDTScore );
  return false;
}

//_____________________________SelectionCutJetIDWP______________________________
//______________________________________________________________________________
SelectionCutJetIDWP::SelectionCutJetIDWP(TauSelectionTool* tTST)
  : SelectionCut("CutJetIDWP", tTST)
{
  m_hHistCutPre = CreateControlPlot("hJetIDWP_pre","JetIDWP_pre;; events",6,-.5,5.5);
  m_hHistCut = CreateControlPlot("hJetIDWP_cut","JetIDWP_cut;; events",6,-.5,5.5);
  // only proceed if histograms are defined
  if (!m_hHistCutPre or !m_hHistCut)
    return;
  m_hHistCutPre->GetXaxis()->SetBinLabel(1,"!Loose");
  m_hHistCutPre->GetXaxis()->SetBinLabel(2,"Loose");
  m_hHistCutPre->GetXaxis()->SetBinLabel(3,"!Medium");
  m_hHistCutPre->GetXaxis()->SetBinLabel(4,"Medium");
  m_hHistCutPre->GetXaxis()->SetBinLabel(5,"!Tight");
  m_hHistCutPre->GetXaxis()->SetBinLabel(6,"Tight");
  m_hHistCut->GetXaxis()->SetBinLabel(1,"!Loose");
  m_hHistCut->GetXaxis()->SetBinLabel(2,"Loose");
  m_hHistCut->GetXaxis()->SetBinLabel(3,"!Medium");
  m_hHistCut->GetXaxis()->SetBinLabel(4,"Medium");
  m_hHistCut->GetXaxis()->SetBinLabel(5,"!Tight");
  m_hHistCut->GetXaxis()->SetBinLabel(6,"Tight");
}

//______________________________________________________________________________
void SelectionCutJetIDWP::fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist)
{
  hHist.Fill(xTau.isTau(xAOD::TauJetParameters::JetBDTSigLoose));
  hHist.Fill(xTau.isTau(xAOD::TauJetParameters::JetBDTSigMedium)+2);
  hHist.Fill(xTau.isTau(xAOD::TauJetParameters::JetBDTSigTight)+4);
}

//______________________________________________________________________________
bool SelectionCutJetIDWP::accept(const xAOD::TauJet& xTau)
{
  // check Jet ID working point, if tau passes JetID working point then return true; false otherwise
  m_tTST->m_aAccept.addCut( "JetIDWP",
                            "Selection of taus according to their JetIDScore" );
  bool pass = false;
  if (m_tTST->m_iJetIDWP == JETIDNONE)
    pass = true;
  if (m_tTST->m_iJetIDWP == JETIDBDTLOOSE)
    if (xTau.isTau(xAOD::TauJetParameters::JetBDTSigLoose))
      pass = true;
  if (m_tTST->m_iJetIDWP == JETIDBDTMEDIUM)
    if (xTau.isTau(xAOD::TauJetParameters::JetBDTSigMedium))
      pass = true;
  if (m_tTST->m_iJetIDWP == JETIDBDTTIGHT)
    if (xTau.isTau(xAOD::TauJetParameters::JetBDTSigTight))
      pass = true;
  if (m_tTST->m_iJetIDWP == JETIDBDTLOOSENOTMEDIUM)
    if (xTau.isTau(xAOD::TauJetParameters::JetBDTSigLoose) and !xTau.isTau(xAOD::TauJetParameters::JetBDTSigMedium))
      pass = true;
  if (m_tTST->m_iJetIDWP == JETIDBDTLOOSENOTTIGHT)
    if (xTau.isTau(xAOD::TauJetParameters::JetBDTSigLoose) and !xTau.isTau(xAOD::TauJetParameters::JetBDTSigTight))
      pass = true;
  if (m_tTST->m_iJetIDWP == JETIDBDTMEDIUMNOTTIGHT)
    if (xTau.isTau(xAOD::TauJetParameters::JetBDTSigMedium) and !xTau.isTau(xAOD::TauJetParameters::JetBDTSigTight))
      pass = true;
  if (m_tTST->m_iJetIDWP == JETIDBDTNOTLOOSE)
    if (!xTau.isTau(xAOD::TauJetParameters::JetBDTSigLoose))
      pass = true;
  if (pass)
  {
    m_tTST->m_aAccept.setCutResult( "JetIDWP", true );
    return true;
  }
  // ATH_MSG_VERBOSE( "Tau failed JetID working point requirement" );
  return false;
}

//___________________________SelectionCutBDTEleScore____________________________
//______________________________________________________________________________
SelectionCutBDTEleScore::SelectionCutBDTEleScore(TauSelectionTool* tTST)
  : SelectionCut("CutEleBDTScore", tTST)
{
  m_hHistCutPre = CreateControlPlot("hEleBDT_pre","EleBDT_pre;BDTEleScore; events",100,0,1);
  m_hHistCut = CreateControlPlot("hEleBDT_cut","EleBDT_cut;BDTEleScore; events",100,0,1);
}

//______________________________________________________________________________
void SelectionCutBDTEleScore::fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist)
{
  hHist.Fill(xTau.discriminant(xAOD::TauJetParameters::BDTEleScore));
}

//______________________________________________________________________________
bool SelectionCutBDTEleScore::accept(const xAOD::TauJet& xTau)
{
  // check EleBDTscore, if tau has a EleBDT score in one of the regions requiered then return true; false otherwise
  m_tTST->m_aAccept.addCut( "EleBDTScore",
                            "Selection of taus according to their EleBDTScore" );
  double dEleBDTScore = xTau.discriminant(xAOD::TauJetParameters::BDTEleScore);
  unsigned int iNumEleBDTRegion = m_tTST->m_vEleBDTRegion.size()/2;
  for( unsigned int iEleBDTRegion = 0; iEleBDTRegion < iNumEleBDTRegion; iEleBDTRegion++ )
  {
    if ( dEleBDTScore >= m_tTST->m_vEleBDTRegion.at(iEleBDTRegion*2) and dEleBDTScore <= m_tTST->m_vEleBDTRegion.at(iEleBDTRegion*2+1))
    {
      m_tTST->m_aAccept.setCutResult( "EleBDTScore", true );
      return true;
    }
  }
  // ATH_MSG_VERBOSE( "Tau failed EleBDTScore requirement, tau EleBDTScore: " << dEleBDTScore );
  return false;
}

//____________________________SelectionCutEleBDTWP______________________________
//______________________________________________________________________________
SelectionCutEleBDTWP::SelectionCutEleBDTWP(TauSelectionTool* tTST)
  : SelectionCut("CutEleBDTWP", tTST)
{
  m_hHistCutPre = CreateControlPlot("hEleBDTWP_pre","EleBDTWP_pre;; events",6,-.5,5.5);
  m_hHistCut = CreateControlPlot("hEleBDTWP_cut","EleBDTWP_cut;; events",6,-.5,5.5);
  // only proceed if histograms are defined
  if (!m_hHistCutPre or !m_hHistCut)
    return;
  m_hHistCutPre->GetXaxis()->SetBinLabel(1,"!Loose");
  m_hHistCutPre->GetXaxis()->SetBinLabel(2,"Loose");
  m_hHistCutPre->GetXaxis()->SetBinLabel(3,"!Medium");
  m_hHistCutPre->GetXaxis()->SetBinLabel(4,"Medium");
  m_hHistCutPre->GetXaxis()->SetBinLabel(5,"!Tight");
  m_hHistCutPre->GetXaxis()->SetBinLabel(6,"Tight");
  m_hHistCut->GetXaxis()->SetBinLabel(1,"!Loose");
  m_hHistCut->GetXaxis()->SetBinLabel(2,"Loose");
  m_hHistCut->GetXaxis()->SetBinLabel(3,"!Medium");
  m_hHistCut->GetXaxis()->SetBinLabel(4,"Medium");
  m_hHistCut->GetXaxis()->SetBinLabel(5,"!Tight");
  m_hHistCut->GetXaxis()->SetBinLabel(6,"Tight");
}

//______________________________________________________________________________
void SelectionCutEleBDTWP::fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist)
{
  hHist.Fill(!xTau.isTau(xAOD::TauJetParameters::ElectronVetoLoose));
  hHist.Fill(!xTau.isTau(xAOD::TauJetParameters::ElectronVetoMedium)+2);
  hHist.Fill(!xTau.isTau(xAOD::TauJetParameters::ElectronVetoTight)+4);
}

//______________________________________________________________________________
bool SelectionCutEleBDTWP::accept(const xAOD::TauJet& xTau)
{
  // check EleBDTscore, if tau passes EleBDT working point then return true; false otherwise
  m_tTST->m_aAccept.addCut( "EleBDTWP",
                            "Selection of taus according to their EleBDTScore" );
  bool pass = false;
  if (m_tTST->m_iEleBDTWP == ELEIDBDTLOOSE)
    if (!xTau.isTau(xAOD::TauJetParameters::EleBDTLoose ))
      pass = true;
  if (m_tTST->m_iEleBDTWP == ELEIDBDTMEDIUM)
    if (!xTau.isTau(xAOD::TauJetParameters::EleBDTMedium ))
      pass = true;
  if (m_tTST->m_iEleBDTWP == ELEIDBDTTIGHT)
    if (!xTau.isTau(xAOD::TauJetParameters::EleBDTTight ))
      pass = true;
  if (pass)
  {
    m_tTST->m_aAccept.setCutResult( "EleBDTWP", true );
    return true;
  }
  // ATH_MSG_VERBOSE( "Tau failed EleBDT working point requirement" );
  return false;
}


//____________________________SelectionCutMuonVeto______________________________
//______________________________________________________________________________
SelectionCutMuonVeto::SelectionCutMuonVeto(TauSelectionTool* tTST)
  : SelectionCut("CutMuonVeto", tTST)
{
  m_hHistCutPre = CreateControlPlot("hMuonVeto_pre","MuonVeto_pre;; events",2,-.5,1.5);
  m_hHistCut = CreateControlPlot("hMuonVeto_cut","MuonVeto_cut;; events",2,-.5,1.5);
  // only proceed if histograms are defined
  if (!m_hHistCutPre or !m_hHistCut)
    return;
  m_hHistCutPre->GetXaxis()->SetBinLabel(1,"!MuonVeto");
  m_hHistCutPre->GetXaxis()->SetBinLabel(2,"MuonVeto");
  m_hHistCut->GetXaxis()->SetBinLabel(1,"!MuonVeto");
  m_hHistCut->GetXaxis()->SetBinLabel(2,"MuonVeto");
}

//______________________________________________________________________________
void SelectionCutMuonVeto::fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist)
{
  hHist.Fill(!xTau.isTau(xAOD::TauJetParameters::MuonVeto));
}

//______________________________________________________________________________
bool SelectionCutMuonVeto::accept(const xAOD::TauJet& xTau)
{
  // check EleBDTscore, if tau passes EleBDT working point then return true; false otherwise
  m_tTST->m_aAccept.addCut( "MuonVeto",
                            "Selection of taus according to their MuonVeto" );
  if (!xTau.isTau(xAOD::TauJetParameters::MuonVeto ))
  {
    m_tTST->m_aAccept.setCutResult( "MuonVeto", true );
    return true;
  }
  // ATH_MSG_VERBOSE( "Tau failed EleBDT working point requirement" );
  return false;
}

#endif // SELECTIONCUTS_CXX
